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


ValDesc::ValDesc( ValueType type_ ) : type(type_) {}
ValDesc::~ValDesc() {}

ValDesc* ValDesc::create( ValueType type_, ... ) {
    switch ( type_ ) {
        case VT_INT: return new IntVal();
        case VT_REAL: return new RealVal();
        case VT_STR: return new StrVal();
        case VT_ARY: {
            va_list ap;
            va_start( ap, type_ );
            ValueType elemType = va_arg( ap, ValueType );
            size_t    dim      = va_arg( ap, size_t    );
            va_end( ap );
            return new AryVal( elemType, dim );
        }
    }
    return 0;
}

IntVal::IntVal() : ValDesc(VT_INT), value(0) {}
IntVal::~IntVal() { value = 0; }

RealVal::RealVal() : ValDesc(VT_REAL), value(0) {}
RealVal::~RealVal() { value = 0; }

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

AryVal::AryVal( ValueType elemType_, size_t dim_ ) 
    : ValDesc(VT_ARY), elemType(elemType_), dim(dim_) {
    cells = new ValDesc* [ dim ];
    for ( size_t i=0; i < dim; i++ ) {
        ValDesc* cell;
        if ( elemType_ == VT_ARY ) {
            // nested Array: in this case, cannot specify cells
            // TODO: need to specify extra dimension info
            cell = 0;
        } else {
            cell = ValDesc::create( elemType );
        }
        cells[i] = cell;
    }
}

AryVal::~AryVal() {
    for ( size_t i=0; i < dim; i++ ) {
        if ( cells[i] ) { delete cells[i]; cells[i] = 0; }
    }
    delete [] cells; cells = 0; elemType = VT_UNDEF; dim = 0;
}
