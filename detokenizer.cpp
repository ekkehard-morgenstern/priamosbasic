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

#include "detokenizer.h"
#include "keywords.h"

Detokenizer::Detokenizer( const uint8_t* pos ) : scan(pos), buf(DETOK_BUFSZ) {}

Detokenizer::~Detokenizer() {}

const char* Detokenizer::detokenize() {

    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok == T_EOL ) break;

        if ( !buf.writeByte(UINT8_C(32)) ) return 0;

        switch ( tok ) {
            uint32_t lineNo; char tmp[100]; int n;
            const uint8_t* text; uint8_t len; double val;
            const char* text2;
            case T_LINENO:
                if ( !scan.getLineNo( lineNo ) ) return 0;
                n = snprintf( tmp, sizeof(tmp), "%" PRIu32, lineNo );
                if ( !buf.writeBlock( tmp, n ) ) return 0;
                break;
            case T_IDENT: 
                if ( !scan.getText( text, len ) ) return 0;
                if ( !buf.writeBlock( text, len ) ) return 0;
                break;
            case T_STRLIT: 
                if ( !buf.writeByte(UINT8_C(34)) ) return 0;
                if ( !scan.getText( text, len ) ) return 0;
                if ( !buf.writeBlock( text, len ) ) return 0;
                if ( !buf.writeByte(UINT8_C(34)) ) return 0;
                break;
            case T_LABEL:
                if ( !scan.getText( text, len ) ) return 0;
                if ( !buf.writeBlock( text, len ) ) return 0;
                if ( !buf.writeByte(UINT8_C(58)) ) return 0;
                break;
            case T_NUMLIT: case T_SBI:
                if ( !scan.getNumber( val ) ) return 0;
                n = snprintf( tmp, sizeof(tmp), "%g", val );
                if ( !buf.writeBlock( tmp, n ) ) return 0;
                break;
            case T_REM:
                text2 = Keywords::getInstance().lookup( tok );
                if ( text2 == 0 ) return 0;
                if ( !buf.writeBlock( text2, strlen(text2) ) ) return 0;
                if ( !buf.writeByte(UINT8_C(32)) ) return 0;
                if ( !scan.getText( text, len ) ) return 0;
                if ( !buf.writeBlock( text, len ) ) return 0;
                break;
            default:
                if ( tok >= UINT16_C(0X0100) || tok == T_PRINT ) {
                    text2 = Keywords::getInstance().lookup( tok );
                    if ( text2 == 0 ) return 0;
                    if ( !buf.writeBlock( text2, strlen(text2) ) ) return 0;
                } else {
                    if ( !buf.writeByte( (uint8_t) tok ) ) return 0;
                }
                break;
        }

        if ( !scan.skipTok() ) return 0;
    }

    if ( !buf.writeByte(0) ) return 0;

    return (const char*) buf.getBaseAddr();
}