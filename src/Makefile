###################################################
# DatGen Makefile
# December 14, 1999
#
# mailto:support@datgen.com
###################################################

# ANSI C compiler
CC=gcc

# compiler flags
#CFLAGS=-g -ansi -pedantic -s -static
CFLAGS= -O2 -ansi -pedantic

# libraries (-lm -> math)
LIBS=-lm

datgen: datgen.c
	${CC} ${CFLAGS} datgen.c ${LIBS} -o datgen

###################################################
