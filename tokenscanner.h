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

#ifndef TOKENSCANNER_H
#define TOKENSCANNER_H  1

#ifndef TYPES_H
#include "types.h"
#endif

#ifndef TOKENS_H
#include "tokens.h"
#endif

class TokenScanner : public NonCopyable {

    const uint8_t* pos;

public:
    TokenScanner( const uint8_t* pos_ );
    virtual ~TokenScanner();

    uint16_t tokType() const;
    bool skipTok();

    // T_IDENT, T_STRLIT, T_LABEL
    bool getText( const uint8_t*& rText, uint8_t& rLen ) const;

    // T_LINENO
    bool getLineNo( uint32_t& rLineNo ) const;

    // T_NUMLIT, T_SBI
    bool getNumber( double& rVal ) const;
    


};



#endif
