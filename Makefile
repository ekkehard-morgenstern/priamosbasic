# PriamosBASIC - a BASIC interpreter written in C++
# Copyright (C) 2019  Ekkehard Morgenstern
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
# NOTE: Programs created with PriamosBASIC do not fall under this license.
# CONTACT INFO:
#     E-Mail: ekkehard@ekkehardmorgenstern.de
#     Mail: Ekkehard Morgenstern, Mozartstr. 1, D-76744 Woerth am Rhein, Germany, Europe */

CXXCOMP=g++

ifdef DEBUG
CPPFLAGS=-g
else
CPPFLAGS=-O3
endif

CPPFLAGS+= -Wall

CXX=$(CXXCOMP) -c $(CPPFLAGS)
LXX=$(CXXCOMP) $(CPPFLAGS)


INCFILES=bytebuffer.h exception.h hashtable.h interpreter.h \
	tokenizer.h types.h variables.h keywords.h tokens.h \
	tokenscanner.h detokenizer.h

MODULES=bytebuffer.o exception.o hashtable.o interpreter.o \
	tokenizer.o types.o variables.o keywords.o tokenscanner.o \
	detokenizer.o

APP_MODULES=main.o $(MODULES)
TEST1_MODULES=testhashtable.o $(MODULES)
TEST2_MODULES=testtokenizer.o $(MODULES)
TEST3_MODULES=testtokenizer2.o $(MODULES)
TEST4_MODULES=testinterpreter.o $(MODULES)

LIBS=-lm -lrt

APP=pribasic
TEST1=testhashtable
TEST2=testtokenizer
TEST3=testtokenizer2
TEST4=testinterpreter

.cpp.o:
	$(CXX) -o $@ $<

all: $(APP) $(TEST1) $(TEST2) $(TEST3) $(TEST4)
	echo ok >all

$(APP): $(APP_MODULES)
	$(LXX) -o $(APP) $(APP_MODULES) $(LIBS)

$(TEST1): $(TEST1_MODULES)
	$(LXX) -o $(TEST1) $(TEST1_MODULES) $(LIBS)

$(TEST2): $(TEST2_MODULES)
	$(LXX) -o $(TEST2) $(TEST2_MODULES) $(LIBS)

$(TEST3): $(TEST3_MODULES)
	$(LXX) -o $(TEST3) $(TEST3_MODULES) $(LIBS)

$(TEST4): $(TEST4_MODULES)
	$(LXX) -o $(TEST4) $(TEST4_MODULES) $(LIBS)

bytebuffer.o: bytebuffer.cpp $(INCFILES)

exception.o: exception.cpp $(INCFILES)

hashtable.o: hashtable.cpp $(INCFILES)

interpreter.o: interpreter.cpp $(INCFILES)

main.o: main.cpp $(INCFILES)

tokenizer.o: tokenizer.cpp $(INCFILES)

types.o: types.cpp $(INCFILES)

variables.o: variables.cpp $(INCFILES)

keywords.o: keywords.cpp $(INCFILES)

tokenscanner.o: tokenscanner.cpp $(INCFILES)

detokenizer.o: detokenizer.cpp $(INCFILES)

testhashtable.o: testhashtable.cpp $(INCFILES)

testtokenizer2.o: testtokenizer2.cpp $(INCFILES)

testinterpreter.o: testinterpreter.cpp $(INCFILES)
