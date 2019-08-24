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

#include "tokenscanner.h"

TokenScanner::TokenScanner() : pos(0) {}
TokenScanner::TokenScanner( const uint8_t* pos_ ) : pos(pos_) {}
TokenScanner::~TokenScanner() { pos = 0; }

uint16_t TokenScanner::tokType() const {
    uint8_t b = *pos;
    if ( b == T_KWBNKC || b == T_KWBNKF || b == T_KWBNKK || b == T_OPBNKO ) {
        uint8_t b2 = pos[1];
        return ( ((uint16_t)b) << UINT8_C(8) ) | b2;
    }
    return b;
}

bool TokenScanner::skipTok() {
    uint16_t tok = tokType();
    switch ( tok ) {
        uint8_t n, b;
        case T_EOL: return false;
        case T_SBI: pos += 2; break;
        case T_LINENO: pos += 4; break;
        case T_IDENT: case T_STRLIT: case T_LABEL: case T_REM:
            n = pos[1];
            pos += 2 + n;
            break;
        case T_NUMLIT:
            b = pos[1] & UINT8_C(0X0F);
            switch ( b ) {
                case NL_I8: pos += 3; break;
                case NL_I16: pos += 4; break;
                case NL_I32: case NL_F32: pos += 6; break;
                case NL_I64: case NL_F64: pos += 10; break;
                default: return false;
            }
            break;
        default:
            if ( tok > UINT16_C(0X00FF) ) { pos += 2; return true; }
            ++pos;
            break;
    }
    return true;
}

bool TokenScanner::getText( const uint8_t*& rText, uint8_t& rLen ) const {
    uint8_t b = *pos;
    switch ( b ) {
        case T_IDENT: case T_STRLIT: case T_LABEL: case T_REM:
            break;
        default:
            return false;
    }
    rText = pos + 2;
    rLen  = pos[1];
    return true;
}

bool TokenScanner::getLineNo( uint32_t& rLineNo ) const {
    uint8_t b = *pos;
    if ( b != T_LINENO ) return false;
    rLineNo = ( ((uint32_t)pos[1]) << UINT8_C(16) ) |
              ( ((uint16_t)pos[2]) << UINT8_C( 8) ) |
                           pos[3];
    return true;
}

bool TokenScanner::getNumber( double& rVal ) const {
    uint8_t b = *pos;
    if ( b == T_SBI ) {
        rVal = (int8_t) pos[1];
        return true;
    }
    if ( b != T_NUMLIT ) return false;
    b = pos[1] & UINT8_C(0X0F);
    switch ( b ) {
        case NL_I8:
            rVal = (int8_t) pos[2];
            return true;
        case NL_I16:
            rVal = (int16_t) ( ( ((uint16_t)pos[2]) << UINT8_C(8) ) | 
                pos[3] );
            return true;
        case NL_I32: case NL_F32:
            U_IntReal32 ir32;
            ir32.ival =
              ( ((uint32_t)pos[2]) << UINT8_C(24) ) |
              ( ((uint32_t)pos[3]) << UINT8_C(16) ) |
              ( ((uint16_t)pos[4]) << UINT8_C( 8) ) |
                           pos[5]                   ;
            if ( b == NL_I32 ) { rVal = (int32_t) ir32.ival; return true; }
            rVal = ir32.rval;
            return true;
        case NL_I64: case NL_F64:
            U_IntReal64 ir64;
            ir64.ival =
              ( ((uint64_t)pos[2]) << UINT8_C(56) ) |
              ( ((uint64_t)pos[3]) << UINT8_C(48) ) |
              ( ((uint64_t)pos[4]) << UINT8_C(40) ) |
              ( ((uint64_t)pos[5]) << UINT8_C(32) ) |
              ( ((uint32_t)pos[6]) << UINT8_C(24) ) |
              ( ((uint32_t)pos[7]) << UINT8_C(16) ) |
              ( ((uint16_t)pos[8]) << UINT8_C( 8) ) |
                           pos[9]                   ;
            if ( b == NL_I64 ) { rVal = (int64_t) ir64.ival; return true; }
            rVal = ir64.rval;
            return true;
        default: break;
    }
    return false;
}




