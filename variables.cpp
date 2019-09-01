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

#include "variables.h"
#include "exception.h"
#include "tokenizer.h"


ValDesc::ValDesc( ValueType type_ ) : type(type_) {}
ValDesc::~ValDesc() {}

ValDesc* ValDesc::create( ValueType type_, ... ) {
    switch ( type_ ) {
        case VT_INT: return new IntVal();
        case VT_REAL: return new RealVal();
        case VT_STR: return new StrVal();
        default: break;
    }
    return 0;
}

double ValDesc::getNumVal() const { return 0; }
void ValDesc::setNumVal( double val ) {}

void ValDesc::getStrVal( uint8_t*& rPtr, size_t& rLen, bool& rFree ) const {
    static uint8_t b;
    rPtr = &b; rLen = 0; rFree = false;
}

void ValDesc::setStrVal( const uint8_t* ptr, size_t len ) {}

IntVal::IntVal() : ValDesc(VT_INT), value(0) {}
IntVal::~IntVal() { value = 0; }

double IntVal::getNumVal() const { return (double) value; }
void IntVal::setNumVal( double val ) { value = (int64_t) trunc( val ); }

void IntVal::getStrVal( uint8_t*& rPtr, size_t& rLen, bool& rFree ) const {    
    format( rPtr, rLen, "%" PRId64, value );
    rFree = true;
}

void IntVal::setStrVal( const uint8_t* ptr, size_t len ) {
    Tokenizer t( ptr, len );
    uint16_t tok = t.nextTok(); bool minus = false;
    if ( tok == T_MINUS ) {
        minus = true;
        tok   = t.nextTok();
    } else if ( tok == T_PLUS ) {
        tok   = t.nextTok();
    }
    if ( tok == T_NUMLIT ) {
        if ( t.numIsInt() ) {
            value = t.numIVal();
        } else {
            value = (int64_t) trunc( t.numRVal() );
        }
        if ( minus ) value = -value;
    } else {
        value = 0;
    }
}

RealVal::RealVal() : ValDesc(VT_REAL), value(0) {}
RealVal::~RealVal() { value = 0; }

double RealVal::getNumVal() const { return value; }
void RealVal::setNumVal( double val ) { value = val; }

void RealVal::getStrVal( uint8_t*& rPtr, size_t& rLen, bool& rFree ) const {
    format( rPtr, rLen, "%g", value );
    rFree = true;
}

void RealVal::setStrVal( const uint8_t* ptr, size_t len ) {
    Tokenizer t( ptr, len );
    uint16_t tok = t.nextTok(); bool minus = false;
    if ( tok == T_MINUS ) {
        minus = true;
        tok   = t.nextTok();
    } else if ( tok == T_PLUS ) {
        tok   = t.nextTok();
    }
    if ( tok == T_NUMLIT ) {
        if ( t.numIsInt() ) {
            value = t.numIVal();
        } else {
            value = t.numRVal();
        }
        if ( minus ) value = -value;
    } else {
        value = 0;
    }
}

StrVal::StrVal() : ValDesc(VT_STR) {
    text = new uint8_t [0];
    len  = 0;
}

StrVal::StrVal( const uint8_t* text_, size_t len_ ) 
    : ValDesc(VT_STR) {
    text = new uint8_t [ len_ ];
    len  = len_;
    if ( len ) memcpy( text, text_, len );
}
    
StrVal::~StrVal() {
    delete [] text; text = 0; len = 0;
}

double StrVal::getNumVal() const { 
    RealVal tmp;
    tmp.setStrVal( text, len );
    return tmp.value;
}

void StrVal::setNumVal( double val ) {
    delete [] text;
    format( text, len, "%g", val );
}

void StrVal::getStrVal( uint8_t*& rPtr, size_t& rLen, bool& rFree ) const {
    rPtr = text; rLen = len; rFree = false;
}

void StrVal::setStrVal( const uint8_t* ptr, size_t len_ ) {
    delete [] text;
    text = new uint8_t [ len_ ];
    if ( len_ ) memcpy( text, ptr, len_ );
    len  = len_;
}

void AryVal::init() {
    if ( elemType == VT_ARY || elemType == VT_FUNC ) {
        throw Exception( "array type impossible" );
    }
    if ( arrayType != AT_STATIC && ndims != 1U ) {
        throw Exception( "array type impossible" );
    }
    totalSize = 0;
    for ( size_t i=0; i < ndims; ++i ) {
        size_t dim = dims[i];
        if ( dim == 0 ) {
            throw Exception( "dimension #%d is zero", (int) i );
        }
        if ( totalSize > SIZE_MAX / dim ) {
            throw Exception( "dimension #%d too large", (int) i );
        }
        totalSize *= dim;
    }
    cells = new ValDesc* [ totalSize ];
    for ( size_t i=0; i < totalSize; i++ ) {
        cells[i] = ValDesc::create( elemType );
    }
    if ( arrayType == AT_ASSOC ) {
        ht = new HashTable();
    } else {
        ht = 0;
    }
}

