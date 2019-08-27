TARGET=baash
CC=gcc
CPPFLAGS=`pkg-config --cflags glib-2.0`
CFLAGS=-std=c99 -Wall -Wextra -Wdeclaration-after-statement -Wbad-function-cast -Wstrict-prototypes -Wmissing-declarations -Wmissing-prototypes -Wno-unused-parameter -Werror -g -pedantic
LDFLAGS=`pkg-config --libs glib-2.0`

# Propagar entorno a make en tests/
export CC CPPFLAGS CFLAGS LDFLAGS

# Agregar bstring/ a los directorios de busqueda
BSTRING=bstrlib.c
vpath $(BSTRING) bstring

SOURCES=$(shell echo *.c) $(BSTRING)
OBJECTS=$(SOURCES:.c=.o)
PRECOMPILED=parser.o lexer.o

all: $(TARGET)

$(TARGET): $(OBJECTS) $(PRECOMPILED)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS) .depend *~
	make -C tests clean

test: $(OBJECTS)
	make -C tests test

test-command: command.o bstrlib.o
	make -C tests test-command

memtest: $(OBJECTS)
	make -C tests memtest

.depend: $(SOURCES)
	$(CC) $(CPPFLAGS) -MM $^ > $@

-include .depend

.PHONY: clean all
