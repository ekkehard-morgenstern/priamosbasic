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
      sourceLen(sourceLen_), outBuf( TOKBUFSZ ) {}

Tokenizer::~Tokenizer() {
    source = pos = 0; sourceLen = 0;
}

void Tokenizer::readIdent( uint8_t b ) {
    int len = 0;
    do {
        if ( b >= UINT8_C(0X61) && b <= UINT8_C(0X7A) ) b -= UINT8_C(0X20);
        if ( len < MAXIDENT-2 ) ident[len++] = b;
        if ( ++pos >= sourceEnd ) break;
        b = *pos;
    } while ( ( b >= UINT8_C(0X41) && b <= UINT8_C(0X5A) ) ||   // A..Z
        ( b >= UINT8_C(0X61) && b <= UINT8_C(0X7A) ) ||         // a..z
        ( b >= UINT8_C(0X30) && b <= UINT8_C(0X39) ) );         // 0..9
    if ( b == UINT8_C(0X24) || b == UINT8_C(0X25) ) do {    // $, %
        ident[len++] = b;
        if ( ++pos >= sourceEnd ) break;
        b = *pos;
    } while (0);
    if ( b == UINT8_C(0X28) ) { // (
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

REDO:

    if ( pos >= sourceEnd ) return T_EOL;

    b = *pos;

    switch ( b ) {
        case UINT8_C(0X21): ++pos; return T_PLING;      // !
        case UINT8_C(0X24): ++pos; return readNum( 16 );    // $
        case UINT8_C(0X25): ++pos; return readNum( 2 ); // %
        case UINT8_C(0X27): ++pos; return T_REM;        // ', REM
        case UINT8_C(0X28): ++pos; return T_LPAREN;     // (
        case UINT8_C(0X29): ++pos; return T_RPAREN;     // )
        case UINT8_C(0X2A): ++pos; 
            if ( pos >= sourceEnd ) return T_SYNERR;
            b = *pos;
            if ( b == UINT8_C(0X2A) ) { ++pos; return T_POW; }  // **, ^
            return T_TIMES;                             // *
        case UINT8_C(0X2B): ++pos; 
            if ( pos >= sourceEnd ) return T_SYNERR;
            b = *pos;
            if ( b == UINT8_C(0X2B) ) { ++pos; return KW_INC; } // ++, INC
            return T_PLUS;                              // +
        case UINT8_C(0X2C): ++pos; return T_COMMA;      // ,
        case UINT8_C(0X2D): ++pos; 
            if ( pos >= sourceEnd ) return T_SYNERR;
            b = *pos;
            if ( b == UINT8_C(0X2D) ) { ++pos; return KW_DEC; } // --, DEC
            return T_MINUS;                             // -
        case UINT8_C(0X2F): ++pos; return T_DIV;        // /
        case UINT8_C(0X3A): ++pos; return T_COLON;      // :
        case UINT8_C(0X3B): ++pos; return T_SEMIC;      // ;
        case UINT8_C(0X3C): ++pos;
            if ( pos >= sourceEnd ) return T_SYNERR;
            b = *pos;
            if ( b == UINT8_C(0X3D) ) { ++pos; return T_LE; } // <=
            if ( b == UINT8_C(0X3C) ) { ++pos; return KW_SHL; } // <<, SHL
            return T_LT;                                // <
        case UINT8_C(0X3D): ++pos;  return T_EQ;        // =
        case UINT8_C(0X3E): ++pos;
            if ( pos >= sourceEnd ) return T_SYNERR;
            b = *pos;
            if ( b == UINT8_C(0X3D) ) { ++pos; return T_GE; } // >=
            if ( b == UINT8_C(0X3E) ) { ++pos; return KW_SHR; } // >>, SHR
            return T_GT;                                // >
        case UINT8_C(0X3F): ++pos; return T_PRINT;      // ?, PRINT
        case UINT8_C(0X40): ++pos; return readNum( 8 ); // @
        case UINT8_C(0X5B): ++pos; return T_LBRACK;     // [
        case UINT8_C(0X5D): ++pos; return T_RBRACK;     // ]
        case UINT8_C(0X5E): ++pos; return T_POW;        // ^, **
        case UINT8_C(0X7B): ++pos; return T_LBRACE;     // {
        case UINT8_C(0X7C): ++pos; return T_LEXT;       // |
        case UINT8_C(0X7D): ++pos; return T_RBRACE;     // }
        default: break;
    }

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
        goto REDO;
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

    return T_SYNERR;
}

bool Tokenizer::storeLineNo() {
    if ( !outBuf.writeByte( T_LINENO ) ) return false;
    return outBuf.writeLineNo( (uint32_t) intVal );
}

bool Tokenizer::storeInt() {
    uint8_t loNyb;
    if ( intVal <= INT8_MAX ) {
        loNyb = NL_I8;
    } else if ( intVal <= INT16_MAX ) {
        loNyb = NL_I16;
    } else if ( intVal <= INT32_MAX ) {
        loNyb = NL_I32;
    } else {
        loNyb = NL_I64;
    }
    uint8_t hiNyb;
    if ( numBase == 10 ) {
        hiNyb = NH_DEC;
    } else if ( numBase == 16 ) {
        hiNyb = NH_HEX;
    } else if ( numBase == 8 ) {
        hiNyb = NH_OCT;
    } else if ( numBase == 2 ) {
        hiNyb = NH_BIN;
    } else {
        return false;
    }
    if ( loNyb == NL_I8 && hiNyb == NH_DEC ) {
        // special encoding for single-byte integer
        if ( !outBuf.writeByte( T_SBI ) ) return false;
        return outBuf.writeByte( (int8_t) intVal );
    }
    if ( !outBuf.writeByte( T_NUMLIT ) ) return false;
    if ( !outBuf.writeByte( hiNyb | loNyb ) ) return false;
    if ( loNyb == NL_I8 ) {
        return outBuf.writeByte( (uint8_t) intVal );
    } else if ( loNyb == NL_I16 ) {
        return outBuf.writeWord( (uint16_t) intVal );
    } else if ( loNyb == NL_I32 ) {
        return outBuf.writeDWord( (uint32_t) intVal );
    } 
    return outBuf.writeQWord( (uint64_t) intVal );
}

bool Tokenizer::storeReal() {
    U_IntReal64 ir;
    ir.rval = realVal;
    // for IEEE double-precision format,
    // 11 bits are for the exponent, and 52 bits are for the mantissa
    int16_t  exp = (int16_t)( ( ir.ival >> UINT8_C(52) ) & 2047U );
    uint64_t man = ir.ival & UINT64_C(0X000FFFFFFFFFFFFF);
    bool     cnv = false;
    bool     nan = false;
    if ( exp == INT16_C(0) ) {
        // signed zero (man==0) or subnormal (man!=0)
        // zero can be converted and subnormals might
        cnv = true;
    } else if ( exp == INT16_C(2047) ) {
        // infinity (man==0) and NaN (man!=0)
        // can both be converted
        cnv = true; if ( man ) nan = true;
    } else {
        // rebase exponent to correct base
        exp -= INT16_C(1023);
        // check if exponent is within single-precision range
        if ( exp >= INT16_C(-126) && exp <= INT16_C(127) ) {
            cnv = true;
        }
    }
    // check if the mantissa uses the lower 52-23 = 29 bits
    // which would mean it requires more precision (applies to
    // subnormals and regular numbers)
    if ( cnv && !nan ) {
        if ( man & UINT32_C(0X1FFFFFFF) ) cnv = false;
    }
    uint8_t loNyb;
    if ( cnv ) {
        loNyb = NL_F32;
    } else {
        loNyb = NL_F64;
    }
    uint8_t hiNyb;
    if ( numBase == 10 ) {
        hiNyb = NH_DEC;
    } else if ( numBase == 16 ) {
        hiNyb = NH_HEX;
    } else if ( numBase == 8 ) {
        hiNyb = NH_OCT;
    } else if ( numBase == 2 ) {
        hiNyb = NH_BIN;
    } else {
        return false;
    }
    if ( !outBuf.writeByte( T_NUMLIT ) ) return false;
    if ( !outBuf.writeByte( hiNyb | loNyb ) ) return false;
    if ( cnv ) {
        return outBuf.writeReal32( (float) realVal );
    }
    return outBuf.writeReal64( realVal );
}

uint16_t Tokenizer::tokenize() {

    bool first = true;

    for (;;) {
        uint16_t tok = nextTok();
        if ( tok == T_EOL || tok >= UINT16_C(0XFF00) ) {
            if ( tok == T_EOL ) {
                if ( !outBuf.writeByte( T_EOL ) ) return T_MEMERR;
            }
            return tok;
        }

        if ( first && tok == T_NUMLIT && isInt && intVal >= 0 &&
            intVal <= (int64_t) UINT24_MAX && numBase == 10 ) {
            // line number
            if ( !storeLineNo() ) return T_MEMERR;
        } else if ( first && tok == T_NUMLIT ) {
            return T_SYNERR;
        } else if ( tok == T_NUMLIT && isInt ) {
            if ( !storeInt() ) return T_MEMERR;
        } else if ( tok == T_NUMLIT ) {
            if ( !storeReal() ) return T_MEMERR;
        }

        // ... TBD ...

        first = false;
    }

}

