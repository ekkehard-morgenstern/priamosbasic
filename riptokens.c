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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main( int argc, char** argv ) {

    FILE* fp = fopen( "tokeninfo.txt", "rt" );
    if ( fp == 0 ) return EXIT_FAILURE;

    char lbuf[1024];
    while ( fgets( lbuf, sizeof(lbuf), fp ) ) {

        char h1[3], h2[3], name[17];
        if ( sscanf( lbuf, "$%2s $%2s %16[A-Za-z0-9]", h1, h2, 
            name ) == 3 && strlen(h1)==2 && strlen(h2)==2 ) {
            int len = strlen( name );
            char oct[4];
            snprintf( oct, sizeof(oct), "%0o", len );
            printf( "{ \"\\%s%s\", 0X%s%s },\n", oct, name,
                h1, h2 );
        }

    }

    fclose( fp );

    return EXIT_SUCCESS;
}