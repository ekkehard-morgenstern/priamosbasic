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

#include "keywords.h"

KW_Hashent::KW_Hashent( const char* p, unsigned char len, 
    uint16_t tok_ ) : HashEntry( (const uint8_t*) p, len ),
    tok(tok_) {}

const PredefKW Keywords::predef[] = {
    { "\3NOP", 0X0300 },
    { "\3END", 0X0301 },
    { "\5AGAIN", 0X0302 },
    { "\5LEAVE", 0X0303 },
    { "\5BREAK", 0X0304 },
    { "\7ITERATE", 0X0305 },
    { "\4CONT", 0X0306 },
    { "\4STOP", 0X0307 },
    { "\7RESTORE", 0X0308 },
    { "\4READ", 0X0309 },
    { "\4DATA", 0X030A },
    { "\3FOR", 0X030B },
    { "\6RETURN", 0X030C },
    { "\5GOSUB", 0X030D },
    { "\4GOTO", 0X030E },
    { "\5RESET", 0X030F },
    { "\5FORTH", 0X0310 },
    { "\4SEND", 0X0311 },
    { "\4BIND", 0X0312 },
    { "\5BLOCK", 0X0313 },
    { "\10NONBLOCK", 0X0314 },
    { "\6LISTEN", 0X0315 },
    { "\7CONNECT", 0X0316 },
    { "\4OPEN", 0X0317 },
    { "\5CLOSE", 0X0318 },
    { "\5INPUT", 0X0319 },
    { "\6REWIND", 0X031A },
    { "\4SEEK", 0X031B },
    { "\3LET", 0X031C },
    { "\2IF", 0X031D },
    { "\6UNLESS", 0X031E },
    { "\3NEW", 0X031F },
    { "\3OLD", 0X0320 },
    { "\4SAVE", 0X0321 },
    { "\4LOAD", 0X0322 },
    { "\3DIR", 0X0323 },
    { "\5CHDIR", 0X0324 },
    { "\7PUSHDIR", 0X0326 },
    { "\6POPDIR", 0X0327 },
    { "\3RUN", 0X0328 },
    { "\4LIST", 0X0329 },
    { "\6DELETE", 0X032A },
    { "\5RENUM", 0X032B },
    { "\4HELP", 0X032C },
    { "\5QHELP", 0X032E },
    { "\3WHY", 0X032F },
    { "\4CALL", 0X0330 },
    { "\6RESULT", 0X0331 },
    { "\6OPTION", 0X0332 },
    { "\3DEF", 0X0333 },
    { "\3DIM", 0X0334 },
    { "\4NEXT", 0X0335 },
    { "\3CLR", 0X0336 },
    { "\11RANDOMIZE", 0X0337 },
    { "\3DEG", 0X0338 },
    { "\3RAD", 0X0339 },
    { "\5WHILE", 0X033A },
    { "\4WEND", 0X033B },
    { "\6REPEAT", 0X033C },
    { "\5UNTIL", 0X033D },
    { "\7FOREVER", 0X033E },
    { "\7FOREACH", 0X033F },
    { "\10WARRANTY", 0X0340 },
    { "\12CONDITIONS", 0X0341 },
    { "\3ASC", 0X0600 },
    { "\3VAL", 0X0601 },
    { "\3STR", 0X0602 },
    { "\2TI", 0X0603 },
    { "\2TI", 0X0604 },
    { "\4LEFT", 0X0605 },
    { "\5RIGHT", 0X0606 },
    { "\3MID", 0X0607 },
    { "\3POS", 0X0608 },
    { "\4HPOS", 0X0609 },
    { "\4VPOS", 0X060A },
    { "\3BIN", 0X060B },
    { "\3OCT", 0X060C },
    { "\3DEC", 0X060D },
    { "\3HEX", 0X060E },
    { "\3CVI", 0X060F },
    { "\3CVF", 0X0610 },
    { "\3MKI", 0X0611 },
    { "\3MKF", 0X0612 },
    { "\5WHERE", 0X0613 },
    { "\4IPV4", 0X0614 },
    { "\4IPV6", 0X0615 },
    { "\10HOSTNAME", 0X0616 },
    { "\6DOMAIN", 0X0617 },
    { "\4RECV", 0X0618 },
    { "\10SOCKETV4", 0X0619 },
    { "\10SOCKETV6", 0X061A },
    { "\6ACCEPT", 0X061B },
    { "\3CWD", 0X061C },
    { "\3RND", 0X061D },
    { "\3SIN", 0X061E },
    { "\2LN", 0X061F },
    { "\3LOG", 0X0620 },
    { "\4LOG2", 0X0621 },
    { "\3COS", 0X0622 },
    { "\3TAN", 0X0623 },
    { "\3COT", 0X0624 },
    { "\3ATN", 0X0625 },
    { "\4HEAD", 0X0626 },
    { "\4TAIL", 0X0627 },
    { "\4TRUE", 0X0628 },
    { "\5FALSE", 0X0629 },
    { "\3NIL", 0X062A },
    { "\4NEXT", 0X062B },
    { "\4PREV", 0X062C },
    { "\5CELLS", 0X062D },
    { "\3SUB", 0X0B00 },
    { "\4FUNC", 0X0B01 },
    { "\4BASE", 0X0B02 },
    { "\11BYTEORDER", 0X0B03 },
    { "\3INT", 0X0B04 },
    { "\5FLOAT", 0X0B05 },
    { "\5FIXED", 0X0B06 },
    { "\2FN", 0X0B07 },
    { "\7DYNAMIC", 0X0B08 },
    { "\3PTR", 0X0B09 },
    { "\4LINE", 0X0B0A },
    { "\2IN", 0X0B0B },
    { "\5LABEL", 0X0B0C },
    { "\3AND", 0X0F03 },
    { "\2OR", 0X0F04 },
    { "\3XOR", 0X0F05 },
    { "\3NOT", 0X0F06 },
    { "\4NAND", 0X0F07 },
    { "\3NOR", 0X0F08 },
    { "\4XNOR", 0X0F09 },
    { "\3EQV", 0X0F10 },
    { "\4NEQV", 0X0F11 },
    { "\3SHL", 0X0F12 },
    { "\3SHR", 0X0F13 },
    { 0, 0 }
};

Keywords::Keywords() {}
Keywords::~Keywords() {}

void Keywords::init() {
    for ( int i=0; predef[i].text; ++i ) {
        const char*     p   = predef[i].text;
        unsigned char   len = *p++;
        uint16_t        tok = (uint16_t) predef[i].tok;
        ht.enter( new KW_Hashent( p, len, tok ) );
    }
}

void Keywords::add( const uint8_t* name, size_t nameLen, 
    uint16_t tok ) {
    ht.enter( new KW_Hashent( (const char*) name, 
        (unsigned char) nameLen, tok ) );
}

uint16_t Keywords::lookup( const uint8_t* name, size_t nameLen ) {

    HashEntry* ent = ht.find( name, nameLen );
    if ( ent == 0 ) return KW_NOTFOUND;

    KW_Hashent* kw = dynamic_cast<KW_Hashent*>( ent );
    if ( kw == 0 ) return KW_NOTFOUND;

    return kw->tok;
}
