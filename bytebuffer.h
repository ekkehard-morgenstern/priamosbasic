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

#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H    1

#ifndef TYPES_H
#include "types.h"
#endif

class ByteBuffer {

    // forbid copying
    ByteBuffer( const ByteBuffer& );
    ByteBuffer& operator=( const ByteBuffer& );
    //

    uint8_t*    baseAddr;
    size_t      bufSize;
    size_t      bufFill;
    size_t      readPos;
    bool        freeMem;

    bool autoScale( size_t size = 1U );

public:
    ByteBuffer( size_t bufSize_ );
    ByteBuffer( uint8_t* baseAddr_, size_t bufSize_, size_t bufFill_ );
    ~ByteBuffer();

    inline size_t getReadPos() const { return readPos; }
    inline void* getAddr( size_t pos ) const { 
        if ( pos >= bufFill ) return 0;
        return &baseAddr[pos];
    }

    inline bool readByte( uint8_t& rOut ) {
        if ( readPos >= bufFill ) return false;
        rOut = baseAddr[readPos++];
        return true;
    }

    inline bool writeByte( uint8_t inp ) {
        if ( bufFill >= bufSize ) {
            if ( !autoScale() ) return false;
        }
        baseAddr[bufFill++] = inp;
        return true;
    }

    uint8_t* readBlock( size_t size );
    bool readBlock( void* target, size_t size );
    bool writeBlock( const void* source, size_t size );

    bool readToken( uint16_t& rOut );
    bool writeToken( uint16_t inp );

    bool readLineNo( uint32_t& rOut ); // 24 bit
    bool writeLineNo( uint32_t inp );  // 24 bit
    
};


#endif
