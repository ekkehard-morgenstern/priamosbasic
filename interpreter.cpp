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

#include "interpreter.h"

CmdHashEnt::CmdHashEnt( uint16_t& tok_, CmdMethodPtr mth_ )
    :   HashEntry( (const uint8_t*)(&tok_), 2U ), mth(mth_) {}
CmdHashEnt::~CmdHashEnt() {}

void Interpreter::expandLineInfo() {
    size_t    newAlloc    = lineInfoAlloc * 2U;
    LineInfo* newLineInfo = new LineInfo[ newAlloc ];
    if ( lineInfoCount ) {
        memcpy( newLineInfo, lineInfo, sizeof(LineInfo) * lineInfoCount );
    }
    delete [] lineInfo;
    lineInfo      = newLineInfo;
    lineInfoAlloc = newAlloc;
}

void Interpreter::appendLineInfo( const LineInfo& src ) {
    if ( lineInfoCount >= lineInfoAlloc ) expandLineInfo();
    lineInfo[lineInfoCount++] = src;
    lastLineNumber = src.lineNo;
    haveLastLineNumber = true;
}

void Interpreter::insertLineInfo( const LineInfo& src, size_t pos ) {
    if ( pos >= lineInfoCount ) {
        appendLineInfo( src );
        return;
    }
    if ( lineInfoCount >= lineInfoAlloc ) expandLineInfo();
    size_t remain = lineInfoCount - pos;
    memmove( &lineInfo[pos+1], &lineInfo[pos], sizeof(LineInfo) * remain );
    lineInfo[pos] = src;
    lineInfoCount++;
}

void Interpreter::insertLineInfo( const LineInfo& src ) {
    if ( !haveLastLineNumber || src.lineNo > lastLineNumber ) {
        appendLineInfo( src );
        return;
    }
    if ( src.lineNo == lastLineNumber ) {
        lineInfo[lineInfoCount-1] = src;
        return;
    }
    for ( size_t pos=0; pos < lineInfoCount; ++pos ) {
        if ( src.lineNo == lineInfo[pos].lineNo ) {
            lineInfo[pos] = src;
            return;
        }
        if ( src.lineNo < lineInfo[pos].lineNo ) {
            insertLineInfo( src, pos );
            return;
        }
    }
    appendLineInfo( src );
}

void Interpreter::deleteLineInfoAt( size_t pos ) {
    if ( pos >= lineInfoCount ) return;
    if ( pos == lineInfoCount-1U ) {
        --lineInfoCount;
        return;
    }
    size_t remain = lineInfoCount-1U-pos;
    memmove( &lineInfo[pos], &lineInfo[pos+1], sizeof(LineInfo) * remain );
    --lineInfoCount;
}

void Interpreter::deleteLineInfo( uint32_t lineNo ) {
    for ( size_t pos=0; pos < lineInfoCount; ++pos ) {
        if ( lineInfo[pos].lineNo == lineNo ) {
            deleteLineInfoAt( pos );
            break;
        }
    }
    if ( haveLastLineNumber && lastLineNumber == lineNo ) {
        if ( lineInfoCount > 0U ) {
            lastLineNumber = lineInfo[lineInfoCount-1U].lineNo;
        } else {
            lastLineNumber = 0;
            haveLastLineNumber = false;
        }
    }
}

void Interpreter::enterLine( const Tokenizer& t ) {
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
        deleteLineInfo( lineNo );
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
    insertLineInfo( li );
}

void Interpreter::compact( ByteBuffer& buf ) {
    if ( buf.getWritePos() == 0 ) return; // ?? should NOT occur
    ByteBuffer tmp( buf.getWritePos() );
    size_t tgtPos = 0;
    for ( size_t pos=0; pos < lineInfoCount; ++pos ) {
        LineInfo& li = lineInfo[pos];
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

void Interpreter::list( TokenScanner& scan ) {
    for ( size_t pos=0; pos < lineInfoCount; ++pos ) {
        const LineInfo& li = lineInfo[pos];
        prg.setReadPos( li.offset );
        const uint8_t* ptr = prg.readBlock( li.length );
        if ( ptr == 0 ) throw Exception( "list error (type A)" );
        Detokenizer d( ptr );
        const char* text = d.detokenize();
        if ( text == 0 ) throw Exception( "list error (type B)" );
        printf( "%s\n", text );
    }
}

void Interpreter::declare( uint16_t tok_, CmdMethodPtr mth_ ) {
    uint16_t tok = tok_;
    commandHt.enter( new CmdHashEnt( tok, mth_ ) );
}

void Interpreter::declare() {
    declare( KW_LIST, &Interpreter::list );
}

Interpreter::Interpreter() : prg( INTP_PRGSIZE ) {
    lineInfo      = new LineInfo [ INTP_MINLINEINFO ];
    lineInfoCount = 0;
    lineInfoAlloc = INTP_MINLINEINFO;
    lastLineNumber = 0;
    haveLastLineNumber = false;
    prg.setMemMgr( *this );
    declare();
}

Interpreter::~Interpreter() {
    delete [] lineInfo;
}

void Interpreter::interpret( TokenScanner& scan ) {
    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok == T_EOL ) break;
        if ( !scan.skipTok() ) {
            throw Exception( "interpret error (type A)" );
        }
        if ( tok == T_LINENO || tok == T_LABEL ) continue;
        HashEntry* he = commandHt.find( (const uint8_t*)(&tok), 2U );
        if ( he ) {
            CmdHashEnt* cmd = dynamic_cast<CmdHashEnt*>( he );
            if ( cmd == 0 ) {
                throw Exception( "interpret error (type B)" );
            }
            CmdMethodPtr mth = cmd->mth;
            (this->*mth)( scan );
            continue;
        }
        throw Exception( "not implemented" );
    }
}

void Interpreter::interpretLine( const char* line ) {
    Tokenizer t( (const uint8_t*) line, strlen(line) );
    uint16_t tok = t.tokenize();
    if ( tok != T_EOL ) {
        // TODO: error handling

        return;
    }
    TokenScanner scan( t.getTokBufAddr() );
    tok = scan.tokType();
    if ( tok == T_LINENO ) {
        uint32_t lineNo;
        if ( !scan.getLineNo( lineNo ) ) return;
        enterLine( t );
        return;
    }
    interpret( scan );
}
