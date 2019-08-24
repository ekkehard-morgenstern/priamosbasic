/*  PriamosBASIC - a BASIC LineInfoManager written in C++
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

#include "lineinfo.h"

void LineInfoManager::expand() {
    size_t    newAlloc = alloc * 2U;
    LineInfo* newInfo  = new LineInfo[ newAlloc ];
    if ( count ) {
        memcpy( newInfo, info, sizeof(LineInfo) * count );
    }
    delete [] info;
    info  = newInfo;
    alloc = newAlloc;
}

LineInfoManager::LineInfoManager( size_t minlineinfo ) {
    info               = new LineInfo [ minlineinfo ];
    count              = 0;
    alloc              = minlineinfo;;
    lastLineNumber     = 0;
    haveLastLineNumber = false;
}

LineInfoManager::~LineInfoManager() {
    delete [] info; info = 0; count = alloc = 0;
    lastLineNumber     = 0;
    haveLastLineNumber = false;
}

void LineInfoManager::append( const LineInfo& src ) {
    if ( count >= alloc ) expand();
    info[count++] = src;
    lastLineNumber = src.lineNo;
    haveLastLineNumber = true;
}

void LineInfoManager::insert( const LineInfo& src, size_t pos ) {
    if ( pos >= count ) {
        append( src );
        return;
    }
    if ( count >= alloc ) expand();
    size_t remain = count - pos;
    memmove( &info[pos+1], &info[pos], sizeof(LineInfo) * remain );
    info[pos] = src;
    count++;
}

void LineInfoManager::insert( const LineInfo& src ) {
    if ( !haveLastLineNumber || src.lineNo > lastLineNumber ) {
        append( src );
        return;
    }
    if ( src.lineNo == lastLineNumber ) {
        info[count-1] = src;
        return;
    }
    for ( size_t pos=0; pos < count; ++pos ) {
        if ( src.lineNo == info[pos].lineNo ) {
            info[pos] = src;
            return;
        }
        if ( src.lineNo < info[pos].lineNo ) {
            insert( src, pos );
            return;
        }
    }
    append( src );
}

void LineInfoManager::deleteAt( size_t pos ) {
    if ( pos >= count ) return;
    if ( pos == count-1U ) {
        --count;
        return;
    }
    size_t remain = count-1U-pos;
    memmove( &info[pos], &info[pos+1], sizeof(LineInfo) * remain );
    --count;
}

void LineInfoManager::deleteLine( uint32_t lineNo ) {
    for ( size_t pos=0; pos < count; ++pos ) {
        if ( info[pos].lineNo == lineNo ) {
            deleteAt( pos );
            break;
        }
    }
    if ( haveLastLineNumber && lastLineNumber == lineNo ) {
        if ( count > 0U ) {
            lastLineNumber = info[count-1U].lineNo;
        } else {
            lastLineNumber = 0;
            haveLastLineNumber = false;
        }
    }
}

