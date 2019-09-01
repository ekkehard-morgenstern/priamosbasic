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

IdentInfo::IdentInfo() : name(0), nLen(0), desc(0), flags(0), param(0) {}

IdentInfo::~IdentInfo() {
    name = 0; nLen = 0; desc = 0; flags = 0;
    if ( param ) { delete param; param = 0; }
}

ExprInfo::ExprInfo() : next(0), value(0) {}

ExprInfo::~ExprInfo() {
    if ( next ) { delete next; next = 0; }
    delete value; 
}

ExprList::ExprList() : first(0), last(0) {}

ExprList::~ExprList() {
    if ( first ) { delete first; first = 0; }
    last = 0;
}
    
void ExprList::add( ExprInfo* expr ) {
    if ( !last ) {
        first = last = expr;
    } else {
        last->next = expr;
        last       = expr;
    }
}

const CmdDecl Interpreter::cmdDeclTable[] = {
    { KW_LIST, &Interpreter::list },
    { 0, 0 }
};

const FnDecl Interpreter::funcDeclTable[] = {
    { 0, FT_UNDEF, 0, 0, 0, false, 0 }
};

void Interpreter::skipTok() {
    if ( !scan.skipTok() ) {
        throw Exception( "interpret error: bad token" );
    }
}

bool Interpreter::getIdentInfo( IdentInfo& ii ) {
    const uint8_t* pos = scan.getPos();
    uint16_t tok = scan.tokType(); bool isFN = false;
    if ( tok == KW_FN ) {   // small function reference
        skipTok();
        tok = scan.tokType();
        if ( tok != T_IDENT ) {
            throw Exception( "syntax error: identifier expected" );
        }
        isFN = true;
        goto IDENT;
    } else if ( tok == T_IDENT ) { // variable or function (declared, or undeclared)
IDENT:  ii.name = 0; ii.nLen = 0;
        if ( !scan.getText( ii.name, ii.nLen ) || ii.name == 0 || 
            ii.nLen == 0 ) {
            throw Exception( "interpret error: token error" );
        }
        ii.desc  = vars.findVar( ii.name, ii.nLen );
    } else if ( ISFUNCKW( tok ) ) { // functional keyword (built-in function)
        const char* text = Keywords::getInstance().lookup( tok );
        if ( text == 0 ) {
            throw Exception( "interpret error: keyword not registered" );
        }
        ii.name = (const uint8_t*) text;
        ii.nLen = ii.name[-1];
        ii.desc = vars.findVar( ii.name, ii.nLen );
        if ( ii.desc == 0 ) {
            throw Exception( "interpret error: function keyword not implemented" );
        }
    } else {
        return false;
    }
    ii.flags = 0;
    if ( ii.desc == 0 ) {
        // undeclared variable or function: guess intended type
        uint8_t x = ii.nLen - UINT8_C(1);
        if ( ii.name[x] == UINT8_C(0X28) ) {  // (
            if ( isFN ) {
                ii.flags |= IIF_FN;
            } else {
                ii.flags |= IIF_ARY | IIF_FN;
            }
            --x;
        }
        if ( ii.name[x] == UINT8_C(0X24) ) {  // $
            ii.flags |= IIF_STR;
        }
    } else {
        // known variable or function: read type
        ValueType vt = ii.desc->type;
        if ( isFN ) {
            if ( vt == VT_FUNC ) {
                FuncVal* fv = dynamic_cast<FuncVal*>( ii.desc );
                if ( fv ) {
                    if ( fv->type == FT_BAS_FN ) {
                        ii.flags |= IIF_FN;
                    } else {
                        throw Exception( "syntax error: not FN-declared" );
                    }
                } else {
                    throw Exception( "interpret error: bad function" );
                }
            } else {
                throw Exception( "syntax error: FN with non-function" );
            }
        } else {
            switch ( vt ) {
                case VT_STR:    ii.flags |= IIF_STR; break;
                case VT_ARY:    ii.flags |= IIF_ARY; break;
                case VT_FUNC:   ii.flags |= IIF_FN;  break;
                default:        break;
            }
        }
    }
    scan.setPos( pos ); // rewind
    return true;
}

