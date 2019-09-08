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

typedef void (Interpreter::*FnMethodPtr)( FuncArg* );


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

#define IIF_INT     1
#define IIF_STR     2
#define IIF_FN      4
#define IIF_ARY     8

struct ExprList;

struct IdentInfo : public NonCopyable {
    const uint8_t*  name;
    uint8_t         nLen;
    ValDesc*        desc;
    int             flags;  // see II* definitions above
    ExprList*       param;  // arguments
    IdentInfo();
    ~IdentInfo();
};

struct ExprInfo : public NonCopyable {
    ExprInfo*       next;
    ExprList*       param;  // arguments
    ValDesc*        value;  // transient -- might be discarded
    bool            bFree;  // whether to discard value after use
    ExprInfo( ValDesc* value_, bool bFree_ );
    ~ExprInfo();

    inline ValDesc* detachValue() { ValDesc* ret = value; value = 0; return ret; }

    void promoteIntToReal();
    void demoteRealToInt();
    void changeStrToInt();
};

struct ExprList : public NonCopyable {
    ExprInfo*       first;
    ExprInfo*       last;
    ExprList();
    ~ExprList();
    void add( ExprInfo* expr );
    void moveFrom( ExprList* exprList );
    void addFirst( ExprInfo* expr );
    size_t count() const;
};

class Interpreter : public NonCopyable {

    Program         prog;
    HashTable       commandHt;
    Variables       vars;
    TokenScanner    scan;

    static const CmdDecl cmdDeclTable[];
    static const FnDecl funcDeclTable[];

    /* expressions:

        -- 'identifier' here means an identifier of which the purpose
        -- has not been determined yet (undeclared variable). If it's used
        -- as a regular variable, it can be declared on the fly, otherwise
        -- it would be an error (array not dimensioned / function not declared).
        -- small functions declared with DEF FN can be recognized by the FN
        -- modifier keyword.
        -- 'func-kw' refers to a 'functional keyword', i.e. a built-in function.

        num-ident := [ 'FN' ] num-identifier | num-variable | num-func-kw .
        str-ident := [ 'FN' ] str-identifier | str-variable | str-func-kw .

        -- in this syntax, the '(' is expressly mentioned for consistency, although 
        -- in reality, it is actually part of the identifier name.

        num-ident-expr := num-ident [ '(' expr-list ')' ] .
        str-ident-expr := str-ident [ '(' expr-list ')' ] .

        -- operator precedence checked with the ANSI BASIC standard.
        -- Priamos BASIC offers more operators, so we're basing our
        -- precedence on some common sense from other languages too.
        -- For instance, in ANSI BASIC *only*, things like X^-Y have
        -- to be put in parenthesis: X^(-Y), while in all Microsoft
        -- BASIC derivates, they don't. 

        num-base-expr := num-ident-expr | num-const | '(' num-expr ')' .
        sign-op       := '-' | '+' .
        signed-expr   := [ sign-op ] num-base-expr .
        not-op        := 'NOT' .
        not-expr      := [ not-op ] signed-expr .
        mult-op       := '*' | '/' .
        mult-expr     := not-expr { mult-op not-expr } .
        pow-op        := '**' | '^' .
        pow-expr      := mult-expr { pow-op mult-expr } .
        add-op        := '-' | '+' .
        add-expr      := pow-expr { add-op pow-expr } .
        shift-op      := 'SHL' | 'SHR' .
        shift-expr    := add-expr [ shift-op add-expr ] .
        cmp-op        := '<' | '>' | '<=' | '>=' | '=' | '<>' .
        cmp-expr      := shift-expr [ cmp-op shift-expr ] .
        num-expr      := cmp-expr .

        -- string expressions

        str-base-expr := str-ident-expr | str-const .
        concat-op     := '+' .
        concat-expr   := str-base-expr { concat-op str-base-expr } .
        str-expr      := str-cmp-expr .

        -- general expressions

        base-expr     := num-expr | str-expr .
        and-op        := 'AND' | 'NAND' .
        and-expr      := cmp-expr { and-op cmp-expr } .
        or-op         := 'OR' | 'XOR' | 'NOR' | 'XNOR' .
        or-expr       := and-expr { log-op and-expr } .
        expr          := or-expr .
        expr-list     := expr { ',' expr } .

        -- assignments
        -- LEFT$(), MID$(), RIGHT$() etc. have a special status as they can
        -- be used on the left side of assignments. Is automatically handled
        -- by the syntax here by the definition of str-ident-expr.

        assign-lvalue  := num-ident-expr | str-ident-expr .
        lvalue-list    := assign-lvalue { ',' assign-lvalue } .
        assignment     := [ 'LET' ] lvalue-list '=' expr-list .

        TODO: perhaps move this to an extra file, 'expressions.ebnf' or so.
    */

