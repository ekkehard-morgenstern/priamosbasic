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

#include "interpreter.h"
#include "keywords.h"

// --- CmdHashEnt --------------------------------------------------------------------

CmdHashEnt::CmdHashEnt( uint16_t& tok_, CmdMethodPtr mth_ )
    :   HashEntry( (const uint8_t*)(&tok_), 2U ), mth(mth_) {}
CmdHashEnt::~CmdHashEnt() {}

// --- IdentInfo ---------------------------------------------------------------------

IdentInfo::IdentInfo() : name(0), nLen(0), desc(0), flags(0), param(0) {}

IdentInfo::~IdentInfo() {
    name = 0; nLen = 0; desc = 0; flags = 0;
    if ( param ) { delete param; param = 0; }
}

// --- ExprInfo ----------------------------------------------------------------------

ExprInfo::ExprInfo( ValDesc* value_, bool bFree_ ) : next(0), param(0), value(value_),
    bFree(bFree_) {}

ExprInfo::~ExprInfo() {
    if ( next ) { delete next ; next = 0; }
    if ( value ) { 
        if ( bFree ) delete value; 
        value = 0; 
    }
    if ( param ) { delete param; param = 0; }
}

void ExprInfo::promoteIntToReal() {
    if ( value->type == VT_INT ) {
        ValDesc* newVal = new RealVal();
        newVal->setIntVal( value->getIntVal() );
        delete value; 
        value = newVal;
    }
}

void ExprInfo::demoteRealToInt() {
    if ( value->type == VT_REAL ) {
        ValDesc* newVal = new IntVal();
        newVal->setIntVal( value->getIntVal() );
        delete value; 
        value = newVal;
    }
}

void ExprInfo::changeStrToInt() {
    if ( value->type == VT_STR ) {
        ValDesc* newVal = new IntVal();
        newVal->setIntVal( value->getIntVal() );
        delete value; 
        value = newVal;
    }
}

// --- ExprList ----------------------------------------------------------------------

ExprList::ExprList() : first(0), last(0) {}

ExprList::~ExprList() {
    if ( first ) { delete first; first = 0; }
    last = 0;
}
    
void ExprList::add( ExprInfo* expr ) {
    if ( !last ) {
        first = last = expr;
    } else {
        last->next = expr;
        last       = expr;
    }
}

void ExprList::moveFrom( ExprList* exprList ) {
    if ( !last ) {
        first = exprList->first; exprList->first = 0;
        last  = exprList->last;  exprList->last  = 0;
    } else {
        last->next = exprList->first; exprList->first = 0;
        last       = exprList->last;  exprList->last  = 0;
    }
}

void ExprList::addFirst( ExprInfo* expr ) {
    if ( !first ) {
        first = last = expr;
    } else {
        expr->next = first;
        first      = expr;
    }
}

size_t ExprList::count() const {
    ExprInfo* ei = first; size_t cnt = 0;
    while ( ei ) { ei = ei->next; ++cnt; }
    return cnt;
}

// --- Interpreter -------------------------------------------------------------------

const CmdDecl Interpreter::cmdDeclTable[] = {
    { KW_LIST, &Interpreter::list },
    { 0, 0 }
};

const FnDecl Interpreter::funcDeclTable[] = {
    { 0, FT_UNDEF, 0, 0, 0, false, 0 }
};

void Interpreter::skipTok() {
    if ( !scan.skipTok() ) {
        throw Exception( "interpret error: bad token" );
    }
}