void Interpreter::getIdentAuto( IdentInfo& ii, ValueType vt ) {

    if ( ii.desc == 0 ) {
        if ( ii.flags & IIF_ARY ) {
            throw Exception( "array not dimensioned" );
        }
        if ( ii.flags & IIF_FN  ) {
            throw Exception( "function not declared" );
        }
        // simple variable: auto-declare it
        ii.desc = ValDesc::create( vt );
        if ( !vars.addVar( ii.name, ii.nLen, ii.desc ) ) {
            throw Exception( "interpret error: failed to add variable" );
        }
    }

    // skip token(s)
    uint16_t tok = scan.tokType();
    if ( tok == KW_FN ) {   // small function reference
        skipTok();
    }

    skipTok();
}

bool Interpreter::getNumIdent( IdentInfo& ii ) {
    // num-ident := [ 'FN' ] num-identifier | num-variable | num-func-kw .

    if ( !getIdentInfo( ii ) ) return false;
    if ( ii.flags & IIF_STR  ) return false;

    getIdentAuto( ii, VT_INT );
    return true;
}

bool Interpreter::getStrIdent( IdentInfo& ii ) {
    // str-ident := [ 'FN' ] str-identifier | str-variable | str-func-kw .

    if ( !getIdentInfo( ii )         ) return false;
    if ( ( ii.flags & IIF_STR ) == 0 ) return false;

    getIdentAuto( ii, VT_STR );
    return true;
}

void Interpreter::getIdentArgs( IdentInfo& ii ) {

    if ( ii.flags & ( IIF_ARY | IIF_FN ) ) {
        ExprList* el = getExprList();
        uint16_t tok = scan.tokType();
        if ( tok != T_RPAREN ) {
            delete el;
            throw Exception( "syntax error: closing parenthesis ')' expected" );
        }
        if ( !scan.skipTok() ) {
            delete el;
            throw Exception( "interpret error: bad token" );
        }
        ii.param = el;
    }
}
    
bool Interpreter::getNumIdentExpr( IdentInfo& ii ) {
    // num-ident-expr := num-ident [ '(' expr-list ')' ] .

    if ( !getNumIdent( ii ) ) return false;

    getIdentArgs( ii );
    return true;
}

bool Interpreter::getStrIdentExpr( IdentInfo& ii ) {
    // str-ident-expr := str-ident [ '(' expr-list ')' ] .

    if ( !getStrIdent( ii ) ) return false;

    getIdentArgs( ii );
    return true;
}

ExprInfo* Interpreter::evalIdentExpr( IdentInfo& ii, ValueType vt ) {

    if ( ii.desc == 0 ) {
        throw Exception( "interpret error: unexpected value" );
    }

    ExprInfo* ei = new ExprInfo;
    ei->value = ValDesc::create( vt );

    switch ( ii.desc->type ) {
        uint8_t* ptr; size_t len; bool bFree;
        case VT_INT:
        case VT_REAL:
            ei->value->setNumVal( ii.desc->getNumVal() );
            break;
        case VT_STR:
            ptr = 0; len = 0; bFree = false;
            ii.desc->getStrVal( ptr, len, bFree );
            ei->value->setStrVal( ptr, len );
            if ( bFree ) delete [] ptr;
            break;
        case VT_ARY:
        case VT_FUNC:
        default:
            delete ei;
            throw Exception( "interpret error: not implemented" );
    }

    return ei;
}

ExprInfo* Interpreter::getNumBaseExpr() {
    // num-base-expr := num-ident-expr | num-const | '(' num-expr ')' .

    IdentInfo ii;
    if ( getNumIdentExpr( ii ) ) {
        // identifier expression, array and function
        return evalIdentExpr( ii, VT_REAL );
    }

    // TBD
    return 0;
}

