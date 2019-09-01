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

#include "types.h"

NonCopyable::NonCopyable() {}
NonCopyable::~NonCopyable() {}

double getTime() {
    struct timespec ts;
    memset( &ts, 0, sizeof(ts) );
    clock_gettime( CLOCK_MONOTONIC, &ts );
    return ( (double) ts.tv_sec ) + ( (double) ts.tv_nsec /
        1.0e9 );
}

void hexDump( const void* addr, size_t size ) {
    static const char hex[] = "0123456789ABCDEF";
    const uint8_t* ptr = (const uint8_t*) addr;
    // 0000: 00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00
    // 00000000001111111111222222222233333333334444444444555555 
    // 01234567890123456789012345678901234567890123456789012345
    //     .... .... .... ....
    // 555566666666667777777777
    // 678901234567890123456789
    char lineBuf[78];
    for ( size_t i=0; i < size; ++i ) {
        size_t cell = i % 16U;
        if ( cell == 0 ) {
            size_t offs = i;
            uint8_t b1  = (uint8_t)( ( offs >> 8U ) & 255U );
            uint8_t b2  = (uint8_t)(   offs         & 255U );
            memset( lineBuf, ' ', 77 ); lineBuf[77] = '\0';
            lineBuf[0] = hex[ ( b1 >> 4U ) & 15U ];
            lineBuf[1] = hex[   b1         & 15U ];
            lineBuf[2] = hex[ ( b2 >> 4U ) & 15U ];
            lineBuf[3] = hex[   b2         & 15U ];
            lineBuf[4] = ':';
        }
        size_t  grp = cell / 4U;
        size_t  inx = cell % 4U;
        size_t  off = 6U + grp * 13U + inx * 3U;
        uint8_t b   = ptr[i];
        lineBuf[off+0U] = hex[ ( b >> 4U ) & 15U ];
        lineBuf[off+1U] = hex[   b         & 15U ];
        off = 58U + grp * 5U + inx;
        lineBuf[off] = (char)( b >= 32U && b < 127U ? b : 46U );
        if ( cell == 15U || i == size-1U ) {
            printf( "%s\n", lineBuf );
        }
    }
}

void format( uint8_t*& rOut, size_t& rLen, const char* fmt, ... ) {
    va_list ap; va_start( ap, fmt );
    uint8_t tmp[1024]; uint8_t* tmp2 = 0;
    int len = vsnprintf( (char*) tmp, sizeof(tmp), fmt, ap );
    va_end( ap );
    if ( len < 0 ) {
        len = 0;
    } else if ( len >= 1024 ) {
        tmp2 = new uint8_t [ len + 1 ];
        va_start( ap, fmt );
        int len2 = vsnprintf( (char*) tmp, len+1, fmt, ap );
        va_end( ap );
        if ( len2 < 0 ) {
            len = 0;
        } else if ( len2 < len ) {
            len = len2;
        }
        rOut = tmp2;
        rLen = len;
        return;
    }
    rOut = new uint8_t [ len ];
    rLen = len;
    if ( len ) memcpy( rOut, tmp, len );
}