bool Interpreter::getIdentInfo( IdentInfo& ii ) {
    const uint8_t* pos = scan.getPos();
    uint16_t tok = scan.tokType(); bool isFN = false;
    if ( tok == KW_FN ) {   // small function reference
        skipTok();
        tok = scan.tokType();
        if ( tok != T_IDENT ) {
            throw Exception( "syntax error: identifier expected" );
        }
        isFN = true;
        goto IDENT;
    } else if ( tok == T_IDENT ) { // variable or function (declared, or undeclared)
IDENT:  ii.name = 0; ii.nLen = 0;
        if ( !scan.getText( ii.name, ii.nLen ) || ii.name == 0 || 
            ii.nLen == 0 ) {
            throw Exception( "interpret error: token error" );
        }
        ii.desc  = vars.findVar( ii.name, ii.nLen );
    } else if ( ISFUNCKW( tok ) ) { // functional keyword (built-in function)
        const char* text = Keywords::getInstance().lookup( tok );
        if ( text == 0 ) {
            throw Exception( "interpret error: keyword not registered" );
        }
        ii.name = (const uint8_t*) text;
        ii.nLen = ii.name[-1];
        ii.desc = vars.findVar( ii.name, ii.nLen );
        if ( ii.desc == 0 ) {
            throw Exception( "interpret error: function keyword not implemented" );
        }
    } else {
        return false;
    }
    ii.flags = 0;
    if ( ii.desc == 0 ) {
        // undeclared variable or function: guess intended type
        uint8_t x = ii.nLen - UINT8_C(1);
        if ( ii.name[x] == UINT8_C(0X28) ) {  // (
            if ( isFN ) {
                ii.flags |= IIF_FN;
            } else {
                ii.flags |= IIF_ARY | IIF_FN;
            }
            --x;
        }
        if ( ii.name[x] == UINT8_C(0X24) ) {        // $
            ii.flags |= IIF_STR;
        } else if ( ii.name[x] == UINT8_C(0X25) ) { // %
            ii.flags |= IIF_INT;
        }
    } else {
        // known variable or function: read type
        ValueType vt = ii.desc->type;
        if ( isFN ) {
            if ( vt == VT_FUNC ) {
                FuncVal* fv = dynamic_cast<FuncVal*>( ii.desc );
                if ( fv ) {
                    if ( fv->type == FT_BAS_FN ) {
                        ii.flags |= IIF_FN;
                    } else {
                        throw Exception( "syntax error: not FN-declared" );
                    }
                } else {
                    throw Exception( "interpret error: bad function" );
                }
            } else {
                throw Exception( "syntax error: FN with non-function" );
            }
        } else {
            switch ( vt ) {
                case VT_INT:    ii.flags |= IIF_INT; break;
                // VT_REAL doesn't require special flag
                case VT_STR:    ii.flags |= IIF_STR; break;
                case VT_ARY:    ii.flags |= IIF_ARY; break;
                case VT_FUNC:   ii.flags |= IIF_FN;  break;
                default:        break;
            }
        }
    }
    scan.setPos( pos ); // rewind
    return true;
}

void Interpreter::getIdentAuto( IdentInfo& ii, ValueType vt ) {

    if ( ii.desc == 0 ) {
        if ( ii.flags & IIF_ARY ) {
            throw Exception( "array not dimensioned" );
        }
        if ( ii.flags & IIF_FN  ) {
            throw Exception( "function not declared" );
        }
        // simple variable: auto-declare it
        ii.desc = ValDesc::create( vt );
        if ( !vars.addVar( ii.name, ii.nLen, ii.desc ) ) {
            throw Exception( "interpret error: failed to add variable" );
        }
    }

    // skip token(s)
    uint16_t tok = scan.tokType();
    if ( tok == KW_FN ) {   // small function reference
        skipTok();
    }

    skipTok();
}

bool Interpreter::getNumIdent( IdentInfo& ii ) {
    // num-ident := [ 'FN' ] num-identifier | num-variable | num-func-kw .

    if ( !getIdentInfo( ii ) ) return false;
    if ( ii.flags & IIF_STR  ) return false;

    getIdentAuto( ii, ii.flags & IIF_INT ? VT_INT : VT_REAL );
    return true;
}

bool Interpreter::getStrIdent( IdentInfo& ii ) {
    // str-ident := [ 'FN' ] str-identifier | str-variable | str-func-kw .

    if ( !getIdentInfo( ii )         ) return false;
    if ( ( ii.flags & IIF_STR ) == 0 ) return false;

    getIdentAuto( ii, VT_STR );
    return true;
}

void Interpreter::getIdentArgs( IdentInfo& ii ) {

    if ( ii.flags & ( IIF_ARY | IIF_FN ) ) {
        ExprList* el = getExprList();
        uint16_t tok = scan.tokType();
        if ( tok != T_RPAREN ) {
            if ( el ) delete el;
            throw Exception( "syntax error: closing parenthesis ')' expected" );
        }
        if ( !scan.skipTok() ) {
            if ( el ) delete el;
            throw Exception( "interpret error: bad token" );
        }
        ii.param = el;
    }
}
    
bool Interpreter::getNumIdentExpr( IdentInfo& ii ) {
    // num-ident-expr := num-ident [ '(' expr-list ')' ] .

    if ( !getNumIdent( ii ) ) return false;

    getIdentArgs( ii );
    return true;
}

