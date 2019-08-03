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

#ifndef TOKENS_H
#define TOKENS_H    1

#define KW_NOP 0X0300
#define KW_END 0X0301
#define KW_AGAIN 0X0302
#define KW_LEAVE 0X0303
#define KW_BREAK 0X0304
#define KW_ITERATE 0X0305
#define KW_CONT 0X0306
#define KW_STOP 0X0307
#define KW_RESTORE 0X0308
#define KW_READ 0X0309
#define KW_DATA 0X030A
#define KW_FOR 0X030B
#define KW_RETURN 0X030C
#define KW_GOSUB 0X030D
#define KW_GOTO 0X030E
#define KW_RESET 0X030F
#define KW_FORTH 0X0310
#define KW_SEND 0X0311
#define KW_BIND 0X0312
#define KW_BLOCK 0X0313
#define KW_NONBLOCK 0X0314
#define KW_LISTEN 0X0315
#define KW_CONNECT 0X0316
#define KW_OPEN 0X0317
#define KW_CLOSE 0X0318
#define KW_INPUT 0X0319
#define KW_REWIND 0X031A
#define KW_SEEK 0X031B
#define KW_LET 0X031C
#define KW_IF 0X031D
#define KW_UNLESS 0X031E
#define KW_NEW 0X031F
#define KW_OLD 0X0320
#define KW_SAVE 0X0321
#define KW_LOAD 0X0322
#define KW_DIR 0X0323
#define KW_CHDIR 0X0324
#define KW_PUSHDIR 0X0326
#define KW_POPDIR 0X0327
#define KW_RUN 0X0328
#define KW_LIST 0X0329
#define KW_DELETE 0X032A
#define KW_RENUM 0X032B
#define KW_HELP 0X032C
#define KW_QHELP 0X032E
#define KW_WHY 0X032F
#define KW_CALL 0X0330
#define KW_RESULT 0X0331
#define KW_OPTION 0X0332
#define KW_DEF 0X0333
#define KW_DIM 0X0334
#define KW_NEXT 0X0335
#define KW_CLR 0X0336
#define KW_RANDOMIZE 0X0337
#define KW_DEG 0X0338
#define KW_RAD 0X0339
#define KW_WHILE 0X033A
#define KW_WEND 0X033B
#define KW_REPEAT 0X033C
#define KW_UNTIL 0X033D
#define KW_FOREVER 0X033E
#define KW_FOREACH 0X033F
#define KW_WARRANTY 0X0340
#define KW_CONDITIONS 0X0341
#define KW_ASCFN 0X0600
#define KW_VALFN 0X0601
#define KW_STRSFN 0X0602
#define KW_TI 0X0603
#define KW_TIS 0X0604
#define KW_LEFTSFN 0X0605
#define KW_RIGHTSFN 0X0606
#define KW_MIDSFN 0X0607
#define KW_POSFN 0X0608
#define KW_HPOSFN 0X0609
#define KW_VPOSFN 0X060A
#define KW_BINSFN 0X060B
#define KW_OCTSFN 0X060C
#define KW_DECSFN 0X060D
#define KW_HEXSFN 0X060E
#define KW_CVIFN 0X060F
#define KW_CVFFN 0X0610
#define KW_MKISFN 0X0611
#define KW_MKFSFN 0X0612
#define KW_WHERES 0X0613
#define KW_IPV4SFN 0X0614
#define KW_IPV6SFN 0X0615
#define KW_HOSTNAMES 0X0616
#define KW_DOMAINS 0X0617
#define KW_RECVSFN 0X0618
#define KW_SOCKETV4 0X0619
#define KW_SOCKETV6 0X061A
#define KW_ACCEPTFN 0X061B
#define KW_CWDS 0X061C
#define KW_RNDFN 0X061D
#define KW_SINFN 0X061E
#define KW_LNFN 0X061F
#define KW_LOGFN 0X0620
#define KW_LOG2FN 0X0621
#define KW_COSFN 0X0622
#define KW_TANFN 0X0623
#define KW_COTFN 0X0624
#define KW_ATNFN 0X0625
#define KW_HEADFN 0X0626
#define KW_TAILFN 0X0627
#define KW_TRUE 0X0628
#define KW_FALSE 0X0629
#define KW_NIL 0X062A
#define KW_NEXTFN 0X062B
#define KW_PREVFN 0X062C
#define KW_CELLSFN 0X062D
#define KW_SUB 0X0B00
#define KW_FUNC 0X0B01
#define KW_BASE 0X0B02
#define KW_BYTEORDER 0X0B03
#define KW_INT 0X0B04
#define KW_FLOAT 0X0B05
#define KW_FIXED 0X0B06
#define KW_FN 0X0B07
#define KW_DYNAMIC 0X0B08
#define KW_PTR 0X0B09
#define KW_LINE 0X0B0A
#define KW_IN 0X0B0B
#define KW_LABEL 0X0B0C
#define KW_AND 0X0F03
#define KW_OR 0X0F04
#define KW_XOR 0X0F05
#define KW_NOT 0X0F06
#define KW_NAND 0X0F07
#define KW_NOR 0X0F08
#define KW_XNOR 0X0F09
#define KW_EQV 0X0F10
#define KW_NEQV 0X0F11
#define KW_SHL 0X0F12
#define KW_SHR 0X0F13

#define T_EOL           0X00
#define T_IDENT         0X05
#define T_STRLIT        0X07
#define T_LINENO        0X08
#define T_NUMLIT        0X09
#define N_MSKHI         0XF0
#define N_MSKLO         0X0F
#define NH_BIN          0X20
#define NH_DEC          0X40
#define NH_OCT          0XF0
#define NH_HEX          0X80
#define NL_I8           0X00
#define NL_I16          0X01
#define NL_I32          0X02
#define NL_I64          0X03
#define NL_F32          0X0E
#define NL_F64          0X0F
#define T_LABEL         0X0C
#define T_LE            0X0F00
#define T_GE            0X0F01
#define T_NE            0X0F02
#define T_SBI           0X11
#define T_SPC           0X20
#define T_PLING         0X21
#define T_REM           0X27
#define T_LPAREN        0X28
#define T_RPAREN        0X29
#define T_TIMES         0X2A
#define T_PLUS          0X2B
#define T_COMMA         0X2C
#define T_MINUS         0X2D
#define T_DIV           0X2F
#define T_COLON         0X3A
#define T_SEMIC         0X3B
#define T_LT            0X3C
#define T_EQ            0X3D
#define T_GT            0X3E
#define T_PRINT         0X3F
#define T_LBRACK        0X5B
#define T_RBRACK        0X5D
#define T_LBRACE        0X7B
#define T_LEXT          0X7C
#define T_RBRACE        0X7D

#define T_SYNERR        0XFFFF  // general syntax error
#define T_UNIMPL        0XFFFE  // unimplemented feature
#define T_STRTRM        0XFFFD  // string not terminated
#define T_STRLNG        0XFFFC  // string too long
#define T_NUMLNG        0XFFFB  // numeric literal too long
#define T_NUMBAD        0XFFFA  // bad number


#endif
