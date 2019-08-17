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
#include "tokens.h"

KW_Hashent::KW_Hashent( const char* p, unsigned char len, 
    uint16_t tok_ ) : HashEntry( (const uint8_t*) p, len ),
    tok(tok_) {}

KW_Hashent2::KW_Hashent2( const uint16_t& tok, const char* text_ )
    : HashEntry( (const uint8_t*)(&tok), 2U ), text(text_) {}

const PredefKW Keywords::predef[] = {
    { "\3NOP", KW_NOP },
    { "\3END", KW_END },
    { "\5AGAIN", KW_AGAIN },
    { "\5LEAVE", KW_LEAVE },
    { "\5BREAK", KW_BREAK },
    { "\7ITERATE", KW_ITERATE },
    { "\4CONT", KW_CONT },
    { "\4STOP", KW_STOP },
    { "\7RESTORE", KW_RESTORE },
    { "\4READ", KW_READ },
    { "\4DATA", KW_DATA },
    { "\3FOR", KW_FOR },
    { "\6RETURN", KW_RETURN },
    { "\5GOSUB", KW_GOSUB },
    { "\4GOTO", KW_GOTO },
    { "\5RESET", KW_RESET },
    { "\5FORTH", KW_FORTH },
    { "\4SEND", KW_SEND },
    { "\4BIND", KW_BIND },
    { "\5BLOCK", KW_BLOCK },
    { "\10NONBLOCK", KW_NONBLOCK },
    { "\6LISTEN", KW_LISTEN },
    { "\7CONNECT", KW_CONNECT },
    { "\4OPEN", KW_OPEN },
    { "\5CLOSE", KW_CLOSE },
    { "\5INPUT", KW_INPUT },
    { "\6REWIND", KW_REWIND },
    { "\4SEEK", KW_SEEK },
    { "\3LET", KW_LET },
    { "\2IF", KW_IF },
    { "\6UNLESS", KW_UNLESS },
    { "\3NEW", KW_NEW },
    { "\3OLD", KW_OLD },
    { "\4SAVE", KW_SAVE },
    { "\4LOAD", KW_LOAD },
    { "\3DIR", KW_DIR },
    { "\5CHDIR", KW_CHDIR },
    { "\7PUSHDIR", KW_PUSHDIR },
    { "\6POPDIR", KW_POPDIR },
    { "\3RUN", KW_RUN },
    { "\4LIST", KW_LIST },
    { "\6DELETE", KW_DELETE },
    { "\5RENUM", KW_RENUM },
    { "\4HELP", KW_HELP },
    { "\5QHELP", KW_QHELP },
    { "\3WHY", KW_WHY },
    { "\4CALL", KW_CALL },
    { "\6RESULT", KW_RESULT },
    { "\6OPTION", KW_OPTION },
    { "\3DEF", KW_DEF },
    { "\3DIM", KW_DIM },
    { "\4NEXT", KW_NEXT },
    { "\3CLR", KW_CLR },
    { "\11RANDOMIZE", KW_RANDOMIZE },
    { "\3DEG", KW_DEG },
    { "\3RAD", KW_RAD },
    { "\5WHILE", KW_WHILE },
    { "\4WEND", KW_WEND },
    { "\6REPEAT", KW_REPEAT },
    { "\5UNTIL", KW_UNTIL },
    { "\7FOREVER", KW_FOREVER },
    { "\7FOREACH", KW_FOREACH },
    { "\10WARRANTY", KW_WARRANTY },
    { "\12CONDITIONS", KW_CONDITIONS },
    { "\4ASC(", KW_ASCFN },
    { "\4VAL(", KW_VALFN },
    { "\5STR$(", KW_STRSFN },
    { "\2TI", KW_TI },
    { "\3TI$", KW_TIS },
    { "\6LEFT$(", KW_LEFTSFN },
    { "\7RIGHT$(", KW_RIGHTSFN },
    { "\5MID$(", KW_MIDSFN },
    { "\4POS(", KW_POSFN },
    { "\5HPOS(", KW_HPOSFN },
    { "\5VPOS(", KW_VPOSFN },
    { "\5BIN$(", KW_BINSFN },
    { "\5OCT$(", KW_OCTSFN },
    { "\5DEC$(", KW_DECSFN },
    { "\5HEX$(", KW_HEXSFN },
    { "\4CVI(", KW_CVIFN },
    { "\4CVF(", KW_CVFFN },
    { "\5MKI$(", KW_MKISFN },
    { "\5MKF$(", KW_MKFSFN },
    { "\6WHERE$", KW_WHERES },
    { "\6IPV4$(", KW_IPV4SFN },
    { "\6IPV6$(", KW_IPV6SFN },
    { "\11HOSTNAME$", KW_HOSTNAMES },
    { "\7DOMAIN$", KW_DOMAINS },
    { "\6RECV$(", KW_RECVSFN },
    { "\10SOCKETV4", KW_SOCKETV4 },
    { "\10SOCKETV6", KW_SOCKETV6 },
    { "\7ACCEPT(", KW_ACCEPTFN },
    { "\4CWD$", KW_CWDS },
    { "\4RND(", KW_RNDFN },
    { "\4SIN(", KW_SINFN },
    { "\3LN(", KW_LNFN },
    { "\4LOG(", KW_LOGFN },
    { "\5LOG2(", KW_LOG2FN },
    { "\4COS(", KW_COSFN },
    { "\4TAN(", KW_TANFN },
    { "\4COT(", KW_COTFN },
    { "\4ATN(", KW_ATNFN },
    { "\5HEAD(", KW_HEADFN },
    { "\5TAIL(", KW_TAILFN },
    { "\4TRUE", KW_TRUE },
    { "\5FALSE", KW_FALSE },
    { "\3NIL", KW_NIL },
    { "\5NEXT(", KW_NEXTFN },
    { "\5PREV(", KW_PREVFN },
    { "\6CELLS(", KW_CELLSFN },
    { "\3INC", KW_INC },
    { "\3DEC", KW_DEC },
    { "\3SUB", KW_SUB },
    { "\4FUNC", KW_FUNC },
    { "\4BASE", KW_BASE },
    { "\11BYTEORDER", KW_BYTEORDER },
    { "\3INT", KW_INT },
    { "\5FLOAT", KW_FLOAT },
    { "\5FIXED", KW_FIXED },
    { "\2FN", KW_FN },
    { "\7DYNAMIC", KW_DYNAMIC },
    { "\3PTR", KW_PTR },
    { "\4LINE", KW_LINE },
    { "\2IN", KW_IN },
    { "\5LABEL", KW_LABEL },
    { "\4THEN", KW_THEN },
    { "\2TO", KW_TO },
    { "\6DOWNTO", KW_DOWNTO },
    { "\3AND", KW_AND },
    { "\2OR", KW_OR },
    { "\3XOR", KW_XOR },
    { "\3NOT", KW_NOT },
    { "\4NAND", KW_NAND },
    { "\3NOR", KW_NOR },
    { "\4XNOR", KW_XNOR },
    { "\3EQV", KW_EQV },
    { "\4NEQV", KW_NEQV },
    { "\3SHL", KW_SHL },
    { "\3SHR", KW_SHR },
// ------------------------------------------
    { "\5PRINT", T_PRINT },
    { "\3REM", T_REM },
    { 0, 0 }
};