AryVal::AryVal( va_list ap ) : ValDesc(VT_ARY) {
    elemType  = (ValueType) va_arg( ap, int );
    arrayType = (ArrayType) va_arg( ap, int );
    ndims     = va_arg( ap, size_t );
    dims      = new size_t [ ndims ];
    for ( size_t i=0; i < ndims; ++i ) {
        dims[i] = va_arg( ap, size_t );
    }
    init();
}

AryVal::AryVal( ValueType elemType_, ArrayType arrayType_, size_t ndims_, ... ) 
    : ValDesc(VT_ARY), elemType(elemType_), arrayType(arrayType_), ndims(ndims_) {
    va_list ap;
    va_start( ap, ndims_ );
    dims = new size_t [ ndims ];
    for ( size_t i=0; i < ndims; ++i ) {
        dims[i] = va_arg( ap, size_t );
    }
    va_end( ap );
    init();
}

AryVal::AryVal( ValueType elemType_, ArrayType arrayType_, size_t ndims_, 
    const size_t* dims_ ) : ValDesc(VT_ARY), elemType(elemType_), arrayType(arrayType_),
    ndims(ndims_) {
    dims = new size_t [ ndims ];
    if ( ndims ) memcpy( dims, dims_, sizeof(size_t) * ndims );
    init();
}

AryVal::~AryVal() {
    if ( ht ) { delete ht; ht = 0; }
    while ( totalSize ) {
        --totalSize;
        if ( cells[totalSize] ) { 
            delete cells[totalSize]; cells[totalSize] = 0; 
        }
    }
    delete [] cells; cells = 0; 
    delete [] dims; dims = 0; ndims = 0;
    elemType = VT_UNDEF;
}

FuncVal::FuncVal( va_list ap ) : ValDesc(VT_FUNC) {
    type     = (FuncType) va_arg( ap, int );
    nForm    = (uint8_t ) va_arg( ap, int );
    nOpt     = (uint8_t ) va_arg( ap, int );
    nRes     = (uint8_t ) va_arg( ap, int );
    bVarArgs = (bool    ) va_arg( ap, int );
    pFunc    = va_arg( ap, FuncPtr  );
    pFuncArg = va_arg( ap, FuncArg* );
}

FuncVal::FuncVal( FuncType type_, uint8_t nForm_, uint8_t nOpt_,
    uint8_t nRes_, bool bVarArgs_, FuncPtr pFunc_, 
    FuncArg* pFuncArg_ ) : ValDesc(VT_FUNC), type(type_),
    nForm(nForm_), nOpt(nOpt_), nRes(nRes_), bVarArgs(bVarArgs_),
    pFunc(pFunc_), pFuncArg(pFuncArg_) {}

FuncVal::~FuncVal() {
    type = FT_UNDEF; nForm = nOpt = nRes = 0; bVarArgs = false;
    pFunc = 0; 
    if ( pFuncArg ) { delete pFuncArg; pFuncArg = 0; }
}

VarDesc::VarDesc( const uint8_t* name, size_t nameLen,
    ValDesc* valueDesc_ ) : HashEntry( name, nameLen ),
    valueDesc(valueDesc_) {}

VarDesc::~VarDesc() { 
    if ( valueDesc ) { delete valueDesc; valueDesc = 0; }
}

Variables::Variables() {}
Variables::~Variables() {}

bool Variables::addVar( const uint8_t* name, size_t nameLen, 
    ValDesc* desc ) {
    if ( ht.find( name, nameLen ) ) return false;
    
    ht.enter( new VarDesc( name, nameLen, desc ) );

    return true;
}

bool Variables::remVar( const uint8_t* name, size_t nameLen ) {

    HashEntry* ent = ht.find( name, nameLen );
    if ( ent == 0 ) return false;

    ht.remove( ent );

    delete ent;
    return true;
}

ValDesc* Variables::findVar( const uint8_t* name, size_t nameLen ) {

    HashEntry* ent = ht.find( name, nameLen );
    if ( ent == 0 ) return 0;

    VarDesc* desc = dynamic_cast<VarDesc*>( ent );
    if ( desc == 0 ) return 0;

    return desc->valueDesc;
}