bool Interpreter::getStrIdentExpr( IdentInfo& ii ) {
    // str-ident-expr := str-ident [ '(' expr-list ')' ] .

    if ( !getStrIdent( ii ) ) return false;

    getIdentArgs( ii );
    return true;
}

void Interpreter::fillFuncArgs( FnArg& args, ExprList* el ) {
    // move values from expression list to argument object
    if ( el ) {
        ExprInfo* ei = el->first;
        while ( ei ) {
            args.addArg( ei->detachValue(), true );
            ei = ei->next;
        }
    }
}

void Interpreter::fillFuncRes( ExprList* el, FnArg& args ) {
    // move values from argument object to expression list
    for (;;) {
        ValDesc* val = args.detachResultBackwards();
        if ( val == 0 ) break;
        el->addFirst( new ExprInfo( val, true ) );
    }
}

void Interpreter::verifyFuncArgs( FuncVal* fn, const FnArg& args ) {
    size_t nArgs = args.nArgs;
    if ( nArgs < fn->nForm ) {
        throw Exception( "syntax error: not enough arguments for function call" );
    }
    if ( nArgs > ((size_t)fn->nForm) + ((size_t)fn->nOpt) && !fn->bVarArgs ) {
        throw Exception( "syntax error: too many arguments for function call" );
    }
}

bool Interpreter::verifyFuncRes( FuncVal* fn, const FnArg& args ) {
    size_t nRes = args.nRes;
    if ( nRes != fn->nRes ) {
        throw Exception( "bad function: invalid number of results" );
    }    
    return true;
}

void Interpreter::fillArrayArgs( ValDesc* desc, ExprList* param, AryVal*& rAv, 
    ValDesc**& rArgs ) {
    AryVal* av = dynamic_cast<AryVal*>( desc );
    if ( av == 0 ) throw Exception( "interpret error: bad array" );
    if ( param == 0 ) throw Exception( "bad subscript" );
    size_t cnt = param->count();
    ArrayType at = av->arrayType;
    if ( at == AT_DYNAMIC || at == AT_ASSOC ) {
        if ( cnt != 1U ) throw Exception( "too many dimensions" );
    } else if ( at == AT_STATIC ) {
        if ( cnt < av->ndims ) throw Exception( "too few dimensions" );
        if ( cnt > av->ndims ) throw Exception( "too many dimensions" );
    }
    ValDesc** args = new ValDesc* [ cnt ];
    ExprInfo* ei   = param->first;
    size_t    pos  = 0;
    while ( ei ) { args[pos++] = ei->value; ei = ei->next; }
    rAv = av; rArgs = args;
}

ExprList* Interpreter::evalIdentExpr( IdentInfo& ii, ValueType vt ) {

    if ( ii.desc == 0 ) {
        throw Exception( "interpret error: unexpected value" );
    }

    ExprList* res = new ExprList();

    try {
        switch ( ii.desc->type ) {
            case VT_INT:
            case VT_REAL:
            case VT_STR:
                res->add( new ExprInfo( ii.desc, false ) );
                break;
            case VT_FUNC:
            case VT_ARY:
                if ( ii.desc->type == VT_FUNC ) {
                    FuncVal* fn = dynamic_cast<FuncVal*>( ii.desc );
                    if ( fn == 0 ) throw Exception( "interpret error: bad function call" );
                    FnArg args;
                    fillFuncArgs( args, ii.param );
                    verifyFuncArgs( fn, args );
                    fn->call();
                    verifyFuncRes( fn, args );
                    fillFuncRes( res, args );
                } else {    // VT_ARY
                    AryVal* av = 0; ValDesc** args = 0;
                    fillArrayArgs( ii.desc, ii.param, av, args );
                    try {
                        res->add( new ExprInfo( av->subscript( args ), false ) );
                    } catch ( const Exception& xcpt ) {
                        delete [] args;
                        throw;
                    }
                    delete [] args;
                }
                break;
            default:
                throw Exception( "interpret error: not implemented" );
        }
    } catch ( const Exception& xcpt ) {
        delete res;
        throw;  // re-throw exception
    }

    return res;
}

