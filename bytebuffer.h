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

class ByteBuffer;

class BBMemMan {
public:
    virtual void compact( ByteBuffer& buf ) = 0;
};

class ByteBuffer : public NonCopyable {

    uint8_t*    baseAddr;
    size_t      bufSize;
    size_t      bufFill;
    size_t      readPos;
    bool        freeMem;
    BBMemMan*   memMgr;

    bool autoScale( size_t size = 1U );

public:
    ByteBuffer( size_t bufSize_ );
    ByteBuffer( uint8_t* baseAddr_, size_t bufSize_, size_t bufFill_ );
    virtual ~ByteBuffer();

    inline void setMemMgr( BBMemMan& mgr ) { memMgr = &mgr; }
    inline void clrMemMgr() { memMgr = 0; }

    inline size_t getReadPos() const { return readPos; }
    inline size_t getWritePos() const { return bufFill; }
    inline uint8_t* getBaseAddr() const { return baseAddr; }

    void setReadPos( size_t pos );
    void setWritePos( size_t pos );

    inline uint8_t* getAddr( size_t pos ) const { 
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

    const uint8_t* readBlock( size_t size );
    bool readBlock( void* target, size_t size );
    bool writeBlock( const void* source, size_t size );

    bool readToken( uint16_t& rOut );
    bool writeToken( uint16_t inp );

    inline bool readWord( uint16_t& rOut ) { return readToken( rOut ); }
    inline bool writeWord( uint16_t inp ) { return writeToken( inp ); }

    bool readLineNo( uint32_t& rOut ); // 24 bit
    bool writeLineNo( uint32_t inp );  // 24 bit

    bool readDWord( uint32_t& rOut );
    bool writeDWord( uint32_t inp );

    bool readReal32( float& rOut );
    bool writeReal32( float inp );

    bool readQWord( uint64_t& rOut );
    bool writeQWord( uint64_t inp );

    bool readReal64( double& rOut );
    bool writeReal64( double inp );
    
};


#endif
