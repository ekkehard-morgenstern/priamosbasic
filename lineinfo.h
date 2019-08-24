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

#ifndef LINEINFO_H
#define LINEINFO_H  1

#ifndef TYPES_H
#include "types.h"
#endif

#define MINLINEINFO    1024U

struct LineInfo {
    uint32_t lineNo;    // line number
    uint32_t offset;    // offset in program buffer
    uint32_t length;    // length of line in bytes (incl. T_EOL)
    uint32_t pad_;      // (padding)
};

class LineInfoManager : public NonCopyable {

    LineInfo*   info;
    size_t      count;
    size_t      alloc;
    uint32_t    lastLineNumber;
    bool        haveLastLineNumber;

    void expand();

public:
    LineInfoManager( size_t minLineInfo );
    virtual ~LineInfoManager();

    inline size_t getCount() const { return count; }
    
    inline LineInfo& getAt( size_t pos ) {
        return info[pos];
    }

    inline const LineInfo& getAt( size_t pos ) const {
        return info[pos];
    }

    void append( const LineInfo& src );
    void insert( const LineInfo& src, size_t pos );
    void insert( const LineInfo& src );
    void deleteAt( size_t pos );
    void deleteLine( uint32_t lineNo );

};

#endif
