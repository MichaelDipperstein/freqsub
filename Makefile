############################################################################
# Makefile for frequency substitution encode/decode programs
#
############################################################################

CC = gcc
LD = gcc
CFLAGS = -O3 -Wall -Wextra -ansi -pedantic -c
LDFLAGS = -O3 -o

# libraries
LIBS = -L. -Loptlist -lfreqsub -loptlist

# Treat NT and non-NT windows the same
ifeq ($(OS),Windows_NT)
	OS = Windows
endif

ifeq ($(OS),Windows)
	EXE = .exe
	DEL = del
else	#assume Linux/Unix
	EXE =
	DEL = rm
endif

all:		sample$(EXE)

sample$(EXE):	sample.o libfreqsub.a optlist/liboptlist.a
		$(LD) $^ $(LIBS) $(LDFLAGS) $@

sample.o:	sample.c freqsub.h optlist/optlist.h
		$(CC) $(CFLAGS) $<

libfreqsub.a:	freqsub.o
		ar crv libfreqsub.a freqsub.o
		ranlib libfreqsub.a

freqsub.o:	freqsub.c freqsub.h
		$(CC) $(CFLAGS) $<

optlist/liboptlist.a:
		cd optlist && $(MAKE) liboptlist.a

clean:
		$(DEL) *.o
		$(DEL) *.a
		$(DEL) sample$(EXE)
		cd optlist && $(MAKE) clean
