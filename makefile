CC=gcc
CFLAGS= -std=c99 -Iinclude -Wall -g -O0 -Wwrite-strings -Wshadow -pedantic-errors -fstack-protector-all
LDFLAGS= -L$(OBJ)

SRC=src
INCLUDE=include
BIN=build
OBJ=$(BIN)/obj
TEST_SRC=tests
TEST_BIN=$(BIN)/tests

_LIB_OBJS=libstring.so libtest.so
LIB_OBJS=$(patsubst %,$(OBJ)/%,$(_LIB_OBJS))

_TESTS=string_tests
TESTS=$(patsubst %,$(TEST_BIN)/%,$(_TESTS))

.PHONY: all clean test

all: $(OBJ) $(LIB_OBJS)

test: $(OBJ) $(TEST_BIN) $(TESTS)

$(OBJ)/libtest.so: $(SRC)/test.c $(INCLUDE)/ilc/test.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

$(OBJ)/libstring.so: $(SRC)/string.c $(INCLUDE)/ilc/string.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $<

$(TEST_BIN)/string_tests: $(OBJ)/string_tests.o $(OBJ)/libstring.so $(OBJ)/libtest.so
	$(CC) $(LDFLAGS) -o $@ $< -lstring -ltest

$(OBJ)/string_tests.o: $(TEST_SRC)/string_tests.c $(INCLUDE)/ilc/string.h $(INCLUDE)/ilc/test.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ):
	mkdir -p $(OBJ)

$(TEST_BIN):
	mkdir -p $(TEST_BIN)

clean:
	rm -rf $(BIN)
