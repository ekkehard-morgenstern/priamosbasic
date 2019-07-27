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

#ifndef BYTEBUFFER_H
#include "bytebuffer.h"
#endif

#ifndef HASHTABLE_H
#include "hashtable.h"
#endif

enum VarType {
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

typedef uint64_t    IntVal;
typedef double      RealVal;

struct StrVal {
    uint8_t*    strmem;
    size_t      length;
};

struct AryVal {
    VarType     type;
    size_t      dim;        // dimension (size)
    void*       arymem;     // pointer to memory containing array elements of type VarDesc
};

struct FuncVal {
    FuncType    type;   // type of function
    uint8_t     nForm;  // number of formal arguments
    uint8_t     nOpt;   // number of optional arguments
    uint8_t     nRes;   // number of results
    bool        bVarArgs;   // variable arguments list?
    void*       pFrag;  // pointer to code fragment
    size_t      szFrag; // size of code fragment
};

struct VarDesc {    // variable descriptor
    VarDesc*    nextHash;   // next variable with same hash value
    bool        deleted;    // if this variable has been deleted
    VarType     type;
    uint8_t     nameLen;
    uint8_t     name[255];  // maximum name length
    /* followed by, depending on type:
        IntVal      intVal;
        RealVal     realVal;
        StrVal      StrVal;
        AryVal      AryVal;
        FuncVal     FuncVal;
    */
};

#define INITIAL_DESCBUF_SIZE    131072U

class Variables {

    // prevent copying
    Variables( const Variables& );
    Variables& operator=( const Variables& );
    //

    ByteBuffer  descBuf;    // descriptor buffer

public:

    void* addVar( const VarDesc& desc );

    void addVal( const IntVal& val );
    void addVal( const RealVal& val );
    void addVal( const StrVal& val );
    void addVal( const AryVal& val );
    void addVal( const FuncVal& val );

};


#endif
