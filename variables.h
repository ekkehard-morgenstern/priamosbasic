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

enum ArrayType {
    AT_STATIC,  // regular, static array (multiple dimensions)
    AT_DYNAMIC, // 1-dimensional dynamic array
    AT_ASSOC    // 1-dimensional associative array
};

enum FuncType {
    FT_UNDEF,   // undefined
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

    virtual int64_t getIntVal() const;
    virtual void setIntVal( int64_t val );

    virtual double getRealVal() const;
    virtual void setRealVal( double val );

    virtual void getStrVal( uint8_t*& rPtr, size_t& rLen, bool& rFree ) const;
    virtual void setStrVal( const uint8_t* ptr, size_t len );

    virtual void alu( uint16_t op );
    virtual void alu( uint16_t op, ValDesc* arg );
};

struct IntVal : public ValDesc {
    int64_t     value;

    IntVal();
    virtual ~IntVal();

    virtual int64_t getIntVal() const;
    virtual void setIntVal( int64_t val );

    virtual double getRealVal() const;
    virtual void setRealVal( double val );

    virtual void getStrVal( uint8_t*& rPtr, size_t& rLen, bool& rFree ) const;
    virtual void setStrVal( const uint8_t* ptr, size_t len );

    virtual void alu( uint16_t op );
    virtual void alu( uint16_t op, ValDesc* arg );
};

struct RealVal : public ValDesc {
    double     value;

    RealVal();
    virtual ~RealVal();

    virtual int64_t getIntVal() const;
    virtual void setIntVal( int64_t val );

    virtual double getRealVal() const;
    virtual void setRealVal( double val );

    virtual void getStrVal( uint8_t*& rPtr, size_t& rLen, bool& rFree ) const;
    virtual void setStrVal( const uint8_t* ptr, size_t len );

    virtual void alu( uint16_t op );
    virtual void alu( uint16_t op, ValDesc* arg );
};

struct StrVal : public ValDesc {
    uint8_t*    text;
    size_t      len;
    bool        bFree;  // true = regular; false = constant (don't delete)

    StrVal();
    StrVal( const uint8_t* text_, size_t len_, bool bFree_ );
    virtual ~StrVal();

    virtual int64_t getIntVal() const;
    virtual void setIntVal( int64_t val );

    virtual double getRealVal() const;
    virtual void setRealVal( double val );

    virtual void getStrVal( uint8_t*& rPtr, size_t& rLen, bool& rFree ) const;
    virtual void setStrVal( const uint8_t* ptr, size_t len_, bool bFree_ );

    virtual void alu( uint16_t op, ValDesc* arg );
};

struct AryVal : public ValDesc {
    ValueType   elemType;   // element type
    ArrayType   arrayType;  // array type
    size_t      ndims;      // number of dimensions
    size_t      totalSize;  // number of total cells
    size_t*     dims;       // dimensions (sizes)
    ValDesc**   cells;      // array cells
    HashTable*  ht;         // hash table for associative arrays

    AryVal( va_list ap );
    AryVal( ValueType elemType_, ArrayType arrayType_, size_t ndims_, ... );
    AryVal( ValueType elemType_, ArrayType arrayType_, size_t ndims_, 
        const size_t* dims_ );
    virtual ~AryVal();

private:
    void init();
};

struct FuncArg : public NonCopyable {

    ValDesc**   pArgs;  // arguments
    bool*       fArgs;  // free argument?
    size_t      nArgs;  // number used
    size_t      aArgs;  // number allocated

    ValDesc**   pRes;   // results
    bool*       fRes;   // free result?
    size_t      nRes;   // number used
    size_t      aRes;   // number allocated

    FuncArg();
    virtual ~FuncArg();

    void addArg( ValDesc* arg, bool fFree );
    void addRes( ValDesc* res, bool fFree );

    inline ValDesc* detachResultBackwards() {
        if ( nRes == 0 ) return 0;
        return pRes[--nRes];
    }

};

typedef void (*FuncPtr)( FuncArg* );

struct FuncVal : public ValDesc {
    FuncType    type;   // type of function
    uint8_t     nForm;  // number of formal arguments
    uint8_t     nOpt;   // number of optional arguments
    uint8_t     nRes;   // number of results
    bool        bVarArgs;   // variable arguments list?
    // callable function in system space (must be specified)
    FuncPtr     pFunc;      // function address
    FuncArg*    pFuncArg;   // function argument

    FuncVal( va_list ap );
    FuncVal( FuncType type_, uint8_t nForm_, uint8_t nOpt_,
        uint8_t nRes_, bool bVarArgs_, FuncPtr pFunc_, 
        FuncArg* pFuncArg_ );
    virtual ~FuncVal();

    void call();
};

struct VarDesc : public HashEntry {    // variable descriptor
    ValDesc*    valueDesc;

    VarDesc( const uint8_t* name, size_t nameLen,
        ValDesc* valueDesc_ );
    virtual ~VarDesc();
};

#define INITIAL_DESCBUF_SIZE    131072U

class Variables : public NonCopyable {

    HashTable ht;

public:
    Variables();
    ~Variables();

    bool addVar( const uint8_t* name, size_t nameLen, 
        ValDesc* desc );

    bool remVar( const uint8_t* name, size_t nameLen );

    inline void clear() { ht.clear(); }

    ValDesc* findVar( const uint8_t* name, size_t nameLen );

};


#endif
