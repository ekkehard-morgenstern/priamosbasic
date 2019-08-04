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

int Tokenizer::digitVal( uint8_t b, int base ) {
    uint8_t v;
    if ( b >= UINT8_C(0X30) && b <= UINT8_C(0X39) ) {
        v = b - UINT8_C(0X30);
    } else if ( b >= UINT8_C(0X41) && b <= UINT8_C(0X5A) ) {
        v = ( b - UINT8_C(0X41) ) + UINT8_C(10);
    } else if ( b >= UINT8_C(0X61) && b <= UINT8_C(0X7A) ) {
        v = ( b - UINT8_C(0X61) ) + UINT8_C(10);
    } else {
        return -1;
    }
    if ( (int) v >= base ) return -1;
    return (int) v;
}

bool Tokenizer::isDigit( uint8_t b, int base ) {
    return digitVal( b, base ) >= 0;
}

int Tokenizer::bitsPerDigit( int base ) {
    int nBits;
    switch ( base ) {
        case 2:     nBits = 1; break;
        case 8:     nBits = 3; break;
        case 16:    nBits = 4; break;
        default:    return -1;
    }
    return nBits;
}

char* Tokenizer::digitToBitGroup( char* buf, char* bufEnd, uint8_t b, 
    int base ) {
    int v = digitVal( b, base );
    if ( v < 0 ) return 0;
    int digitBits = bitsPerDigit( base );
    char* end = &buf[digitBits];
    if ( end > bufEnd ) return 0;
    while ( digitBits > 0 ) {
        int b = v & 1; v >>= 1;
        buf[ --digitBits ] = '0' + b;
    }
    return end;
}

char* Tokenizer::digitsToBitGroup( char* buf, char* bufEnd, const char* dig,
     int ndig, int base ) {
    while ( ndig ) {
        buf = digitToBitGroup( buf, bufEnd, *dig, base );
        if ( buf == 0 ) return 0;
        --ndig; ++dig;
    }
    return buf;
}

char* Tokenizer::digitsToHex( char* buf, int nbits ) {
    char* out = buf;
    while ( nbits ) {
        uint8_t b3 = buf[0] - '0';
        uint8_t b2 = buf[1] - '0';
        uint8_t b1 = buf[2] - '0';
        uint8_t b0 = buf[3] - '0';
        b3 <<= 3; b2 <<= 2; b1 <<= 1;
        uint8_t b  = b3 | b2 | b1 | b0;
        *out++ = "0123456789ABCDEF" [ b ];
        buf += 4; nbits -= 4;
    }
    return out;
}

uint16_t Tokenizer::readNum( const char* buf, int len, int base, 
    bool haveDot, bool haveExp ) {
    if ( !haveDot && !haveExp ) {
        char* endPtr = 0; errno = 0;
        #if ULONG_MAX == 18446744073709551615UL
        uint64_t v = strtoul( buf, &endPtr, base );
        #else
        uint64_t v = strtoull( buf, &endPtr, base );
        #endif
        if ( errno != 0 ) return T_NUMBAD;
        if ( (const char*) endPtr != buf + len ) return T_NUMBAD;
        if ( (int64_t) v < 0 ) return T_NUMBAD;
        intVal  = v;
        isInt   = true;
        numBase = base;
        return T_NUMLIT;
    } else if ( base == 10 ) {  // base 10 floating-point
        realVal = atof( buf );
        isInt   = false;
        numBase = 10;
        return T_NUMLIT;
    }
    // floating-point conversion for non base 10 numbers
    // bases 2, 8 and 16 can easily be converted into bit patterns
    int preDotDigits = 0, postDotDigits = 0, dotPos = -1, expPos = -1;
    if ( haveDot ) {
        const char* p = buf;
        while ( isDigit( *p, base ) ) ++p;
        // p is on dot now
        preDotDigits = (int)( p - buf );
        if ( *p != '.' ) return T_INTERR;
        dotPos = preDotDigits;
        ++p; const char* p0 = p;
        while ( isDigit( *p, base ) ) ++p;
        // p is on either exponent specifier or '\0' now
        postDotDigits = (int)( p - p0 );
        if ( haveExp ) expPos = (int)( p - buf );
    } else {
        const char* p = buf;
        while ( isDigit( *p, base ) ) ++p;
        // p is on either exponent specifier or '\0' now
        preDotDigits = (int)( p - buf );
        if ( haveExp ) expPos = preDotDigits;
    }
    char buf2[NUMBUFSZ*4+1]; int pos = 0;
    char* buf2End = &buf2[NUMBUFSZ*4];
    buf2[pos++] = '0';
    buf2[pos  ] = 'X';
    // transcribe digits into hexadecimal form, beginning with digits before dot
    char* endPtr = 0; errno = 0;
    #if ULONG_MAX == 18446744073709551615UL
    uint64_t v = strtoul( buf, &endPtr, base );
    #else
    uint64_t v = strtoull( buf, &endPtr, base );
    #endif
    if ( errno != 0 ) return T_NUMBAD;
    if ( (const char*) endPtr != buf + preDotDigits ) return T_NUMBAD;
    if ( (int64_t) v < 0 ) return T_NUMBAD;
    #if ULONG_MAX == 18446744073709551615UL
    snprintf( &buf2[2], NUMBUFSZ*4-2, "%lX", v );
    #else
    snprintf( &buf2[2], NUMBUFSZ*4-2, "%LX", v );
    #endif
    pos = (int) strlen(buf2);
    if ( haveDot ) buf2[pos++] = '.';
    if ( postDotDigits ) {
        char* start = &buf2[pos];
        char* end = digitsToBitGroup( start, buf2End, &buf[dotPos+1],
            postDotDigits, base );
        if ( end == 0 ) return T_INTERR;
        int nbits = (int)( end - start );
        // pad to 4 digit alignment
        while ( nbits & 3 ) {
            *end++ = '0'; ++nbits;
        }
        *end = '\0';
        // convert binary digits to hex
        end = digitsToHex( start, nbits );
        pos = end - &buf2[0];
    }
    if ( haveExp ) {
        buf2[pos++] = 'P';
        // library expects exponent to be in decimal form -- comply
        errno = 0;
        long ex = strtol( &buf[expPos+1], 0, base );
        if ( errno != 0 ) return T_NUMBAD;
        // need to correct exponent, since base 2 is expected
        int mult = bitsPerDigit( base ); ex *= mult;
        snprintf( &buf2[pos], NUMBUFSZ*4-pos, "%ld", ex );
        pos = strlen(buf2);
    }
    buf2[pos] = '\0';
    errno = 0;
    double v2 = strtod( buf2, 0 );
    if ( errno != 0 ) return T_NUMBAD;
    realVal = v2;
    isInt   = false;
    numBase = base;
    return T_NUMLIT;
}

