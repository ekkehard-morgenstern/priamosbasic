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

// --- ValDesc ------------------------------------------------------------------------

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

int64_t ValDesc::getIntVal() const { return 0; }
void ValDesc::setIntVal( int64_t val ) {}

double ValDesc::getRealVal() const { return 0; }
void ValDesc::setRealVal( double val ) {}

void ValDesc::getStrVal( uint8_t*& rPtr, size_t& rLen, bool& rFree ) const {
    static uint8_t b;
    rPtr = &b; rLen = 0; rFree = false;
}

void ValDesc::setStrVal( const uint8_t* ptr, size_t len, bool bFree_ ) {}

void ValDesc::alu( uint16_t op ) {}
void ValDesc::alu( uint16_t op, ValDesc* arg ) {}

// --- IntVal -------------------------------------------------------------------------

IntVal::IntVal() : ValDesc(VT_INT), value(0) {}
IntVal::~IntVal() { value = 0; }

int64_t IntVal::getIntVal() const { return value; }
void IntVal::setIntVal( int64_t val ) { value = val; }

double IntVal::getRealVal() const { return (double) value; }
void IntVal::setRealVal( double val ) { value = (int64_t) trunc( val ); }

void IntVal::getStrVal( uint8_t*& rPtr, size_t& rLen, bool& rFree ) const {    
    format( rPtr, rLen, "%" PRId64, value );
    rFree = true;
}

void IntVal::setStrVal( const uint8_t* ptr, size_t len, bool bFree_ ) {
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

void IntVal::alu( uint16_t op ) {
    switch ( op ) {
        case T_MINUS:   value = -value; break;
        case KW_NOT:    value = ~value; break;
        default:        break;
    }
}

void IntVal::alu( uint16_t op, ValDesc* arg ) {
    int64_t value2 = arg->getIntVal();
    switch ( op ) {
        case T_TIMES:
            value *= value2;
            break;
        case T_DIV:
            if ( value2 == 0 ) throw Exception( "division by zero" );
            value /= value2;
            break;
        case T_PLUS:
            value += value2;
            break;
        case T_MINUS:
            value -= value2;
            break;
        case KW_SHL:
            value <<= value2;
            break;
        case KW_SHR:
            value >>= value2;
            break;
        case T_EQ:
            value = ( value == value2 ? -1 : 0 );
            break;
        case T_NE:
            value = ( value != value2 ? -1 : 0 );
            break;
        case T_LT:
            value = ( value < value2 ? -1 : 0 );
            break;
        case T_GT:
            value = ( value > value2 ? -1 : 0 );
            break;
        case T_LE:
            value = ( value <= value2 ? -1 : 0 );
            break;
        case T_GE:
            value = ( value >= value2 ? -1 : 0 );
            break;
        case KW_AND:
            value &= value2;
            break;
        case KW_NAND:
            value = ~( value & value2 );
            break;
        case KW_OR:
            value |= value2;
            break;
        case KW_NOR:
            value = ~( value | value2 );
            break;
        case KW_XOR:
            value ^= value2;
            break;
        case KW_XNOR:
            value = ~( value ^ value2 );
            break;
        default:    
            break;
    }
}

// --- RealVal -----------------------------------------------------------------------

RealVal::RealVal() : ValDesc(VT_REAL), value(0) {}
RealVal::~RealVal() { value = 0; }

int64_t RealVal::getIntVal() const { return (int64_t) trunc(value); }
void RealVal::setIntVal( int64_t val ) { value = (double) val; }

double RealVal::getRealVal() const { return value; }
void RealVal::setRealVal( double val ) { value = val; }

void RealVal::getStrVal( uint8_t*& rPtr, size_t& rLen, bool& rFree ) const {
    format( rPtr, rLen, "%g", value );
    rFree = true;
}

void RealVal::setStrVal( const uint8_t* ptr, size_t len, bool bFree_ ) {
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
            value = (double) t.numIVal();
        } else {
            value = t.numRVal();
        }
        if ( minus ) value = -value;
    } else {
        value = 0;
    }
}

