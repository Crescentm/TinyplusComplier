# Detect system OS

ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname -s 2>/dev/null || echo not')
endif

# Clean the default value of CC.
CC=

# Detect proper C compiler by system OS.
ifndef CC
	ifeq ($(detected_OS),Windows)
		CC=gcc
	else ifeq ($(detected_OS),Darwin)
		CC=clang
	else
		CC=gcc
	endif
endif

ifeq ($(detected_OS),Windows)
	RM=del /q /f
endif

ifeq ($(detected_OS),Windows)
	tiny=tiny.exe
else
	tiny=tiny.out
endif

ifeq ($(detected_OS),Windows)
	tm=tm.exe
else
	tm=tm.out
endif

CFLAGS = 

OBJS = main.o util.o scan.o parse.o symtab.o analyze.o code.o cgen.o

tiny: $(OBJS)
	$(CC) $(CFLAGS) -o $(tiny) $(OBJS)
	$(RM) *.o

tm: tm.c
	$(CC) $(CFLAGS) -o $(tiny) tm.c
	$(RM) *.o

main.o: main.c globals.h util.h scan.h parse.h analyze.h cgen.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

scan.o: scan.c scan.h util.h globals.h
	$(CC) $(CFLAGS) -c scan.c

parse.o: parse.c parse.h scan.h globals.h util.h
	$(CC) $(CFLAGS) -c parse.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c globals.h symtab.h analyze.h
	$(CC) $(CFLAGS) -c analyze.c

code.o: code.c code.h globals.h
	$(CC) $(CFLAGS) -c code.c

cgen.o: cgen.c globals.h symtab.h code.h cgen.h
	$(CC) $(CFLAGS) -c cgen.c

clean:
	$(RM) *.out *.exe

all: tiny tm

