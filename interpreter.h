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

#ifndef VARIABLES_H
#include "variables.h"
#endif

#ifndef PROGRAM_H
#include "program.h"
#endif

class Interpreter;
typedef void (Interpreter::*CmdMethodPtr)();

struct CmdHashEnt : public HashEntry {

    CmdMethodPtr mth;   // method in Interpreter class

    CmdHashEnt( uint16_t& tok_, CmdMethodPtr mth_ );
    virtual ~CmdHashEnt();
};

struct CmdDecl { uint16_t tok; CmdMethodPtr mth; };

typedef void (Interpreter::*FnMethodPtr)();


struct FnDecl { 
    uint16_t    tok;    // keyword token
    FuncType    type;   // type of function
    uint8_t     nForm;  // number of formal arguments
    uint8_t     nOpt;   // number of optional arguments
    uint8_t     nRes;   // number of results
    bool        bVarArgs;   // variable arguments list?
    FnMethodPtr mth; 
};

struct FnArg : public FuncArg { // for FuncVal
    Interpreter*    intp;
    FnMethodPtr     mth; 
};

#define IIF_STR     1
#define IIF_FN      2
#define IIF_ARY     4

struct IdentInfo {
    const uint8_t*  name;
    uint8_t         nLen;
    ValDesc*        desc;
    int             flags;  // see II* definitions above
};

class Interpreter : public NonCopyable {

    Program         prog;
    HashTable       commandHt;
    Variables       vars;
    TokenScanner    scan;

    static const CmdDecl cmdDeclTable[];
    static const FnDecl funcDeclTable[];

    bool getIdentInfo( IdentInfo& ii );

    bool getLineNo( uint32_t& rLineNo );
    bool getLineNoExpr( uint32_t& lineNo1, uint32_t& lineNo2 );
    void list();

    static void funcHandler( FuncArg* arg );

    void declareCmd( const CmdDecl& decl );
    void declareFunc( const FnDecl& decl );

    void declare(); // declare all of the built-in commands

    // interpret a line of tokens (direct mode)
    void interpret();

public:
    Interpreter();
    virtual ~Interpreter();

    // interpret a line in direct mode
    void interpretLine( const char* line );
    
};



#endif