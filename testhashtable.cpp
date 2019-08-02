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

#define TESTNODES       1000000
#define MINNAME         5U
#define MAXNAME         32U

static FILE* fp_rand;

static void rand_bytes( uint8_t* buf, size_t bufsz ) {
    if ( fread( buf, bufsz, 1U, fp_rand ) != 1U ) {
        fprintf( stderr, "I/O error: %m\n" );
        exit( EXIT_FAILURE );
    }
}

static uint64_t rand_uint() {

    uint8_t tmp[8];
    rand_bytes( tmp, sizeof(tmp) );

    uint64_t val = 0;
    memcpy( &val, tmp, sizeof(tmp) );

    return val;
}

static uint64_t rand_uint_rng( uint64_t rng ) {
    return rand_uint() % rng;
}

static void rand_str( uint8_t* buf, size_t bufsz ) {
    static const char charset[] = "0123456789ABCDEFGHIJKLMNOP"
        "QRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    uint64_t blob = 0; size_t nchars = strlen(charset);
    for ( size_t i=0; i < bufsz; ++i ) {
        while ( blob == 0 ) blob = rand_uint();
        uint8_t b = (uint8_t) ( blob % nchars );
        buf[i] = (uint8_t) charset[ b ];
        blob >>= 8U;
    }
}

static size_t randName( uint8_t* buf, size_t bufsz ) {

    size_t maxlen = bufsz;
    size_t minlen = MINNAME;
    if ( maxlen <= minlen ) {
        fprintf( stderr, "bad MINNAME\n" );
        exit( EXIT_FAILURE );
    }

    size_t reqlen = minlen + (size_t) rand_uint_rng( maxlen - 
        minlen );
    rand_str( buf, reqlen );

    return reqlen;
}

int main( int argc, char** argv ) {

    fp_rand = fopen( "/dev/urandom", "rb" );
    if ( fp_rand == 0 ) {
        fprintf( stderr, "failed to open /dev/urandom: %m\n" );
        return EXIT_FAILURE;
    }

    HashTable ht;

    double ti0 = getTime();

    for ( int i=0; i < TESTNODES; ++i ) {

        uint8_t name[MAXNAME];
        size_t nameLen;

        do {
            nameLen = randName( name, sizeof(name) );
        } while ( ht.find( name, nameLen ) );

        HashEntry* ent = new HashEntry( name, nameLen );
        ht.enter( ent );

    }

    double ti1 = getTime();
    double dif = ti1 - ti0;

    printf( "%ld nodes added in %g seconds\n", (long) TESTNODES,
        dif );

    double cov = ht.coverage();
    printf( "coverage is %g%%\n", cov );

    ht.dumpCounts();

    fclose( fp_rand );

    return EXIT_SUCCESS;
}