ExprList* Interpreter::getNumBaseExpr() {
    // num-base-expr := num-ident-expr | num-const | '(' num-expr ')' .

    IdentInfo ii;
    if ( getNumIdentExpr( ii ) ) {
        // identifier expression, array and function
        ValueType vt = ii.flags & IIF_INT ? VT_INT : VT_REAL;
        return evalIdentExpr( ii, vt );
    }

    uint16_t tok = scan.tokType();
    if ( tok == T_NUMLIT || tok == T_SBI ) {
        if ( scan.isInt() ) {
            IntVal* val = new IntVal();
            if ( !scan.getInt( val->value ) || !scan.skipTok() ) {
                delete val;
                throw Exception( "interpret error: bad int token" );
            }
            ExprList* el = new ExprList();
            el->add( new ExprInfo( val, true ) );
            return el;           
        } 
        RealVal* val = new RealVal();
        if ( !scan.getReal( val->value ) || !scan.skipTok() ) {
            delete val;
            throw Exception( "interpret error: bad real token" );
        }
        ExprList* el = new ExprList();
        el->add( new ExprInfo( val, true ) );
        return el;
    }

    if ( tok == T_LPAREN ) {
        skipTok();
        ExprList* el = getNumExpr();
        if ( el == 0 ) throw Exception( "syntax error: expression expected" );
        tok = scan.tokType();
        if ( tok != T_RPAREN ) {
            delete el;
            throw Exception( "syntax error: closing parenthesis ')' expected" );
        }
        if ( !scan.skipTok() ) {
            delete el;
            throw Exception( "interpret error: bad token" );
        }
        return el;
    }

    return 0;
}

ExprList* Interpreter::getStrBaseExpr() {
    // str-base-expr := str-ident-expr | str-const .

    IdentInfo ii;
    if ( getStrIdentExpr( ii ) ) {
        // identifier expression, array and function
        return evalIdentExpr( ii, VT_STR );
    }

    uint16_t tok = scan.tokType();
    if ( tok == T_STRLIT ) {
        const uint8_t* text = 0; uint8_t len = 0;
        if ( !scan.getText( text, len ) || !scan.skipTok() ) {
            throw Exception( "interpret error: bad string token" );
        }
        ExprList* el = new ExprList();
        el->add( new ExprInfo( new StrVal( text, len, false ), true ) );
        return el;
    }

    return 0;
}

void Interpreter::verifySingleNumber( ExprList* el ) {
    if ( el->first == 0 || el->first != el->last ) {
        throw Exception( "syntax error: single value expected" );
    }
    ValDesc* val = el->first->value;
    if ( val->type != VT_INT && val->type != VT_REAL ) {
        throw Exception( "syntax error: number expected" );
    }
}

ExprList* Interpreter::getSignedExpr() {
    // sign-op     := '-' | '+' .
    // signed-expr := [ sign-op ] num-base-expr .
    uint16_t tok = scan.tokType();
    if ( tok != T_MINUS && tok != T_PLUS ) tok = T_EOL; else skipTok();
    ExprList* el = getNumBaseExpr();
    if ( tok == T_EOL ) return el;
    if ( el == 0 ) throw Exception( "syntax error: expression expected" );
    verifySingleNumber( el );
    if ( tok == T_PLUS ) return el;
    el->first->value->alu( tok );
    return el;
}

ExprList* Interpreter::getNotExpr() {
    // not-op   := 'NOT' .
    // not-expr := [ not-op ] signed-expr .
    uint16_t tok = scan.tokType();
    if ( tok != KW_NOT ) tok = T_EOL; else skipTok();
    ExprList* el = getSignedExpr();
    if ( tok == T_EOL ) return el;
    if ( el == 0 ) throw Exception( "syntax error: expression expected" );
    verifySingleNumber( el );
    if ( el->first->value->type == VT_REAL ) el->first->demoteRealToInt();
    el->first->value->alu( tok );
    return el;
}

void Interpreter::autoPromote( ExprInfo* ei1, ExprInfo* ei2, bool harder ) {
    if ( harder ) {
        if ( ei1->value->type == VT_INT ) ei1->promoteIntToReal();
        if ( ei2->value->type == VT_INT ) ei2->promoteIntToReal();
        return;
    } 
    if ( ei1->value->type == VT_INT && ei2->value->type == VT_REAL ) {
        if ( ei1->value->type == VT_INT ) ei1->promoteIntToReal();
        return;
    }
    if ( ei1->value->type == VT_REAL && ei2->value->type == VT_INT ) {
        if ( ei2->value->type == VT_INT ) ei2->promoteIntToReal();
        return;
    }
}

void Interpreter::autoDemote( ExprInfo* ei1, ExprInfo* ei2 ) {
    if ( ei1->value->type == VT_REAL ) ei1->demoteRealToInt();
    if ( ei2->value->type == VT_REAL ) ei2->demoteRealToInt();
}

