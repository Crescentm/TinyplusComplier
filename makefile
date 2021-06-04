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
		CC=clang++
	else
		CC=clang++
	endif
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

CFLAGS = -g

OBJS = main.o util.o scan.o parse.o symtab.o analyze.o execution.o

tiny: $(OBJS)
	$(CC) $(CFLAGS) -o $(tiny) $(OBJS)
	$(RM) *.o

main.o: main.cpp globals.h util.h scan.h parse.h analyze.h execution.h
	$(CC) $(CFLAGS) -c main.cpp

util.o: util.cpp util.h globals.h
	$(CC) $(CFLAGS) -c util.cpp

scan.o: scan.cpp scan.h util.h globals.h
	$(CC) $(CFLAGS) -c scan.cpp

parse.o: parse.cpp parse.h scan.h globals.h util.h
	$(CC) $(CFLAGS) -c parse.cpp

symtab.o: symtab.cpp symtab.h
	$(CC) $(CFLAGS) -c symtab.cpp

analyze.o: analyze.cpp globals.h symtab.h analyze.h
	$(CC) $(CFLAGS) -c analyze.cpp

execution.o: execution.cpp globals.h symtab.h execution.h
	$(CC) $(CFLAGS) -c execution.cpp

clean:
	$(RM) *.out *.exe *.o

all: tiny tm

