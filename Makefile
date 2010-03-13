############################################################################
# Makefile for frequency substitution encode/decode programs
#
#   $Id: Makefile,v 1.1.1.1 2008/12/22 15:17:38 michael Exp $
#   $Log: Makefile,v $
#   Revision 1.1.1.1  2008/12/22 15:17:38  michael
#   Initial Release
#
############################################################################

CC = gcc
LD = gcc
CFLAGS = -O3 -Wall -ansi -pedantic -c
LDFLAGS = -O3 -o

# libraries
LIBS = -L. -lfreqsub -loptlist

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

sample$(EXE):	sample.o libfreqsub.a liboptlist.a
		$(LD) $^ $(LIBS) $(LDFLAGS) $@

sample.o:	sample.c freqsub.h optlist.h
		$(CC) $(CFLAGS) $<

libfreqsub.a:	freqsub.o
		ar crv libfreqsub.a freqsub.o
		ranlib libfreqsub.a

freqsub.o:	freqsub.c freqsub.h
		$(CC) $(CFLAGS) $<

liboptlist.a:	optlist.o
		ar crv liboptlist.a optlist.o
		ranlib liboptlist.a

optlist.o:	optlist.c optlist.h
		$(CC) $(CFLAGS) $<

clean:
		$(DEL) *.o
		$(DEL) *.a
		$(DEL) sample$(EXE)
