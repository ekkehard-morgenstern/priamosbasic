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

int main( int argc, char** argv ) {

    Interpreter intp;
    char buf[1024];
    while ( fgets( buf, sizeof(buf), stdin ) ) {
        size_t len = strlen(buf);
        if ( len > 0U && buf[len-1U] == '\n' ) buf[--len] = '\0';

        try {
            intp.interpretLine( buf );
        }
        catch ( const Exception& xcpt ) {
            printf( "? %s\n", xcpt.what() );
        }

    }

    return EXIT_SUCCESS;
}
