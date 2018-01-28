##
##  This file is part of nanomidi.
##
##  Copyright (C) 2018 Adam Heinrich <adam@adamh.cz>
##
##  Nanomidi is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  Nanomidi is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with nanomidi.  If not, see <http://www.gnu.org/licenses/>.
##

TARGETS := example-encode
TARGETS += example-decode

NANOMIDI_DIR = ..

HEADERS := $(wildcard $(NANOMIDI_DIR)/include/*.h)
SOURCES := $(wildcard $(NANOMIDI_DIR)/src/*.c)
HEADERS += $(wildcard *.h)
SOURCES += common.c

OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

CC := gcc

CFLAGS = -std=c99 -g -Wall -pedantic -I$(NANOMIDI_DIR)/include
CFLAGS += -Wextra -Wconversion -Wdouble-promotion -Wfloat-conversion
LDFLAGS = $(CFLAGS)

.PHONY: all
all: $(TARGETS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

example-encode: $(OBJECTS) encode.o
	$(CC) $^ $(LDFLAGS) -o $@

example-decode: $(OBJECTS) decode.o
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(NANOMIDI_DIR)/src/*.o
	rm -f $(TARGETS)
