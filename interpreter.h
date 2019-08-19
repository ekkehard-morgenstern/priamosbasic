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

#ifndef INTERPRETER_H
#define INTERPRETER_H   1

#ifndef TYPES_H
#include "types.h"
#endif

#ifndef TOKENIZER_H
#include "tokenizer.h"
#endif

#ifndef TOKENSCANNER_H
#include "tokenscanner.h"
#endif

#ifndef EXCEPTION_H
#include "exception.h"
#endif

#ifndef HASHTABLE_H
#include "hashtable.h"
#endif

#ifndef DETOKENIZER_H
#include "detokenizer.h"
#endif

// initial program buffer size
#define INTP_PRGSIZE        16384U
#define INTP_MINLINEINFO    1024U

struct LineInfo {
    uint32_t lineNo;    // line number
    uint32_t offset;    // offset in program buffer
    uint32_t length;    // length of line in bytes (incl. T_EOL)
    uint32_t pad_;      // (padding)
};

class Interpreter;
typedef void (Interpreter::*CmdMethodPtr)( TokenScanner& scan );

struct CmdHashEnt : public HashEntry {

    CmdMethodPtr mth;   // method in Interpreter class

    CmdHashEnt( uint16_t& tok_, CmdMethodPtr mth_ );
    virtual ~CmdHashEnt();
};

class Interpreter : public NonCopyable, protected BBMemMan {

    ByteBuffer  prg;
    LineInfo*   lineInfo;
    size_t      lineInfoCount;
    size_t      lineInfoAlloc;
    uint32_t    lastLineNumber;
    bool        haveLastLineNumber;
    HashTable   commandHt;

    void expandLineInfo();
    void appendLineInfo( const LineInfo& src );
    void insertLineInfo( const LineInfo& src, size_t pos );
    void insertLineInfo( const LineInfo& src );
    void deleteLineInfoAt( size_t pos );
    void deleteLineInfo( uint32_t lineNo );
    void enterLine( const Tokenizer& t );
    virtual void compact( ByteBuffer& buf );

    static bool getLineNo( TokenScanner& scan, uint32_t& rLineNo );
    static bool getLineNoExpr( TokenScanner& scan, uint32_t& lineNo1, 
        uint32_t& lineNo2 );
    void list( TokenScanner& scan );

    void declare( uint16_t tok_, CmdMethodPtr mth_ );
    void declare(); // declare all of the built-in commands

public:
    Interpreter();
    virtual ~Interpreter();

    // interpret a line of tokens (direct mode)
    void interpret( TokenScanner& scan );

    // interpret a line in direct mode
    void interpretLine( const char* line );
    
};



#endif