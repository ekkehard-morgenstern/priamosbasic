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
	tokenizer.h types.h variables.h 

MODULES=bytebuffer.o exception.o hashtable.o interpreter.o \
	tokenizer.o types.o variables.o

APP_MODULES=main.o $(MODULES)
TEST1_MODULES=testhashtable.o $(MODULES)

LIBS=-lm -lrt

APP=pribasic
TEST1=testhashtable

.cpp.o:
	$(CXX) -o $@ $<

all: $(APP) $(TEST1)
	echo ok >all

$(APP): $(APP_MODULES)
	$(LXX) -o $(APP) $(APP_MODULES) $(LIBS)

$(TEST1): $(TEST1_MODULES)
	$(LXX) -o $(TEST1) $(TEST1_MODULES) $(LIBS)

bytebuffer.o: bytebuffer.cpp $(INCFILES)

exception.o: exception.cpp $(INCFILES)

hashtable.o: hashtable.cpp $(INCFILES)

interpreter.o: interpreter.cpp $(INCFILES)

main.o: main.cpp $(INCFILES)

tokenizer.o: tokenizer.cpp $(INCFILES)

types.o: types.cpp $(INCFILES)

variables.o: variables.cpp $(INCFILES)

testhashtable.o: testhashtable.cpp $(INCFILES)