ExprInfo* Interpreter::getStrBaseExpr() {
    // str-base-expr := str-ident-expr | str-const .

    IdentInfo ii;
    if ( getStrIdentExpr( ii ) ) {
        // identifier expression, array and function
        return evalIdentExpr( ii, VT_STR );
    }


    // TBD
    return 0;
}

ExprInfo* Interpreter::getNumExpr() {
    // TBD
    return 0;
}

ExprInfo* Interpreter::getStrExpr() {
    // TBD
    return 0;
}

ExprInfo* Interpreter::getExpr() {
    ExprInfo* ei = getNumExpr();
    if ( ei ) return ei;
    return getStrExpr();
}

ExprList* Interpreter::getExprList() {

    ExprInfo* ei = getExpr();
    if ( ei == 0 ) return 0;

    ExprList* el = new ExprList();

    for (;;) {
        el->add( ei );

        uint16_t tok = scan.tokType();
        if ( tok != T_COMMA ) break;

        if ( !scan.skipTok() ) {
            delete el;
            throw Exception( "interpret error: bad token" );
        }

        ei = getExpr();
        if ( ei == 0 ) {
            delete el;
            throw Exception( "syntax error: expression expected after comma" );
        }
    }

    return el;
}

bool Interpreter::getLineNo( uint32_t& rLineNo ) {
   uint16_t tok = scan.tokType();
    if ( tok != T_LINENO ) return false;
    if ( !scan.getLineNo( rLineNo ) ) return false;
    scan.skipTok();
    return true;
 }

bool Interpreter::getLineNoExpr( uint32_t& lineNo1, uint32_t& lineNo2 ) {
    bool hadFirst = getLineNo( lineNo1 );
    uint16_t tok = scan.tokType();
    if ( tok == T_MINUS ) {
        skipTok();
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
        if ( ptr == 0 ) throw Exception( "list error: bad read" );
        Detokenizer d( ptr );
        const char* text = d.detokenize();
        if ( text == 0 ) throw Exception( "list error: detokenization failed" );
        printf( "%s\n", text );
    }
}

void Interpreter::funcHandler( FuncArg* arg ) {
    FnArg* fnArg = dynamic_cast<FnArg*>( arg );
    if ( fnArg == 0 ) throw Exception( "call error: bad function" );
    FnMethodPtr mth = fnArg->mth;
    (fnArg->intp->*mth)();
}

void Interpreter::declareCmd( const CmdDecl& decl ) {
    uint16_t tok = decl.tok;
    commandHt.enter( new CmdHashEnt( tok, decl.mth ) );
}

void Interpreter::declareFunc( const FnDecl& decl ) {
    const char* name0 = Keywords::getInstance().lookup( decl.tok );
    if ( name0 == 0 ) throw Exception( "declare error: keyword not found" );
    const uint8_t* name    = (const uint8_t*) name0;
    size_t         nameLen = name[-1];
    FnArg* arg = new FnArg;
    arg->intp = this;
    arg->mth  = decl.mth;
    FuncVal* val = new FuncVal( decl.type, decl.nForm, decl.nOpt, decl.nRes,
        decl.bVarArgs, funcHandler, arg );
    if ( !vars.addVar( name, nameLen, val ) ) {
        throw Exception( "declare error: failed to add variable" );
    }    
}

void Interpreter::declare() {
    for ( int i=0; cmdDeclTable[i].tok; ++i ) {
        declareCmd( cmdDeclTable[i] );
    }
    clearVars( true );
}

void Interpreter::clearVars( bool declareOnly ) {
    if ( !declareOnly ) vars.clear();
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
        skipTok();
        if ( tok == T_LINENO || tok == T_LABEL || tok == T_COLON ) continue;
        HashEntry* he = commandHt.find( (const uint8_t*)(&tok), 2U );
        if ( he ) {
            CmdHashEnt* cmd = dynamic_cast<CmdHashEnt*>( he );
            if ( cmd == 0 ) {
                throw Exception( "interpret error: bad command" );
            }
            CmdMethodPtr mth = cmd->mth;
            (this->*mth)();
            continue;
        }
        throw Exception( "interpret error: command not implemented" );
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
