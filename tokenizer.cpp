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
    : source(source_), pos(source_), sourceEnd(source_+sourceLen_),
      sourceLen(sourceLen_) {}

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
    if ( b == '$' || b == '%' ) do {
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

bool Tokenizer::isDigit( uint8_t b, int base ) {
    uint8_t v;
    if ( b >= UINT8_C(0X30) && b <= UINT8_C(0X39) ) {
        v = b - UINT8_C(0X30);
    } else if ( b >= UINT8_C(0X41) && b <= UINT8_C(0X5A) ) {
        v = ( b - UINT8_C(0X41) ) + UINT8_C(10);
    } else if ( b >= UINT8_C(0X61) && b <= UINT8_C(0X7A) ) {
        v = ( b - UINT8_C(0X61) ) + UINT8_C(10);
    } else {
        return false;
    }
    if ( (int) v >= base ) return false;
    return true;
}

uint16_t Tokenizer::readNum( uint8_t b, int base ) {
    char numBuf[NUMBUFSZ+1];
    int nbPos = 0; bool haveDot = false;
    if ( b == UINT8_C(0X2E) ) {
        numBuf[nbPos++] = UINT8_C(0X30);
        haveDot = true;
    }
    do {    // read digits (before/after dot)
        if ( nbPos >= NUMBUFSZ ) return T_NUMLNG;
        numBuf[nbPos++] = b;
        if ( ++pos >= sourceEnd ) break;
        b = *pos;
    } while ( isDigit( b, base ) );
    if ( !haveDot && b == UINT8_C(0X2E) ) do {
        // store dot
        haveDot = true;
        if ( nbPos >= NUMBUFSZ ) return T_NUMLNG;
        numBuf[nbPos++] = b;
        // if EOL occurs right after dot
        if ( ++pos >= sourceEnd ) {
            // attempt to store at least one zero
            if ( nbPos >= NUMBUFSZ ) return T_NUMLNG;
            numBuf[nbPos++] = UINT8_C(0X30);
            break;
        }
        // otherwise, check if there's a digit after the dot
        b = *pos;
        if ( isDigit( b, base ) ) {
            // yes, continue to read and store digits
            do {
                if ( nbPos >= NUMBUFSZ ) return T_NUMLNG;
                numBuf[nbPos++] = b;
                if ( ++pos >= sourceEnd ) break;
                b = *pos;
            } while ( isDigit( b, base ) );
        } else {
            // no, store at least a zero
            if ( nbPos >= NUMBUFSZ ) return T_NUMLNG;
            numBuf[nbPos++] = UINT8_C(0X30);
        }        
    } while(0);
    // check for exponent (P for bases > 10, E otherwise)
    if ( ( base > 10 && ( b == UINT8_C(0X50) || 
        b == UINT8_C(0X70) ) ) || ( b == UINT8_C(0X45) ||
        b == UINT8_C(0X65) ) ) {
        if ( nbPos >= NUMBUFSZ ) return T_NUMLNG;
        numBuf[nbPos++] = b;
        if ( ++pos >= sourceEnd ) return T_NUMBAD;
        b = *pos;
        // check for '+' or '-'
        if ( b == UINT8_C(0X2B) ) {
            // '+': ignore
            if ( ++pos >= sourceEnd ) return T_NUMBAD;
            b = *pos;
        } else if ( b == UINT8_C(0X2D) ) {
            // '-': store
            if ( nbPos >= NUMBUFSZ ) return T_NUMLNG;
            numBuf[nbPos++] = b;
            if ( ++pos >= sourceEnd ) return T_NUMBAD;
            b = *pos;
        }
        // check for digits
        if ( !isDigit( b, base ) ) return T_NUMBAD;
        do {
            if ( nbPos >= NUMBUFSZ ) return T_NUMLNG;
            numBuf[nbPos++] = b;
            if ( ++pos >= sourceEnd ) break;
            b = *pos;
        } while ( isDigit( b, base ) );        
    }
    numBuf[nbPos] = '\0';
    printf( "%s\n", numBuf );
    return T_NUMLIT;
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

    if ( b == UINT8_C(0X22) ) {
        slLen = 0;
        do {
            if ( ++pos >= sourceEnd ) return T_STRTRM;
            if ( slLen >= MAXSTRLIT ) return T_STRLNG;
            b = *pos;
            if ( b == UINT8_C(0X22) ) { ++pos; break; }
            strlit[slLen++] = b;
        } while (1);
        return T_STRLIT;
    }

    if ( b >= UINT8_C(0X30) && b <= UINT8_C(0X39) ) {
        readNum( b, 10 );
        return T_NUMLIT;
    }

    return T_UNIMPL;
}


