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
#include "keywords.h"


Tokenizer::Tokenizer( const uint8_t* source_, size_t sourceLen_ ) 
    : source(source_), pos(source_), sourceLen(sourceLen_),
      sourceEnd(source_ + sourceLen_) {

}

Tokenizer::~Tokenizer() {
    source = pos = 0; sourceLen = 0;
}

void Tokenizer::readIdent( uint8_t b ) {
    int len = 0;
    do {
        if ( len < MAXIDENT-2 ) ident[len++] = b;
        if ( ++pos >= sourceEnd ) break;
        b = *pos;
    } while ( ( b >= UINT8_C(0X41) && b <= UINT8_C(0X5A) ) ||
        ( b >= UINT8_C(0X61) && b <= UINT8_C(0X7A) ) ||
        ( b >= UINT8_C(0X30) && b <= UINT8_C(0X39) ) );
    if ( b == '$' ) do {
        ident[len++] = b;
        if ( ++pos >= sourceEnd ) break;
        b = *pos;
    } while (0);
    if ( b == '(' ) {
        ident[len++] = b;
        ++pos;
    }
    idLen = len;
}

uint16_t Tokenizer::nextTok() {

     uint8_t b; uint16_t t;

     if ( pos >= sourceEnd ) return T_EOL;

     b = *pos;
     if ( ( b >= UINT8_C(0X41) && b <= UINT8_C(0X5A) ) ||
          ( b >= UINT8_C(0X61) && b <= UINT8_C(0X7A) ) ) {
          readIdent( b );
          t = Keywords::getInstance().lookup( ident, idLen );
          if ( t != KW_NOTFOUND ) return t;
          return T_IDENT;
     }

     if ( b == UINT8_C(0X20) || b == UINT8_C(0X08) ) {
          do {
              if ( ++pos >= sourceEnd ) break;
              b = *pos;
          } while ( b == UINT8_C(0X20) || b == UINT8_C(0X08) );
          return T_SPC;
     }


}