void RealVal::alu( uint16_t op ) {
    switch ( op ) {
        case T_MINUS:   value = -value; break;
        default:        break;
    }
}

void RealVal::alu( uint16_t op, ValDesc* arg ) {
    double value2 = arg->getRealVal();
    switch ( op ) {
        case T_TIMES:
            value *= value2;
            break;
        case T_DIV:
            if ( value2 == 0 ) throw Exception( "division by zero" );
            value /= value2;
            break;
        case T_POW:
            value = pow( value, value2 );
            break;
        case T_PLUS:
            value += value2;
            break;
        case T_MINUS:
            value -= value2;
            break;
        case T_EQ:
            value = ( value == value2 ? -1 : 0 );
            break;
        case T_NE:
            value = ( value != value2 ? -1 : 0 );
            break;
        case T_LT:
            value = ( value < value2 ? -1 : 0 );
            break;
        case T_GT:
            value = ( value > value2 ? -1 : 0 );
            break;
        case T_LE:
            value = ( value <= value2 ? -1 : 0 );
            break;
        case T_GE:
            value = ( value >= value2 ? -1 : 0 );
            break;
        default:    
            break;
    }
}

// --- StrVal ------------------------------------------------------------------------

StrVal::StrVal() : ValDesc(VT_STR) {
    text  = new uint8_t [0];
    len   = 0;
    bFree = true;
}

StrVal::StrVal( const uint8_t* text_, size_t len_, bool bFree_ ) 
    : ValDesc(VT_STR) {
    if ( bFree_ ) {
        text = new uint8_t [ len_ ];
        len  = len_;
        if ( len ) memcpy( text, text_, len );
    } else {
        text = (uint8_t*) text_;
        len  = len_;
    }
    bFree = bFree_;
}
    
StrVal::~StrVal() {
    if ( text ) {
        if ( bFree ) delete [] text;
        text = 0;    
    }
    len = 0; bFree = false;
}

int64_t StrVal::getIntVal() const { 
    IntVal tmp;
    tmp.setStrVal( text, len, false );
    return tmp.value;
}

void StrVal::setIntVal( int64_t val ) {
    if ( bFree && text ) delete [] text;
    text = 0; len = 0;
    format( text, len, "%" PRId64, val );
    bFree = true;
}

double StrVal::getRealVal() const { 
    RealVal tmp;
    tmp.setStrVal( text, len, false );
    return tmp.value;
}

void StrVal::setRealVal( double val ) {
    if ( bFree && text ) delete [] text;
    text = 0; len = 0;
    format( text, len, "%g", val );
    bFree = true;
}

void StrVal::getStrVal( uint8_t*& rPtr, size_t& rLen, bool& rFree ) const {
    rPtr = text; rLen = len; rFree = false;
}

void StrVal::setStrVal( const uint8_t* ptr, size_t len_, bool bFree_ ) {
    if ( text && bFree ) delete [] text;
    if ( bFree_ ) {
        text = new uint8_t [ len_ ];
        if ( len_ ) memcpy( text, ptr, len_ );
        len  = len_;
    } else {
        text = (uint8_t*) ptr;
        len  = len_;
    }
    bFree = bFree_;
}

void StrVal::alu( uint16_t op, ValDesc* arg ) {
    uint8_t* text2 = 0; size_t len2 = 0; bool bFree2 = false;
    arg->getStrVal( text2, len2, bFree2 );
    if ( op == T_PLUS ) { // concat
        uint8_t* text3 = new uint8_t [ len + len2 ];
        if ( len  ) memcpy( text3      , text , len  );
        if ( len2 ) memcpy( text3 + len, text2, len2 );
        size_t len3 = len + len2;
        if ( bFree2 ) delete [] text2;
        if ( bFree  ) delete [] text;
        text  = text3;
        len   = len3;
        bFree = true;
        return;
    }
    if ( op != T_EQ && op != T_NE && op != T_LT && op != T_GT &&
        op != T_LE && op != T_GE ) {
        if ( bFree2 ) delete [] text2;
        return;
    }
    // comparison
    bool   result = false;
    int    cmpRes = 0;
    size_t nComp  = len < len2 ? len : len2;
    if ( nComp ) {
        cmpRes = memcmp( text, text2, nComp );
    }
    if ( bFree2 ) delete [] text2;
    if ( cmpRes == 0 ) {
        if      ( len < len2 ) cmpRes = -1;
        else if ( len > len2 ) cmpRes =  1;
    }
    switch ( op ) {
        case T_EQ:
            result = ( cmpRes == 0 );
            break;
        case T_NE:
            result = ( cmpRes != 0 );
            break;
        case T_LT:
            result = ( cmpRes < 0 );
            break;
        case T_GT:
            result = ( cmpRes > 0 );
            break;
        case T_LE:
            result = ( cmpRes <= 0 );
            break;
        case T_GE:
            result = ( cmpRes >= 0 );
            break;
        default:    
            break;
    }
    setIntVal( result ? -1 : 0 );
}