Keywords::Keywords() { init(); }
Keywords::~Keywords() {}

void Keywords::init() {
    for ( int i=0; predef[i].text; ++i ) {
        const char*     p   = predef[i].text;
        unsigned char   len = *p++;
        uint16_t        tok = (uint16_t) predef[i].tok;
        ht.enter( new KW_Hashent( p, len, tok ) );
        ht2.enter( new KW_Hashent2( tok, p ) );
    }
}

Keywords Keywords::instance;

void Keywords::add( const uint8_t* name, size_t nameLen, 
    uint16_t tok ) {
    ht.enter( new KW_Hashent( (const char*) name, 
        (unsigned char) nameLen, tok ) );
}

uint16_t Keywords::lookup( const uint8_t* name, size_t nameLen ) const {

    HashEntry* ent = ht.find( name, nameLen );
    if ( ent == 0 ) return KW_NOTFOUND;

    KW_Hashent* kw = dynamic_cast<KW_Hashent*>( ent );
    if ( kw == 0 ) return KW_NOTFOUND;

    return kw->tok;
}

const char* Keywords::lookup( uint16_t tok ) const {

    uint16_t tmp( tok );
    HashEntry* ent = ht2.find( (const uint8_t*)(&tmp), 2U );
    if ( ent == 0 ) return 0;

    KW_Hashent2* kw = dynamic_cast<KW_Hashent2*>( ent );
    if ( kw == 0 ) return 0;

    return kw->text;
}