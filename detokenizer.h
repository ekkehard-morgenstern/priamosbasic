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

#ifndef DETOKENIZER_H
#define DETOKENIZER_H   1

#ifndef TYPES_H
#include "types.h"
#endif

#ifndef BYTEBUFFER_H
#include "bytebuffer.h"
#endif 

#ifndef TOKENSCANNER_H
#include "tokenscanner.h"
#endif

// initial detokenize buffer size
#define DETOK_BUFSZ     1024U

class Detokenizer : public NonCopyable {

    TokenScanner    scan;
    ByteBuffer      buf;

public:
    Detokenizer( const uint8_t* pos );
    virtual ~Detokenizer();

    const char* detokenize();

};


#endif