// --- AryHashEnt --------------------------------------------------------------------

AryHashEnt::AryHashEnt( size_t cellIndex_, const uint8_t* name_, size_t nameLen_ )
    :   HashEntry( name_, nameLen_ ), cellIndex(cellIndex_) {}

AryHashEnt::~AryHashEnt() { cellIndex = SIZE_MAX; }

// --- AryVal ------------------------------------------------------------------------

void AryVal::init() {
    if ( elemType == VT_ARY || elemType == VT_FUNC ) {
        throw Exception( "array type impossible" );
    }
    if ( arrayType != AT_STATIC && ndims != 1U ) {
        throw Exception( "array type impossible" );
    }
    totalSize = 1;
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
    size_t offset = 1U;
    for ( ssize_t i=ndims-1; i >= 0; --i ) {
        if ( i < (ssize_t)(ndims-1) ) {
            coordMult[i] = offset;
        }
        offset *= dims[i];
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
    if ( arrayType == AT_ASSOC || arrayType == AT_DYNAMIC ) {
        totalSize = 0;
    }
}

AryVal::AryVal( va_list ap ) : ValDesc(VT_ARY) {
    elemType  = (ValueType) va_arg( ap, int );
    arrayType = (ArrayType) va_arg( ap, int );
    ndims     = va_arg( ap, size_t );
    dims      = new size_t [ ndims ];
    coordMult = new size_t [ ndims ];
    for ( size_t i=0; i < ndims; ++i ) {
        dims[i]      = va_arg( ap, size_t );
        coordMult[i] = 1;
    }
    init();
}

AryVal::AryVal( ValueType elemType_, ArrayType arrayType_, size_t ndims_, ... ) 
    : ValDesc(VT_ARY), elemType(elemType_), arrayType(arrayType_), ndims(ndims_) {
    va_list ap;
    va_start( ap, ndims_ );
    dims = new size_t [ ndims ];
    for ( size_t i=0; i < ndims; ++i ) {
        dims[i]      = va_arg( ap, size_t );
        coordMult[i] = 1;
    }
    va_end( ap );
    init();
}

AryVal::AryVal( ValueType elemType_, ArrayType arrayType_, size_t ndims_, 
    const size_t* dims_ ) : ValDesc(VT_ARY), elemType(elemType_), arrayType(arrayType_),
    ndims(ndims_) {
    dims = new size_t [ ndims ];
    if ( ndims ) memcpy( dims, dims_, sizeof(size_t) * ndims );
    coordMult = new size_t [ ndims ];
    for ( size_t i=0; i < ndims; ++i ) coordMult[i] = 1;
    init();
}

AryVal::~AryVal() {
    if ( ht ) { delete ht; ht = 0; }
    if ( arrayType == AT_ASSOC || arrayType == AT_DYNAMIC ) {
        totalSize = dims[0];
    }
    while ( totalSize ) {
        --totalSize;
        if ( cells[totalSize] ) { 
            delete cells[totalSize]; cells[totalSize] = 0; 
        }
    }
    delete [] cells; cells = 0; 
    delete [] coordMult; coordMult = 0; 
    delete [] dims; dims = 0; ndims = 0;
    elemType = VT_UNDEF;
}

ValDesc* AryVal::subscriptStatic( ValDesc** args ) {
    size_t pos = 0;
    for ( size_t i=0; i < ndims; ++i ) {
        ValDesc* val   = args[i];
        size_t   mult  = i < ndims-1U ? coordMult[i] : 0;
        if ( val->type != VT_INT && val->type != VT_REAL ) {
            throw Exception( "type mismatch dimension #d", (int) i );
        }
        int64_t d     = val->getIntVal();
        if ( d < 0 ) {
            throw Exception( "negative array index #%d", (int) i );
        }
        size_t  index = (size_t) d;
        if ( index >= dims[i] ) {
            throw Exception( "index #%d out of range", (int) i );
        }
        pos += mult ? mult * index : index;
    }
    if ( pos >= totalSize ) throw Exception( "internal error: bad index" );
    return cells[pos];
}

ValDesc* AryVal::subscriptDynamic( ValDesc** args ) {
    ValDesc* val = args[0];
    if ( val->type != VT_INT && val->type != VT_REAL ) {
        throw Exception( "type mismatch dimension #0" );
    }
    int64_t d = val->getIntVal();
    if ( d < 0 ) {
        throw Exception( "negative array index" );
    }
    size_t index     = (size_t) d;
    size_t numFilled = totalSize;
    if ( index >= dims[0] ) {
        // resize cells array
        size_t newdim;
        if ( dims[0] >= SIZE_MAX / 2U ) {
            newdim = SIZE_MAX;
        } else {
            newdim = dims[0] * 2U; 
        }
        if ( index >= newdim && index < SIZE_MAX ) {
            newdim = index + 1U;
        } else if ( index >= newdim ) {
            newdim = SIZE_MAX;
        }
        size_t maxsize = SIZE_MAX / sizeof(ValDesc*);
        if ( newdim > maxsize ) throw Exception( "array too large" );
        ValDesc** newCells;
        try {
            newCells = new ValDesc* [ newdim ];
        } catch ( const std::exception& xcpt ) {
            throw Exception( "out of memory" );
        }
        if ( numFilled ) {
            memcpy( (void*) newCells, (void*) cells, sizeof(ValDesc*)
                * numFilled );
        }
        delete [] cells;
        cells   = newCells;
        dims[0] = newdim;
    }
    if ( index >= numFilled ) { // allocate cell values
        while ( numFilled <= index ) {
            cells[numFilled++] = ValDesc::create( elemType );
        }
        totalSize = numFilled;
    }
    return cells[index];
}

ValDesc* AryVal::subscriptAssoc( ValDesc** args ) {
    ValDesc* val = args[0]; U_IntReal64 ir; 
    uint8_t* key = 0; size_t keyLen = 0; bool bFree = false;
    switch ( val->type ) {
        case VT_INT:
            ir.ival = val->getIntVal();
            key     = (uint8_t*)(&ir.ival);
            keyLen  = sizeof(ir.ival);
            break;
        case VT_REAL:
            ir.rval = val->getRealVal();
            key     = (uint8_t*)(&ir.rval);
            keyLen  = sizeof(ir.rval);
            break;
        case VT_STR:
            val->getStrVal( key, keyLen, bFree );
            break;
        default:
            throw Exception( "type mismatch dimension #0" );
    }
    HashEntry* hashEnt = ht->find( key, keyLen );
    if ( hashEnt ) {    // hash entry found
        AryHashEnt* aryHashEnt = dynamic_cast<AryHashEnt*>( hashEnt );
        if ( aryHashEnt == 0 ) throw Exception( "bad associative array" );
        return cells[ aryHashEnt->cellIndex ];
    }
    // see if adding a new cell at the end of the array would resize it
    size_t index = totalSize;
    if ( index >= dims[0] ) {
        // resize cells array
        size_t newdim;
        if ( dims[0] >= SIZE_MAX / 2U ) {
            newdim = SIZE_MAX;
        } else {
            newdim = dims[0] * 2U; 
        }
        size_t maxsize = SIZE_MAX / sizeof(ValDesc*);
        if ( newdim > maxsize ) {
            if ( bFree ) delete [] key;
            throw Exception( "array too large" );
        }
        ValDesc** newCells;
        try {
            newCells = new ValDesc* [ newdim ];
        } catch ( const std::exception& xcpt ) {
            if ( bFree ) delete [] key;
            throw Exception( "out of memory" );
        }
        if ( index ) {
            memcpy( (void*) newCells, (void*) cells, sizeof(ValDesc*)
                * index );
        }
        delete [] cells;
        cells   = newCells;
        dims[0] = newdim;
    }
    // create a new cell at the end of the array
    cells[index] = ValDesc::create( elemType );
    totalSize = index + 1U;
    ValDesc* cell = cells[index];
    // add it to the hash table
    ht->enter( new AryHashEnt( index, key, keyLen ) );
    if ( bFree ) delete [] key;
    // return accessed cell
    return cell;
}

ValDesc* AryVal::subscript( ValDesc** args ) {
    switch ( arrayType ) {
        default:
            throw Exception( "internal error: bad array" );
        case AT_STATIC:     return subscriptStatic ( args );
        case AT_DYNAMIC:    return subscriptDynamic( args );
        case AT_ASSOC:      return subscriptAssoc  ( args );
    }
}

// --- FuncArg ------------------------------------------------------------------------

FuncArg::FuncArg() {
    pArgs = new ValDesc* [ 5 ];
    fArgs = new bool     [ 5 ];
    nArgs = 0;
    aArgs = 5;
    pRes  = new ValDesc* [ 3 ];
    fRes  = new bool     [ 3 ];
    nRes  = 0;
    aRes  = 3;
}

FuncArg::~FuncArg() {
    delete [] fRes;  fRes  = 0;
    delete [] pRes;  pRes  = 0;
    delete [] fArgs; fArgs = 0;
    delete [] pArgs; pArgs = 0;
    aRes = nRes = aArgs = nArgs = 0;
}

void FuncArg::addArg( ValDesc* arg, bool fFree ) {
    if ( nArgs >= aArgs ) {
        size_t newSz = aArgs * 2U;
        if ( nArgs >= newSz ) newSz = nArgs + 1U;
        ValDesc** pArgsNew = new ValDesc* [ newSz ];
        bool*     fArgsNew = new bool     [ newSz ];
        if ( nArgs ) {
            memcpy( (void*)(&pArgsNew[0]), (void*)(&pArgs[0]),
                sizeof(ValDesc*) * nArgs );
            memcpy( &fArgsNew[0], &fArgs[0], sizeof(bool) * nArgs );
        }
        delete [] fArgs; fArgs = fArgsNew;
        delete [] pArgs; pArgs = pArgsNew;
        aArgs = newSz;
    }
    pArgs[nArgs] = arg;
    fArgs[nArgs] = fFree;
    ++nArgs;
}

void FuncArg::addRes( ValDesc* res, bool fFree ) {
    if ( nRes >= aRes ) {
        size_t newSz = aRes * 2U;
        if ( nRes >= newSz ) newSz = nRes + 1U;
        ValDesc** pResNew = new ValDesc* [ newSz ];
        bool*     fResNew = new bool     [ newSz ];
        if ( nRes ) {
            memcpy( (void*)(&pResNew[0]), (void*)(&pRes[0]),
                sizeof(ValDesc*) * nRes );
            memcpy( &fResNew[0], &fRes[0], sizeof(bool) * nRes );
        }
        delete [] fRes; fRes = fResNew;
        delete [] pRes; pRes = pResNew;
        aRes = newSz;
    }
    pRes[nRes] = res;
    fRes[nRes] = fFree;
    ++nRes;
}

// --- FuncVal ------------------------------------------------------------------------

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

void FuncVal::call() {
    pFunc( pFuncArg );
}

// --- VarDesc -----------------------------------------------------------------------


VarDesc::VarDesc( const uint8_t* name, size_t nameLen,
    ValDesc* valueDesc_ ) : HashEntry( name, nameLen ),
    valueDesc(valueDesc_) {}

VarDesc::~VarDesc() { 
    if ( valueDesc ) { delete valueDesc; valueDesc = 0; }
}

// --- Variables --------------------------------------------------------------------

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