uint16_t Tokenizer::readNum( uint8_t b, int base ) {
    char numBuf[NUMBUFSZ+1];
    int nbPos = 0; bool haveDot = false;
    if ( b == UINT8_C(0X2E) ) { // .
        numBuf[nbPos++] = UINT8_C(0X30);    // 0
        haveDot = true;
    }
    do {    // read digits (before/after dot)
        if ( nbPos >= NUMBUFSZ ) return T_NUMLNG;
        numBuf[nbPos++] = b;
        if ( ++pos >= sourceEnd ) break;
        b = *pos;
    } while ( isDigit( b, base ) );
    if ( !haveDot && b == UINT8_C(0X2E) ) do {  // .
        // store dot
        haveDot = true;
        if ( nbPos >= NUMBUFSZ ) return T_NUMLNG;
        numBuf[nbPos++] = b;
        // if EOL occurs right after dot
        if ( ++pos >= sourceEnd ) {
            // attempt to store at least one zero
            if ( nbPos >= NUMBUFSZ ) return T_NUMLNG;
            numBuf[nbPos++] = UINT8_C(0X30);    // 0
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
            numBuf[nbPos++] = UINT8_C(0X30);    // 0
        }        
    } while(0);
    // check for exponent (P for bases > 10, E otherwise)
    bool haveExp = false;
    if ( ( base > 10 && ( b == UINT8_C(0X50) || // P p
        b == UINT8_C(0X70) ) ) || ( b == UINT8_C(0X45) ||   // E e
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
        haveExp = true;        
    }
    numBuf[nbPos] = '\0';
    return readNum( numBuf, nbPos, base, haveDot, haveExp );
}

uint16_t Tokenizer::readNum( int base ) {
    if ( pos >= sourceEnd ) return T_NUMBAD;
    uint8_t b = *pos;
    if ( !isDigit( b, base ) && b != UINT8_C(0X2E) ) return T_NUMBAD;
    return readNum( b, base );
}

uint16_t Tokenizer::nextTok() {

    uint8_t b; uint16_t t;

    if ( pos >= sourceEnd ) return T_EOL;

    b = *pos;
    if ( ( b >= UINT8_C(0X41) && b <= UINT8_C(0X5A) ) ||    // A..Z
        ( b >= UINT8_C(0X61) && b <= UINT8_C(0X7A) ) ) {    // a..z
        readIdent( b );
        t = Keywords::getInstance().lookup( ident, idLen );
        if ( t != KW_NOTFOUND ) return t;
        return T_IDENT;
    }

    if ( b == UINT8_C(0X20) || b == UINT8_C(0X08) || 
         b == UINT8_C(0X0D) || b == UINT8_C(0X0A) ) {   // SP HT CR LF
        // CR, LF should not normally occur within a text line
        do {
            if ( ++pos >= sourceEnd ) break;
            b = *pos;
        } while ( b == UINT8_C(0X20) || b == UINT8_C(0X08) || 
         b == UINT8_C(0X0D) || b == UINT8_C(0X0A) );
        return T_SPC;
    }

    if ( b == UINT8_C(0X22) ) { // "
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

    if ( ( b >= UINT8_C(0X30) && b <= UINT8_C(0X39) ) || // 0..9
        b == UINT8_C(0X2E) ) { // .
        return readNum( b, 10 );
    }

    if ( b == UINT8_C(0X24) ) { // $
        ++pos;
        return readNum( 16 );
    }

    if ( b == UINT8_C(0X40) ) { // @
        ++pos;
        return readNum( 8 );
    }

    if ( b == UINT8_C(0X25) ) { // %
        ++pos;
        return readNum( 2 );
    }

    return T_UNIMPL;
}