ExprList* Interpreter::getMultExpr() {
    // mult-op   := '*' | '/' .
    // mult-expr := not-expr { mult-op not-expr } .
    ExprList* el = getNotExpr();
    if ( el == 0 ) return 0;

    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok != T_TIMES && tok != T_DIV ) break;
        verifySingleNumber( el );
        if ( !scan.skipTok() ) { 
            delete el; 
            throw Exception( "interpret error: bad token");
        }
        ExprList* el2 = getNotExpr();
        if ( el2 == 0 ) {
            delete el;
            throw Exception( "syntax error: expression expected" );
        }
        verifySingleNumber( el2 );
        autoPromote( el->first, el2->first );
        el->first->value->alu( tok, el2->first->value );
        delete el2;
    }

    return el;
}

ExprList* Interpreter::getPowExpr() {
    // pow-op   := '**' | '^' .
    // pow-expr := mult-expr { pow-op mult-expr } .
    ExprList* el = getMultExpr();
    if ( el == 0 ) return 0;

    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok != T_POW ) break;
        verifySingleNumber( el );
        if ( !scan.skipTok() ) { 
            delete el; 
            throw Exception( "interpret error: bad token");
        }
        ExprList* el2 = getMultExpr();
        if ( el2 == 0 ) {
            delete el;
            throw Exception( "syntax error: expression expected" );
        }
        verifySingleNumber( el2 );
        autoPromote( el->first, el2->first, true );
        el->first->value->alu( tok, el2->first->value );
        delete el2;
    }

    return el;
}

ExprList* Interpreter::getAddExpr() {
    // add-op   := '-' | '+' .
    // add-expr := pow-expr { add-op pow-expr } .

    ExprList* el = getPowExpr();
    if ( el == 0 ) return 0;

    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok != T_MINUS && tok != T_PLUS ) break;
        verifySingleNumber( el );
        if ( !scan.skipTok() ) { 
            delete el; 
            throw Exception( "interpret error: bad token");
        }
        ExprList* el2 = getPowExpr();
        if ( el2 == 0 ) {
            delete el;
            throw Exception( "syntax error: expression expected" );
        }
        verifySingleNumber( el2 );
        autoPromote( el->first, el2->first );
        el->first->value->alu( tok, el2->first->value );
        delete el2;
    }

    return el;
}

ExprList* Interpreter::getShiftExpr() {
    // shift-op   := 'SHL' | 'SHR' .
    // shift-expr := add-expr [ shift-op add-expr ] .

    ExprList* el = getAddExpr();
    if ( el == 0 ) return 0;

    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok != KW_SHL && tok != KW_SHR ) break;
        verifySingleNumber( el );
        if ( !scan.skipTok() ) { 
            delete el; 
            throw Exception( "interpret error: bad token");
        }
        ExprList* el2 = getAddExpr();
        if ( el2 == 0 ) {
            delete el;
            throw Exception( "syntax error: expression expected" );
        }
        verifySingleNumber( el2 );
        autoDemote( el->first, el2->first );
        el->first->value->alu( tok, el2->first->value );
        delete el2;
    }

    return el;
}

ExprList* Interpreter::getCmpExpr() {
    // cmp-op   := '<' | '>' | '<=' | '>=' | '=' | '<>' .
    // cmp-expr := shift-expr [ cmp-op shift-expr ] .

    ExprList* el = getShiftExpr();
    if ( el == 0 ) return 0;

    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok != T_EQ && tok != T_NE && tok != T_LT && tok != T_GT &&
            tok != T_GE && tok != T_LE ) break;
        verifySingleNumber( el );
        if ( !scan.skipTok() ) { 
            delete el; 
            throw Exception( "interpret error: bad token");
        }
        ExprList* el2 = getShiftExpr();
        if ( el2 == 0 ) {
            delete el;
            throw Exception( "syntax error: expression expected" );
        }
        verifySingleNumber( el2 );
        autoPromote( el->first, el2->first );
        el->first->value->alu( tok, el2->first->value );
        delete el2;
        if ( el->first->value->type == VT_REAL ) el->first->demoteRealToInt();
        break;
    }

    return el;
}

ExprList* Interpreter::getNumExpr() {
    // num-expr := cmp-expr .
    return getCmpExpr();
}

