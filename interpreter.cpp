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

CmdHashEnt::CmdHashEnt( uint16_t& tok_, CmdMethodPtr mth_ )
    :   HashEntry( (const uint8_t*)(&tok_), 2U ), mth(mth_) {}
CmdHashEnt::~CmdHashEnt() {}

const CmdDecl Interpreter::declTable[] = {
    { KW_LIST, &Interpreter::list },
    { 0, 0 }
};

bool Interpreter::getIdentInfo( TokenScanner& scan ) {
    uint16_t tok = scan.tokType();
    if ( tok != T_IDENT ) return false;

    return false;
}

bool Interpreter::getLineNo( TokenScanner& scan, uint32_t& rLineNo ) {
   uint16_t tok = scan.tokType();
    if ( tok != T_LINENO ) return false;
    if ( !scan.getLineNo( rLineNo ) ) return false;
    if ( !scan.skipTok() ) return false;
    return true;
 }

bool Interpreter::getLineNoExpr( TokenScanner& scan, uint32_t& lineNo1, uint32_t& lineNo2 ) {
    bool hadFirst = getLineNo( scan, lineNo1 );
    uint16_t tok = scan.tokType();
    if ( tok == T_MINUS ) {
        if ( !scan.skipTok() ) return false;
        getLineNo( scan, lineNo2 );
    } else if ( hadFirst ) {
        lineNo2 = lineNo1;
    }
    return true;
}

void Interpreter::list( TokenScanner& scan ) {
    uint32_t lineNo1 = 0;
    uint32_t lineNo2 = UINT32_MAX;
    getLineNoExpr( scan, lineNo1, lineNo2 );
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

void Interpreter::declare( uint16_t tok_, CmdMethodPtr mth_ ) {
    uint16_t tok = tok_;
    commandHt.enter( new CmdHashEnt( tok, mth_ ) );
}

void Interpreter::declare() {
    for ( int i=0; declTable[i].tok; ++i ) {
        declare( declTable[i].tok, declTable[i].mth );
    }
}

Interpreter::Interpreter() {    
    declare();
}

Interpreter::~Interpreter() {
}

void Interpreter::interpret( TokenScanner& scan ) {
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
            (this->*mth)( scan );
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
    TokenScanner scan( t.getTokBufAddr() );
    tok = scan.tokType();
    if ( tok == T_LINENO ) {
        uint32_t lineNo;
        if ( !scan.getLineNo( lineNo ) ) return;
        prog.enterLine( t );
        return;
    }
    interpret( scan );
}
