#include "variables.h"
#include "exception.h"


static unsigned randomUint() {
    unsigned val;
    FILE* fp = fopen( "/dev/urandom", "rb" );
    if ( fp == 0 ) exit( EXIT_FAILURE );
    fread( &val, sizeof(val), 1U, fp );
    fclose( fp );
    return val;
}

#define CP_NTRIES_I     5
#define CP_NTRIES_J     3

class CoordPool : public NonCopyable {
    bool*   hadThis;
    size_t  nCoords;
    size_t  nAlloc;
public:
    CoordPool( size_t nCoords_ );
    ~CoordPool();
    int dealCoord();
};

CoordPool::CoordPool( size_t nCoords_ ) {
    hadThis = new bool [ nCoords_ ];
    nCoords = nCoords_;
    for ( size_t i=0; i < nCoords; ++i ) {
        hadThis[i] = false;
    }
    nAlloc = 0;
}

CoordPool::~CoordPool() {
    delete [] hadThis; hadThis = 0;
    nCoords = 0; nAlloc =0;
}

int CoordPool::dealCoord() {
    if ( nAlloc >= nCoords ) return -1;
    for ( int j=0; j < CP_NTRIES_J; ++j ) {
        for ( int i=0; i < CP_NTRIES_I; ++i ) {
            int coord = rand() % ((int)nCoords);
            if ( !hadThis[coord] ) {
                hadThis[coord] = true; ++nAlloc;
                return coord;
            }
        }
        srand( randomUint() );
    }
    int coord0 = rand() % ((int)nCoords);
    int coord  = coord0;
    for (;;) {
        coord = ( coord + 1 ) % ((int)nCoords);
        if ( coord == coord0 ) break;
        if ( !hadThis[coord] ) {
            hadThis[coord] = true; ++nAlloc;
            return coord;
        }
    }
    return -1;
}

struct IntTestInfo {
    int         coordinate;
    int64_t     storedValue;
};

static void testIntAry_static() {

    static const size_t dims[3] = { 17, 33, 11 };
    AryVal av( VT_INT, AT_STATIC, 3, dims );
    IntVal ix[3]; ValDesc* vd[3]; vd[0] = &ix[0]; vd[1] = &ix[1]; vd[2] = &ix[2];
    int numreg = 0; int maxreg = dims[0] * dims[1] * dims[2];
    IntTestInfo* check = new IntTestInfo [ maxreg ];
    CoordPool cp( maxreg );
    printf( "testIntAry_static(): initializing AryVal ...\n" );
    for (;;) {
        if ( numreg >= maxreg ) break;
        int coord = cp.dealCoord();
        if ( coord < 0 ) break;
        int coor1 = coord / ( dims[1] * dims[2] );
        int coor2 = ( coord / dims[2] ) % dims[1];
        int coor3 = coord % dims[2];
        ix[0].setIntVal( coor1 );
        ix[1].setIntVal( coor2 );
        ix[2].setIntVal( coor3 );
        ValDesc* cell = av.subscript( vd );
        int64_t value = cell->getIntVal();
        if ( value != 0 ) {
            printf( "testIntAry_static(): implementation broken, nonzero value "
                "found at coord %d (%d,%d,%d), value = %" PRId64 "\n", coord,
                coor1, coor2, coor3, value );
            break;
        }
        int cellValue;
        do {
            cellValue = rand();
        } while ( cellValue == 0 );       
        cell->setIntVal( cellValue );
        check[numreg].coordinate  = coord;
        check[numreg].storedValue = cellValue;
        numreg++;
    }
    printf( "testIntAry_static(): registered %d out of %d entries\n", numreg, maxreg );
    printf( "testIntAry_static(): checking AryVal ...\n" );
    int nCorrect = 0;
    int nFailed  = 0;
    for ( int i=0; i < numreg; ++i ) {
        int coord = check[i].coordinate;
        int coor1 = coord / ( dims[1] * dims[2] );
        int coor2 = ( coord / dims[2] ) % dims[1];
        int coor3 = coord % dims[2];
        ix[0].setIntVal( coor1 );
        ix[1].setIntVal( coor2 );
        ix[2].setIntVal( coor3 );
        ValDesc* cell = av.subscript( vd );
        int64_t cellValue = cell->getIntVal();
        if ( cellValue != check[i].storedValue ) {
            if ( nFailed < 10 ) {
                printf( "#%d @%d (%d,%d,%d) %" PRId64 " != %" PRId64 "\n", 
                    i, coord, coor1, coor2, coor3, cellValue, check[i].storedValue );
            } else if ( nFailed == 10 ) {
                printf( "...\n" );
            }
            ++nFailed;
        } else {
            ++nCorrect;
        }
    }
    printf( "testIntAry_static(): %d entries correct, %d failed\n", nCorrect, nFailed );
}

int main( int argc, char** argv ) {

    srand( randomUint() );

    try {
        testIntAry_static();

    } catch ( const Exception& xcpt ) {
        printf( "? %s\n", xcpt.what() );
    }

    return EXIT_SUCCESS;
}