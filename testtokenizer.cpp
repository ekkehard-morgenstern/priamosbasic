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

#include "tokenizer.h"

int main( int argc, char** argv ) {

    char buf[1024];
    while ( fgets( buf, sizeof(buf), stdin ) ) {

        Tokenizer t( (const uint8_t*) buf, strlen( buf ) );
        for (;;) {
            uint16_t tok = t.nextTok();
            printf( "%u\n", tok );
            if ( tok >= 0XFF00U || tok == T_EOL ) {
                break;
            }
            if ( tok == T_IDENT ) {
                printf( "ident '%-*.*s'\n", (int) t.getIdentLen(),
                    (int) t.getIdentLen(), t.getIdent() );
            } else if ( tok == T_STRLIT ) {
                printf( "strlit '%-*.*s'\n", (int) t.getStrLitLen(),
                    (int) t.getStrLitLen(), t.getStrLit() );
            } else if ( tok == T_NUMLIT ) {
                if ( t.numIsInt() ) {
                    printf( "numlit %" PRId64 " base %d\n", 
                        t.numIVal(), t.numBase_() );
                } else {
                    printf( "numlit %g base %d\n", t.numRVal(), 
                        t.numBase_() );
                }
            }
        }

    }

    return EXIT_SUCCESS;
}