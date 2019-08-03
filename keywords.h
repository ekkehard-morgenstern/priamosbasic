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

#ifndef KEYWORDS_H
#define KEYWORDS_H  1

#ifndef HASHTABLE_H
#include "hashtable.h"
#endif

#define KW_NOTFOUND UINT16_C(0XFFFF)

struct PredefKW {
    const char* text;   // counted string (1st byte = length)
    short       tok;
};

struct KW_Hashent : public HashEntry {

    uint16_t tok;

    KW_Hashent( const char* p, unsigned char len, uint16_t tok_ );

};

class Keywords : private NonCopyable {

    HashTable ht;

    static const PredefKW predef[];

    Keywords();
    virtual ~Keywords();

    void init();    // add predefined keywords

    // initialized at start of program
    static Keywords instance;

public:

    static inline Keywords& getInstance() { return instance; }

    void add( const uint8_t* name, size_t nameLen, uint16_t tok );

    uint16_t lookup( const uint8_t* name, size_t nameLen );

};


#endif
