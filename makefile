CC=gcc
CFLAGS= -std=c99 -Iinclude -Wall -g -O0 -Wwrite-strings -Wshadow -pedantic-errors -fstack-protector-all
LDFLAGS= -L$(OBJ)

SRC=src
INCLUDE=include
BIN=build
OBJ=$(BIN)/obj
TEST_BIN=$(BIN)/tests

_LIB_OBJS=string.so test.so
LIB_OBJS=$(patsubst %,$(OBJ)/%,$(_LIB_OBJS))

.PHONY: all clean test

all: $(OBJ) $(LIB_OBJS)

$(OBJ)/test.so: $(SRC)/test.c $(INCLUDE)/ilc/test.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

$(OBJ)/string.so: $(SRC)/string.c $(INCLUDE)/ilc/string.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

$(OBJ):
	mkdir -p $(OBJ)

clean:
	rm -rf $(OBJ)
