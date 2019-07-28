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

#ifndef VARIABLES_H
#define VARIABLES_H     1

#ifndef HASHTABLE_H
#include "hashtable.h"
#endif

enum ValueType {
    VT_UNDEF,   // undefined
    VT_INT,     // an integer variable
    VT_REAL,    // a floating-point variable
    VT_STR,     // a string variable
    VT_ARY,     // an array variable
    VT_FUNC,    // a function variable
};

enum FuncType {
    FT_SYS,     // a system function
    FT_USR,     // a user-defined function (machine code)
    FT_BAS_FN,  // a user-defined BASIC function (DEF FN)
    FT_BAS_FUNC, // a user-defined BASIC function (FUNC)
    FT_BAS_SUB, // a user-defined BASIC subroutine / procedure (SUB)
};

struct ValDesc : public NonCopyable {

    ValueType   type;

    ValDesc( ValueType type_ );
    virtual ~ValDesc();

    static ValDesc* create( ValueType type_, ... );
};

struct IntVal : public ValDesc {
    int64_t     value;

    IntVal();
    virtual ~IntVal();
};

struct RealVal : public ValDesc {
    double     value;

    RealVal();
    virtual ~RealVal();
};

struct StrVal : public ValDesc {
    uint8_t*    text;
    size_t      len;

    StrVal();
    StrVal( const uint8_t* text_, size_t len_ );
    virtual ~StrVal();
};

struct AryVal : public ValDesc {
    ValueType   elemType;   // element type
    size_t      dim;        // dimension (size)
    ValDesc**   cells;      // array cells

    AryVal( ValueType elemType_, size_t dim_ = 10 );
    virtual ~AryVal();
};

struct FuncVal {
    FuncType    type;   // type of function
    uint8_t     nForm;  // number of formal arguments
    uint8_t     nOpt;   // number of optional arguments
    uint8_t     nRes;   // number of results
    bool        bVarArgs;   // variable arguments list?
    size_t      pFrag;  // offset in code memory
    size_t      szFrag; // size of code fragment
};

struct VarDesc : public HashEntry {    // variable descriptor
    ValDesc*    valueDesc;
};

#define INITIAL_DESCBUF_SIZE    131072U

class Variables : public NonCopyable {

    HashTable ht;

public:


};


#endif