void Interpreter::verifySingleString( ExprList* el ) {
    if ( el->first == 0 || el->first != el->last ) {
        throw Exception( "syntax error: single string expected" );
    }
    ValDesc* val = el->first->value;
    if ( val->type != VT_STR ) {
        throw Exception( "syntax error: string expected" );
    }
}

ExprList* Interpreter::getConcatExpr() {
    // concat-op   := '+' .
    // concat-expr := str-base-expr { concat-op str-base-expr } .
    ExprList* el = getStrBaseExpr();
    if ( el == 0 ) return 0;

    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok != T_PLUS ) break;
        verifySingleString( el );
        if ( !scan.skipTok() ) { 
            delete el; 
            throw Exception( "interpret error: bad token");
        }
        ExprList* el2 = getStrBaseExpr();
        if ( el2 == 0 ) {
            delete el;
            throw Exception( "syntax error: expression expected" );
        }
        verifySingleString( el2 );
        el->first->value->alu( tok, el2->first->value );
        delete el2;
    }

    return el;
}

ExprList* Interpreter::getStrCmpExpr() {
    // str-cmp-expr := concat-expr [ cmp-op concat-expr ] .
    ExprList* el = getConcatExpr();
    if ( el == 0 ) return 0;

    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok != T_EQ && tok != T_NE && tok != T_LT && tok != T_GT && 
            tok != T_LE && tok != T_GE ) break;
        verifySingleString( el );
        if ( !scan.skipTok() ) { 
            delete el; 
            throw Exception( "interpret error: bad token");
        }
        ExprList* el2 = getStrBaseExpr();
        if ( el2 == 0 ) {
            delete el;
            throw Exception( "syntax error: expression expected" );
        }
        verifySingleString( el2 );
        el->first->value->alu( tok, el2->first->value );
        delete el2;
        el->first->changeStrToInt();
        break;
    }

    return el;
}

ExprList* Interpreter::getStrExpr() {
    // str-expr := str-cmp-expr .
    return getStrCmpExpr();
}

ExprList* Interpreter::getBaseExpr() {
    // base-expr := num-expr | str-expr .
    ExprList* el = getNumExpr();
    if ( el ) return el;
    return getStrExpr();
}

ExprList* Interpreter::getAndExpr() {
    // and-op   := 'AND' | 'NAND' .
    // and-expr := base-expr { and-op base-expr } .
    ExprList* el = getBaseExpr();
    if ( el == 0 ) return 0;

    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok != KW_AND && tok != KW_NAND ) break;
        verifySingleNumber( el );
        if ( !scan.skipTok() ) { 
            delete el; 
            throw Exception( "interpret error: bad token");
        }
        ExprList* el2 = getBaseExpr();
        if ( el2 == 0 ) {
            delete el;
            throw Exception( "syntax error: expression expected" );
        }
        verifySingleNumber( el2 );
        autoDemote( el->first, el2->first );
        el->first->value->alu( tok, el2->first->value );
        delete el2;
    }

    return el;
}

ExprList* Interpreter::getOrExpr() {
    // or-op   := 'OR' | 'XOR' | 'NOR' | 'XNOR' .
    // or-expr := and-expr { log-op and-expr } .
    ExprList* el = getAndExpr();
    if ( el == 0 ) return 0;

    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok != KW_OR && tok != KW_NOR && tok != KW_XOR && tok != KW_XNOR ) break;
        verifySingleNumber( el );
        if ( !scan.skipTok() ) { 
            delete el; 
            throw Exception( "interpret error: bad token");
        }
        ExprList* el2 = getAndExpr();
        if ( el2 == 0 ) {
            delete el;
            throw Exception( "syntax error: expression expected" );
        }
        verifySingleNumber( el2 );
        autoDemote( el->first, el2->first );
        el->first->value->alu( tok, el2->first->value );
        delete el2;
    }

    return el;
}

ExprList* Interpreter::getExpr() {
    // expr := or-expr .
    return getOrExpr();
}

ExprList* Interpreter::getExprList() {
    // expr-list := expr { ',' expr } .

    ExprList* el = getExpr();
    if ( el == 0 ) return 0;

    ExprList* res = new ExprList();

    for (;;) {
        res->moveFrom( el ); delete el;

        uint16_t tok = scan.tokType();
        if ( tok != T_COMMA ) break;

        if ( !scan.skipTok() ) {
            delete res;
            throw Exception( "interpret error: bad token" );
        }

        el = getExpr();
        if ( el == 0 ) {
            delete res;
            throw Exception( "syntax error: expression expected after comma" );
        }
    }

    return res;
}

