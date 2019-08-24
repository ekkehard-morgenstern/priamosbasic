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

#ifndef PROGRAM_H
#define PROGRAM_H   1

#ifndef LINEINFO_H
#include "lineinfo.h"
#endif

#ifndef TOKENIZER_H
#include "tokenizer.h"
#endif

// initial program buffer size
#define MINPRGSIZE        16384U

class Program : public NonCopyable, protected BBMemMan {

    ByteBuffer      prg;
    LineInfoManager lineInfo;

    virtual void compact( ByteBuffer& buf );

public:
    Program();
    virtual ~Program();

    inline size_t getLineInfoCount() const { 
        return lineInfo.getCount(); 
    }

    inline const LineInfo& getLineInfoAt( size_t pos ) const {
        return lineInfo.getAt( pos );
    }

    inline void setReadPos( size_t pos ) {
        prg.setReadPos( pos );
    }

    inline const uint8_t* readBlock( size_t length ) {
        return prg.readBlock( length );
    }

    void enterLine( const Tokenizer& t );

};

#endif