    void skipTok();
        // convenience function that skips the current token.

    bool getIdentInfo( IdentInfo& ii );
        // getIdentInfo() peeks at the current token without swallowing it.
        // A potential preceding 'FN' keyword is not skipped, and the main identifier 
        // or variable token is not skipped either. Both must be done by the caller.

    void getIdentAuto( IdentInfo& ii, ValueType vt );
        // auto-creates simple variables for IdentInfo (used by getNumIdent/getStrIdent)

    bool getNumIdent( IdentInfo& ii );
        // gets a numeric identifier at the current position, creating it on the fly,
        // if possible. The token(s) belonging to the identifier are skipped.

    bool getStrIdent( IdentInfo& ii );
        // gets a string identifier at the current position, creating it on the fly,
        // if possible. The token(s) belonging to the identifier are skipped.

    void getIdentArgs( IdentInfo& ii );
        // gets parameters for ident expr (used by getNumIdentExpr/getStrIdentExpr)

    static void fillFuncArgs( FnArg& args, ExprList* el );
        // moves arguments from expression list to FnArg object.

    static void fillFuncRes( ExprList* el, FnArg& args );
        // moves arguments from FnArg object to expression list.

    static void verifyFuncArgs( FuncVal* fn, const FnArg& args );
        // verifies that number of arguments is accurate for function call.

    static bool verifyFuncRes( FuncVal* fn, const FnArg& args );
        // verifies that number of results is accurate for function call.

    ExprList* evalIdentExpr( IdentInfo& ii, ValueType vt );
        // evaluates an identifier expression

    bool getNumIdentExpr( IdentInfo& ii );
        // gets a numeric identifier, possibly with arguments.

    bool getStrIdentExpr( IdentInfo& ii );
        // gets a string identifier, possibly with arguments.

    ExprList* getNumBaseExpr();
        // gets a numeric base expression (incl. evaluation)

    ExprList* getStrBaseExpr();
        // gets a string base expression (incl. evaluation)

    static void verifySingleNumber( ExprList* el );
        // assures that ExprList contains only a single, numeric value

    ExprList* getSignedExpr();
        // gets a numeric signed expression (incl. evaluation)

    ExprList* getNotExpr();
        // gets a numeric NOT expression (incl. evaluation)

    static void autoPromote( ExprInfo* ei1, ExprInfo* ei2, bool harder = false );
        // promote from VT_INT to VT_REAL if necessary

    static void autoDemote( ExprInfo* ei1, ExprInfo* ei2 );
        // demote from VT_REAL to VT_INT if necessary

    ExprList* getMultExpr();
        // gets a numeric multiplication expression (incl. evaluation)

    ExprList* getPowExpr();
        // gets a numeric power expression (incl. evaluation)

    ExprList* getAddExpr();
        // gets a numeric addition expression (incl. evaluation)

    ExprList* getShiftExpr();
        // gets a shift expression (incl. evaluation)

    ExprList* getCmpExpr();
        // gets a comparison expression (incl. evaluation)

    ExprList* getNumExpr();
        // gets a numeric expression with a transient ValDesc

    static void verifySingleString( ExprList* el );
        // verifies that an expression is a single string.

    ExprList* getConcatExpr();
        // gets a string concat expression

    ExprList* getStrCmpExpr();
        // gets a string comparison expression

    ExprList* getStrExpr();
        // gets a string expression with a transient ValDesc

    ExprList* getBaseExpr();
        // gets a numeric or string expression
    
    ExprList* getAndExpr();
        // gets an AND expression (incl. evaluation)

    ExprList* getOrExpr();
        // gets an OR expression (incl. evaluation)

    ExprList* getExpr();
        // gets any expression with a transient ValDesc

    ExprList* getExprList();
        // gets any expression list

    ExprList* getAssignLvalue();
        // gets an lvalue for assignment

    ExprList* getLvalueList();
        // gets a list of lvalues for assignment

    bool      getAssignment( ExprList*& lvalues, ExprList*& rvalues );
        // get lvalues and rvalues for assignment

    bool getLineNo( uint32_t& rLineNo );
    bool getLineNoExpr( uint32_t& lineNo1, uint32_t& lineNo2 );
    void list();

    static void funcHandler( FuncArg* arg );

    void declareCmd( const CmdDecl& decl );
    void declareFunc( const FnDecl& decl );

    void declare(); // declare all of the built-in commands
    void clearVars( bool declareOnly = false ); 
        // clear all variables, then redeclare built-in stuff

    // interpret a line of tokens (direct mode)
    void interpret();

public:
    Interpreter();
    virtual ~Interpreter();

    // interpret a line in direct mode
    void interpretLine( const char* line );
    
};



#endif