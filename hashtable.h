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

#ifndef HASHTABLE_H
#define HASHTABLE_H 1

#ifndef TYPES_H
#include "types.h"
#endif

struct HashEntry : public NonCopyable {

    HashEntry*  nextHash;    // next entry with same hash value
    uint8_t*    name;
    size_t      nameLen;

    HashEntry( const uint8_t* name_, size_t nameLen_ );
    virtual ~HashEntry();

};


#define HT_SIZE     1024

class HashTable : public NonCopyable {

    HashEntry* table[HT_SIZE];
    size_t     count[HT_SIZE];
    size_t     total;

    size_t computeHashVal( const uint8_t* name, size_t nameLen );

public:
    HashTable();
    virtual ~HashTable();

    void enter( HashEntry* hashEntry );

    void remove( HashEntry* hashEntry );
    
    HashEntry* find( const uint8_t* name, size_t nameLen );

    void clear();

    void dumpCounts() const;
    double coverage() const;

};


#endif

