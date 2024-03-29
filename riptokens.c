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

    FILE* fpt = fopen( "tokens.lst", "wt" );
    if ( fpt == 0 ) return EXIT_FAILURE;

    FILE* fpn = fopen( "names.lst", "wt" );
    if ( fpn == 0 ) return EXIT_FAILURE;

    char lbuf[1024];
    while ( fgets( lbuf, sizeof(lbuf), fp ) ) {

        char h1[3], h2[3], name[17], kwname[32];
        if ( sscanf( lbuf, "$%2s $%2s %16[A-Za-z0-9$(]", h1, h2, 
            name ) == 3 && strlen(h1)==2 && strlen(h2)==2 ) {
            int len = strlen( name );
            int j   = 0;
            for ( int i=0; i < len; ++i ) {
                char c = name[i]; char c2 = '\0';
                if ( c == '$' ) c = 'S';
                else if ( c == '(' ) { c = 'F'; c2 = 'N'; }
                kwname[j++] = c;
                if ( c2 ) kwname[j++] = c2;
            }
            kwname[j] = '\0';
            char oct[4];
            snprintf( oct, sizeof(oct), "%0o", len );
            fprintf( fpt, "#define KW_%s 0X%s%s\n", kwname, h1, h2  );
            fprintf( fpn, "{ \"\\%s%s\", KW_%s },\n", oct, name,
                kwname );
        }

    }

    fclose( fpt );
    fclose( fpn );
    fclose( fp );

    return EXIT_SUCCESS;
}