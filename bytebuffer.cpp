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

#include "bytebuffer.h"

ByteBuffer::ByteBuffer( size_t bufSize_ ) : baseAddr( new uint8_t [ bufSize_ ] ), bufSize(bufSize_), bufFill(0), readPos(0),
    freeMem(true) {}

ByteBuffer::ByteBuffer( uint8_t* baseAddr_, size_t bufSize_, size_t bufFill_ ) : baseAddr(baseAddr_), bufSize(bufSize_), 
    bufFill(bufFill_), readPos(0), freeMem(false) {}

ByteBuffer::~ByteBuffer() {
    if ( freeMem ) delete [] baseAddr;
    baseAddr = 0; bufSize = bufFill = readPos = 0; freeMem = false;
}

bool ByteBuffer::readToken( uint16_t& rOut ) {
    uint8_t hi, lo;
    if ( !readByte( hi ) ) return false;
    if ( !readByte( lo ) ) return false;
    rOut = ( ((uint16_t)hi) << UINT8_C(8) ) | lo;
    return true;
}

bool ByteBuffer::writeToken( uint16_t inp ) {
    uint8_t hi = (uint8_t)( inp >> UINT8_C(8) );
    uint8_t lo = (uint8_t)  inp;
    if ( !writeByte( hi ) ) return false;
    if ( !writeByte( lo ) ) return false;
    return true;
}

bool ByteBuffer::readLineNo( uint32_t& rOut ) {
    uint8_t hi, mid, lo;
    if ( !readByte( hi  ) ) return false;
    if ( !readByte( mid ) ) return false;
    if ( !readByte( lo  ) ) return false;
    rOut = ( ((uint32_t)hi) << UINT8_C(16) ) | ( ((uint16_t)mid) << UINT8_C(8) ) | lo;
    return true;
}

bool ByteBuffer::writeLineNo( uint32_t inp ) {
    uint8_t hi  = (uint8_t)( inp >> UINT8_C(16) );
    uint8_t mid = (uint8_t)( inp >> UINT8_C( 8) );
    uint8_t lo  = (uint8_t)  inp;
    if ( !writeByte( hi  ) ) return false;
    if ( !writeByte( mid ) ) return false;
    if ( !writeByte( lo  ) ) return false;
    return true;
}

bool ByteBuffer::readDWord( uint32_t& rOut ) {
    uint16_t hi, lo;
    if ( !readWord( hi ) ) return false;
    if ( !readWord( lo ) ) return false;
    rOut = ( ((uint32_t)hi) << UINT8_C(16) ) | lo;
    return true;
}

bool ByteBuffer::writeDWord( uint32_t inp ) {
    uint16_t hi = (uint16_t)( inp >> UINT8_C(16) );
    uint16_t lo = (uint16_t)  inp;
    if ( !writeWord( hi ) ) return false;
    if ( !writeWord( lo ) ) return false;
    return true;
}

bool ByteBuffer::readReal32( float& rOut ) {
    U_IntReal32 ir;
    if ( !readDWord( ir.ival ) ) return false;
    rOut = ir.rval;
    return true;
}

bool ByteBuffer::writeReal32( float inp ) {
    U_IntReal32 ir;
    ir.rval = inp;
    if ( !writeDWord( ir.ival ) ) return false;
    return true;
}

bool ByteBuffer::readQWord( uint64_t& rOut ) {
    uint32_t hi, lo;
    if ( !readDWord( hi ) ) return false;
    if ( !readDWord( lo ) ) return false;
    rOut = ( ((uint64_t)hi) << UINT8_C(32) ) | lo;
    return true;
}

bool ByteBuffer::writeQWord( uint64_t inp ) {
    uint32_t hi = (uint32_t)( inp >> UINT8_C(32) );
    uint32_t lo = (uint32_t)  inp;
    if ( !writeDWord( hi ) ) return false;
    if ( !writeDWord( lo ) ) return false;
    return true;
}

bool ByteBuffer::readReal64( double& rOut ) {
    U_IntReal64 ir;
    if ( !readQWord( ir.ival ) ) return false;
    rOut = ir.rval;
    return true;
}

bool ByteBuffer::writeReal64( double inp ) {
    U_IntReal64 ir;
    ir.rval = inp;
    if ( !writeQWord( ir.ival ) ) return false;
    return true;
}

bool ByteBuffer::autoScale( size_t size ) {
    if ( !freeMem ) return false;
    size_t newSize = bufSize * 2U;
    if ( bufFill + size > newSize ) { newSize = bufFill + size; }
    uint8_t* newBuf = new uint8_t [ newSize ];
    if ( bufFill ) memcpy( newBuf, baseAddr, bufFill );
    delete [] baseAddr; baseAddr = newBuf; 
    bufSize = newSize;
    return true;
}

uint8_t* ByteBuffer::readBlock( size_t size ) {
    if ( readPos + size > bufFill ) return 0;
    uint8_t* pOut = new uint8_t [ size ];
    if ( size ) memcpy( pOut, &baseAddr[readPos], size );
    readPos += size;
    return pOut;
}

bool ByteBuffer::readBlock( void* target, size_t size ) {
    if ( readPos + size > bufFill ) return false;
    if ( size ) memcpy( target, &baseAddr[readPos], size );
    readPos += size;
    return true;
}

bool ByteBuffer::writeBlock( const void* source, size_t size ) {
    if ( bufFill + size > bufSize ) {
        if ( !autoScale( size ) ) return false;
    }
    if ( size ) memcpy( &baseAddr[bufFill], source, size );
    bufFill += size;
    return true;
}
