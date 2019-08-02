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

#include "hashtable.h"

HashEntry::HashEntry( const uint8_t* name_, size_t nameLen_ ) {
    nextHash = 0;
    name     = new uint8_t [ nameLen_ ];
    nameLen  = nameLen_;
    if ( nameLen ) memcpy( name, name_, nameLen );
}

HashEntry::~HashEntry() {
    if ( nextHash ) { delete nextHash; nextHash = 0; }
    delete [] name; name = 0; nameLen = 0;
}

HashTable::HashTable() {
    memset( table, 0, sizeof(HashEntry*) * HT_SIZE );
    memset( count, 0, sizeof(size_t    ) * HT_SIZE );
    total = 0;
}

HashTable::~HashTable() {
    clear();
}

size_t HashTable::computeHashVal( const uint8_t* name, size_t nameLen ) {
    uint32_t v1 = UINT32_C(0XFA720BA3);
    uint32_t v2 = UINT32_C(0XD920F8BE);
    uint32_t v3 = UINT32_C(0X7A915F24);
    uint32_t v  = v1 ^ v2;
    while ( nameLen-- ) {
        uint8_t b = *name++;
        v  += b;
        v1 -= v;
        v2 ^= v1;
        v3 += v2;
        v  -= v3;
    }
    v %= (uint32_t) HT_SIZE;
    return (size_t) v;
}

void HashTable::enter( HashEntry* hashEntry ) {
    size_t hv = computeHashVal( hashEntry->name, hashEntry->nameLen );
    hashEntry->nextHash = table[hv];
    table[hv] = hashEntry;
    count[hv] += 1U; total += 1U;
}

void HashTable::remove( HashEntry* hashEntry ) {
    size_t hv = computeHashVal( hashEntry->name, hashEntry->nameLen );
    HashEntry* prev = 0;
    HashEntry* curr = table[hv];
    while ( curr ) {
        if ( curr->nameLen == hashEntry->nameLen && 
            memcmp( curr->name, hashEntry->name, 
                hashEntry->nameLen ) == 0 ) {
            if ( prev ) {
                prev->nextHash = curr->nextHash;
            } else {
                table[hv] = curr->nextHash;
            }
            curr->nextHash = 0;
            count[hv] -= 1U; total -= 1U;
            return;
        }
        prev = curr; curr = curr->nextHash;
    }
}

HashEntry* HashTable::find( const uint8_t* name, size_t nameLen ) {
    size_t hv = computeHashVal( name, nameLen );
    HashEntry* hashEntry = table[hv];
    while ( hashEntry ) {
        if ( hashEntry->nameLen == nameLen && 
            memcmp( hashEntry->name, name, nameLen ) == 0 ) {
            return hashEntry;    
        }
        hashEntry = hashEntry->nextHash;
    }
    return 0;
}

void HashTable::clear() {
    for ( int i=0; i < HT_SIZE; i++ ) {
        if ( table[i] ) { delete table[i]; table[i] = 0; }
        count[i] = 0;
    }
    total = 0;
}

void HashTable::dumpCounts() const {
    size_t x = 0;
    for ( int i=0; i < HT_SIZE; i++ ) {
        printf( " %lu", (unsigned long) count[i] );
        if ( ++x >= 16U ) { printf( "\n" ); x = 0; }
        else if ( i == HT_SIZE-1 ) printf( "\n" );
    }
}

double HashTable::coverage() const {
    if ( total == 0 ) return 0;
    double sum = 0; size_t max_count = 0;
    for ( int i=0; i < HT_SIZE; ++i ) {
        if ( count[i] > max_count ) max_count = count[i];
    }
    for ( int i=0; i < HT_SIZE; ++i ) {
        if ( count[i] ) sum += (long) count[i];
    }
    double perc = ( sum * 100 ) / (long)( HT_SIZE * max_count );
    return perc;
}
