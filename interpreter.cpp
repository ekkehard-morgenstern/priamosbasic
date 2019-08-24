/*  PriamosBASIC - a BASIC interpreter written in C++
    Copyright (C) 2019  Ekkehard Morgenstern

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    NOTE: Programs created with PriamosBASIC do not fall under this license.

    CONTACT INFO:
        E-Mail: ekkehard@ekkehardmorgenstern.de
        Mail: Ekkehard Morgenstern, Mozartstr. 1, D-76744 Woerth am Rhein, Germany, Europe */

#include "interpreter.h"
#include "keywords.h"

CmdHashEnt::CmdHashEnt( uint16_t& tok_, CmdMethodPtr mth_ )
    :   HashEntry( (const uint8_t*)(&tok_), 2U ), mth(mth_) {}
CmdHashEnt::~CmdHashEnt() {}

const CmdDecl Interpreter::cmdDeclTable[] = {
    { KW_LIST, &Interpreter::list },
    { 0, 0 }
};

const FnDecl Interpreter::funcDeclTable[] = {
    { 0, FT_UNDEF, 0, 0, 0, false, 0 }
};

bool Interpreter::getIdentInfo( IdentInfo& ii ) {
    uint16_t tok = scan.tokType();
    if ( tok == T_IDENT ) {
    ii.name = 0; ii.nLen = 0;
        if ( !scan.getText( ii.name, ii.nLen ) || ii.name == 0 || 
            ii.nLen == 0 ) {
            throw Exception( "token error" );
        }
        ii.desc  = vars.findVar( ii.name, ii.nLen );
    } else if ( ISFUNCKW( tok ) ) {
        const char* text = Keywords::getInstance().lookup( tok );
        if ( text == 0 ) {
            throw Exception( "keyword not registered" );
        }
        ii.name = (const uint8_t*) text;
        ii.nLen = ii.name[-1];
        ii.desc = vars.findVar( ii.name, ii.nLen );
        if ( ii.desc == 0 ) {
            throw Exception( "function keyword not implemented" );
        }
    } else {
        return false;
    }
    ii.flags = 0;
    if ( ii.desc == 0 ) {
        uint8_t x = ii.nLen - UINT8_C(1);
        if ( ii.name[x] == UINT8_C(0X28) ) {  // (
            ii.flags |= IIF_ARY | IIF_FN;
            --x;
        }
        if ( ii.name[x] == UINT8_C(0X24) ) {  // $
            ii.flags |= IIF_STR;
        }
    } else {
        ValueType vt = ii.desc->type;
        switch ( vt ) {
            case VT_STR:    ii.flags |= IIF_STR; break;
            case VT_ARY:    ii.flags |= IIF_ARY; break;
            case VT_FUNC:   ii.flags |= IIF_FN;  break;
            default:        break;
        }
    }
    return true;
}

bool Interpreter::getLineNo( uint32_t& rLineNo ) {
   uint16_t tok = scan.tokType();
    if ( tok != T_LINENO ) return false;
    if ( !scan.getLineNo( rLineNo ) ) return false;
    if ( !scan.skipTok() ) return false;
    return true;
 }

bool Interpreter::getLineNoExpr( uint32_t& lineNo1, uint32_t& lineNo2 ) {
    bool hadFirst = getLineNo( lineNo1 );
    uint16_t tok = scan.tokType();
    if ( tok == T_MINUS ) {
        if ( !scan.skipTok() ) return false;
        getLineNo( lineNo2 );
    } else if ( hadFirst ) {
        lineNo2 = lineNo1;
    }
    return true;
}

void Interpreter::list() {
    uint32_t lineNo1 = 0;
    uint32_t lineNo2 = UINT32_MAX;
    getLineNoExpr( lineNo1, lineNo2 );
    size_t count = prog.getLineInfoCount();
    for ( size_t pos=0; pos < count; ++pos ) {
        const LineInfo& li = prog.getLineInfoAt( pos );
        if ( li.lineNo < lineNo1 || li.lineNo > lineNo2 ) continue;
        prog.setReadPos( li.offset );
        const uint8_t* ptr = prog.readBlock( li.length );
        if ( ptr == 0 ) throw Exception( "list error (type A)" );
        Detokenizer d( ptr );
        const char* text = d.detokenize();
        if ( text == 0 ) throw Exception( "list error (type B)" );
        printf( "%s\n", text );
    }
}

void Interpreter::funcHandler( FuncArg* arg ) {
    FnArg* fnArg = dynamic_cast<FnArg*>( arg );
    if ( fnArg == 0 ) throw Exception( "bad function" );
    FnMethodPtr mth = fnArg->mth;
    (fnArg->intp->*mth)( );
}

void Interpreter::declareCmd( const CmdDecl& decl ) {
    uint16_t tok = decl.tok;
    commandHt.enter( new CmdHashEnt( tok, decl.mth ) );
}

void Interpreter::declareFunc( const FnDecl& decl ) {
    const char* name0 = Keywords::getInstance().lookup( decl.tok );
    if ( name0 == 0 ) throw Exception( "keyword not found" );
    const uint8_t* name    = (const uint8_t*) name0;
    size_t         nameLen = name[-1];
    FnArg* arg = new FnArg;
    arg->intp = this;
    arg->mth  = decl.mth;
    FuncVal* val = new FuncVal( decl.type, decl.nForm, decl.nOpt, decl.nRes,
        decl.bVarArgs, funcHandler, arg );
    if ( !vars.addVar( name, nameLen, val ) ) {
        throw Exception( "failed to add var" );
    }    
}

void Interpreter::declare() {
    for ( int i=0; cmdDeclTable[i].tok; ++i ) {
        declareCmd( cmdDeclTable[i] );
    }
    for ( int i=0; funcDeclTable[i].tok; ++i ) {
        declareFunc( funcDeclTable[i] );
    }
}

Interpreter::Interpreter() {    
    declare();
}

Interpreter::~Interpreter() {
}

void Interpreter::interpret() {
    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok == T_EOL ) break;
        if ( !scan.skipTok() ) {
            throw Exception( "interpret error (type A)" );
        }
        if ( tok == T_LINENO || tok == T_LABEL || tok == T_COLON ) continue;
        HashEntry* he = commandHt.find( (const uint8_t*)(&tok), 2U );
        if ( he ) {
            CmdHashEnt* cmd = dynamic_cast<CmdHashEnt*>( he );
            if ( cmd == 0 ) {
                throw Exception( "interpret error (type B)" );
            }
            CmdMethodPtr mth = cmd->mth;
            (this->*mth)();
            continue;
        }
        throw Exception( "not implemented" );
    }
}

void Interpreter::interpretLine( const char* line ) {
    Tokenizer t( (const uint8_t*) line, strlen(line) );
    uint16_t tok = t.tokenize();
    if ( tok != T_EOL ) {
        // TODO: error handling
        printf( "error, %d\n", tok );

        return;
    }
    scan.setPos( t.getTokBufAddr() );
    tok = scan.tokType();
    if ( tok == T_LINENO ) {
        uint32_t lineNo;
        if ( !scan.getLineNo( lineNo ) ) return;
        prog.enterLine( t );
        return;
    }
    interpret();
}