ExprList* Interpreter::getAssignLvalue() {
    // assign-lvalue := num-ident-expr | str-ident-expr .
    IdentInfo ii;
    if ( !getNumIdentExpr( ii ) ) {
        if ( !getStrIdentExpr( ii ) ) return 0;
    }
    ExprInfo* ei = new ExprInfo( ii.desc, false );
    ei->param = ii.param; ii.param = 0;
    ExprList* el = new ExprList();
    el->add( ei );
    return el;
}

ExprList* Interpreter::getLvalueList() {
    // lvalue-list := assign-lvalue { ',' assign-lvalue } .
    ExprList* el = getAssignLvalue();
    if ( el == 0 ) return 0;

    ExprList* res = new ExprList();

    for (;;) {
        res->moveFrom( el ); delete el;

        uint16_t tok = scan.tokType();
        if ( tok != T_COMMA ) break;

        if ( !scan.skipTok() ) {
            delete res;
            throw Exception( "interpret error: bad token" );
        }

        el = getAssignLvalue();
        if ( el == 0 ) {
            delete res;
            throw Exception( "syntax error: lvalue expected after comma" );
        }
    }

    return res;
}

bool Interpreter::getAssignment( ExprList*& lvalues, ExprList*& rvalues ) {
    // assignment := [ 'LET' ] lvalue-list '=' expr-list .
    ExprList* el1 = getLvalueList();
    if ( el1 == 0 ) return false;
    uint16_t tok = scan.tokType();
    if ( tok != T_EQ || !scan.skipTok() ) {
        delete el1;
        throw Exception( "syntax error: '=' expected" );
    }
    ExprList* el2 = getExprList();
    if ( el2 == 0 ) {
        delete el1;
        throw Exception( "syntax error: expression(s) expected" );
    }
    if ( el1->count() != el2->count() ) {
        delete el2;
        delete el1;
        throw Exception( "syntax error: pairing mismatch" );
    }
    lvalues = el1;
    rvalues = el2;
    return true;
}

void Interpreter::assignBaseType( ValDesc* target, ValDesc* source ) {
    ValueType vt1 = target->type;
    ValueType vt2 = source->type;
    if ( vt1 == VT_STR ) {
        if ( vt2 != VT_STR ) throw Exception( "type mismatch" );
        uint8_t* text = 0; size_t len = 0; bool bFree = false;
        source->getStrVal( text, len, bFree );
        target->setStrVal( text, len );
        if ( bFree ) delete [] text;

    } else if ( vt1 == VT_INT ) {
        if ( vt2 != VT_INT && vt2 != VT_REAL ) throw Exception( "type mismatch" );
        target->setIntVal( source->getIntVal() );

    } else if ( vt1 == VT_REAL ) {
        if ( vt2 != VT_INT && vt2 != VT_REAL ) throw Exception( "type mismatch" );
        target->setRealVal( source->getRealVal() );
    } else {
        throw Exception( "bad base type" );
    }
}

void Interpreter::doAssignment( const ExprList* lvalues, const ExprList* rvalues ) {
    if ( lvalues == 0 || rvalues == 0 ) return;
    const ExprInfo* ei1 = lvalues->first;
    const ExprInfo* ei2 = rvalues->first;
    while ( ei1 && ei2 ) {
        ValueType vt1 = ei1->value->type;

        if ( vt1 == VT_STR || vt1 == VT_INT || vt1 == VT_REAL ) {
            assignBaseType( ei1->value, ei2->value );

        } else if ( vt1 == VT_ARY ) {
            AryVal* av = 0; ValDesc** args = 0;
            fillArrayArgs( ei1->value, ei1->param, av, args );
            try {
                ValDesc* cell = av->subscript( args );
                switch ( av->elemType ) {
                    case VT_STR: case VT_INT: case VT_REAL:
                        assignBaseType( cell, ei2->value );
                        break;
                    default:
                        throw Exception( "unsupported array element type" );
                }
            } catch ( const Exception& xcpt ) {
                delete [] args;
                throw;
            }
            delete [] args;

        } else if ( vt1 == VT_FUNC ) {
            // TODO: LEFT$() etc.
            throw Exception( "assignment to function" );
            
        }
        ei1 = ei1->next;
        ei2 = ei2->next;
    }
}


