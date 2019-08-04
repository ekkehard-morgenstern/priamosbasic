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
    // T_IDENT
    uint8_t        ident[MAXIDENT];
    int            idLen;
    // T_STRLIT
    uint8_t        strlit[MAXSTRLIT];
    int            slLen;
    // T_NUMLIT
    int64_t        intVal;
    double         realVal;
    bool           isInt;
    int            numBase;

    void readIdent( uint8_t b );
    
    static int digitVal( uint8_t b, int base );
    static bool isDigit( uint8_t b, int base );
    static int bitsPerDigit( int base );
    static char* digitToBitGroup( char* buf, char* bufEnd, uint8_t b, 
        int base );
    static char* digitsToBitGroup( char* buf, char* bufEnd, 
        const char* dig, int ndig, int base );
    static char* digitsToHex( char* buf, int nbits );

    uint16_t readNum( const char* buf, int len, int base, bool haveDot,
        bool haveExp );
    uint16_t readNum( uint8_t b, int base );
    uint16_t readNum( int base );

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

    // only valid for T_NUMLIT
    inline bool numIsInt() const { return isInt; }
    inline int64_t numIVal() const { return intVal; }
    inline double numRVal() const { return realVal; }
    inline int numBase_() const { return numBase; }


};


#endif
