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

#ifndef TOKENIZER_H
#define TOKENIZER_H     1

#ifndef TYPES_H
#include "types.h"
#endif

#ifndef TOKENS_H
#include "tokens.h"
#endif

#define MAXIDENT         32
#define MAXSTRLIT        255
#define NUMBUFSZ         256

class Tokenizer : public NonCopyable {

    const uint8_t* source;
    const uint8_t* pos;
    const uint8_t* sourceEnd;
    size_t         sourceLen;
    uint8_t        ident[MAXIDENT];
    int            idLen;
    uint8_t        strlit[MAXSTRLIT];
    int            slLen;

    void readIdent( uint8_t b );
    
    static bool isDigit( uint8_t b, int base );
    uint16_t readNum( uint8_t b, int base );

public:
    Tokenizer( const uint8_t* source_, size_t sourceLen_ );
    virtual ~Tokenizer();

    uint16_t nextTok();

    // only valid for T_IDENT:
    inline const uint8_t* getIdent() const { return ident; }
    inline size_t getIdentLen() const { return idLen; }

    // only valid for T_STRLIT:
    inline const uint8_t* getStrLit() const { return strlit; }
    inline size_t getStrLitLen() const { return slLen; }


};


#endif