bool Interpreter::getLineNo( uint32_t& rLineNo ) {
   uint16_t tok = scan.tokType();
    if ( tok != T_LINENO ) return false;
    if ( !scan.getLineNo( rLineNo ) ) return false;
    scan.skipTok();
    return true;
 }

bool Interpreter::getLineNoExpr( uint32_t& lineNo1, uint32_t& lineNo2 ) {
    bool hadFirst = getLineNo( lineNo1 );
    uint16_t tok = scan.tokType();
    if ( tok == T_MINUS ) {
        skipTok();
        getLineNo( lineNo2 );
    } else if ( hadFirst ) {
        lineNo2 = lineNo1;
    }
    return true;
}

void Interpreter::list() {
    uint32_t lineNo1 = 0;
    uint32_t lineNo2 = UINT32_MAX;
    getLineNoExpr( lineNo1, lineNo2 );
    size_t count = prog.getLineInfoCount();
    for ( size_t pos=0; pos < count; ++pos ) {
        const LineInfo& li = prog.getLineInfoAt( pos );
        if ( li.lineNo < lineNo1 || li.lineNo > lineNo2 ) continue;
        prog.setReadPos( li.offset );
        const uint8_t* ptr = prog.readBlock( li.length );
        if ( ptr == 0 ) throw Exception( "list error: bad read" );
        Detokenizer d( ptr );
        const char* text = d.detokenize();
        if ( text == 0 ) throw Exception( "list error: detokenization failed" );
        printf( "%s\n", text );
    }
}

void Interpreter::funcHandler( FuncArg* arg ) {
    FnArg* fnArg = dynamic_cast<FnArg*>( arg );
    if ( fnArg == 0 ) throw Exception( "call error: bad function" );
    FnMethodPtr mth = fnArg->mth;
    (fnArg->intp->*mth)( fnArg );
}

void Interpreter::declareCmd( const CmdDecl& decl ) {
    uint16_t tok = decl.tok;
    commandHt.enter( new CmdHashEnt( tok, decl.mth ) );
}

void Interpreter::declareFunc( const FnDecl& decl ) {
    const char* name0 = Keywords::getInstance().lookup( decl.tok );
    if ( name0 == 0 ) throw Exception( "declare error: keyword not found" );
    const uint8_t* name    = (const uint8_t*) name0;
    size_t         nameLen = name[-1];
    FnArg* arg = new FnArg;
    arg->intp = this;
    arg->mth  = decl.mth;
    FuncVal* val = new FuncVal( decl.type, decl.nForm, decl.nOpt, decl.nRes,
        decl.bVarArgs, funcHandler, arg );
    if ( !vars.addVar( name, nameLen, val ) ) {
        throw Exception( "declare error: failed to add variable" );
    }    
}

void Interpreter::declare() {
    for ( int i=0; cmdDeclTable[i].tok; ++i ) {
        declareCmd( cmdDeclTable[i] );
    }
    clearVars( true );
}

void Interpreter::clearVars( bool declareOnly ) {
    if ( !declareOnly ) vars.clear();
    for ( int i=0; funcDeclTable[i].tok; ++i ) {
        declareFunc( funcDeclTable[i] );
    }
}

Interpreter::Interpreter() {    
    declare();
}

Interpreter::~Interpreter() {
}

void Interpreter::interpret() {
    for (;;) {
        uint16_t tok = scan.tokType();
        if ( tok == T_EOL ) break;
        skipTok();
        if ( tok == T_LINENO || tok == T_LABEL || tok == T_COLON ) continue;
        HashEntry* he = commandHt.find( (const uint8_t*)(&tok), 2U );
        if ( he ) {
            CmdHashEnt* cmd = dynamic_cast<CmdHashEnt*>( he );
            if ( cmd == 0 ) {
                throw Exception( "interpret error: bad command" );
            }
            CmdMethodPtr mth = cmd->mth;
            (this->*mth)();
            continue;
        }
        throw Exception( "interpret error: command not implemented" );
    }
}

void Interpreter::interpretLine( const char* line ) {
    Tokenizer t( (const uint8_t*) line, strlen(line) );
    uint16_t tok = t.tokenize();
    if ( tok != T_EOL ) {
        // TODO: error handling
        printf( "error, %d\n", tok );

        return;
    }
    scan.setPos( t.getTokBufAddr() );
    tok = scan.tokType();
    if ( tok == T_LINENO ) {
        uint32_t lineNo;
        if ( !scan.getLineNo( lineNo ) ) return;
        prog.enterLine( t );
        return;
    }
    interpret();
}
