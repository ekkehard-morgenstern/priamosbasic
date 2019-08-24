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

#include "program.h"
#include "exception.h"
#include "tokenscanner.h"

void Program::compact( ByteBuffer& buf ) {
    if ( buf.getWritePos() == 0 ) return; // ?? should NOT occur
    ByteBuffer tmp( buf.getWritePos() );
    size_t tgtPos = 0;
    size_t count  = lineInfo.getCount();
    for ( size_t pos=0; pos < count; ++pos ) {
        LineInfo& li = lineInfo.getAt( pos );
        buf.setReadPos( li.offset );
        const uint8_t* ptr = buf.readBlock( li.length );
        if ( ptr == 0 ) {
            throw Exception( "compact error (type A)" );
        }
        li.offset = tgtPos;
        if ( tmp.writeBlock( ptr, li.length ) ) {
            tgtPos += li.length;
        } else {
            throw Exception( "compact error (type B)" );
        }
    }
    buf.setWritePos(0);
    if ( tmp.getWritePos() > 0U ) {
        buf.clrMemMgr(); // avoid recursion (*IF* that ever happens)
        if ( !buf.writeBlock( tmp.getBaseAddr(), tmp.getWritePos() ) ) {
            throw Exception( "compact error (type C)" );
        }
        buf.setMemMgr( *this );
    }
}

Program::Program() : prg(MINPRGSIZE), lineInfo(MINLINEINFO) {
    prg.setMemMgr( *this );
}

Program::~Program() {
    prg.clrMemMgr();
}

void Program::enterLine( const Tokenizer& t ) {
    const uint8_t* addr = t.getTokBufAddr();
    size_t         size = t.getTokBufSz();
    TokenScanner scan( addr );
    uint16_t tok = scan.tokType();
    if ( tok != T_LINENO ) return;
    uint32_t lineNo;
    if ( !scan.getLineNo( lineNo ) ) return;
    if ( !scan.skipTok() ) return;
    tok = scan.tokType();
    if ( tok == T_EOL ) {
        // delete line
        lineInfo.deleteLine( lineNo );
        return;
    }
#if SIZE_MAX > UINT32_MAX
    if ( prg.getWritePos() > UINT32_MAX ) return;
#endif
    // otherwise, add or replace line
    LineInfo li;
    li.lineNo = lineNo;
    li.offset = (uint32_t) prg.getWritePos();
    if ( !prg.writeBlock( addr, size ) ) return;
#if SIZE_MAX > UINT32_MAX
    if ( prg.getWritePos() > UINT32_MAX ) return;
#endif
    li.length = ( (uint32_t) prg.getWritePos() ) - li.offset;
    lineInfo.insert( li );
}

