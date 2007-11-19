# makefile for liblas.a
#
#COPTS    = -g
#COMPILER  = CC
#LIB_LINKER  = CC -ar
COMPILER  = g++
LIB_LINKER  = g++ -shared
COPTS     = -O3
#BITS     = -64

LIBS     = -L/usr/lib32
#LIBS     = -L/usr/lib64
INCLUDE  = -I/usr/include/ -I../inc

OBJ_LAS = lasreader.o laswriter.o

all: liblas.a

liblas.a: ${OBJ_LAS}
	${LIB_LINKER} ${BITS} ${COPTS} ${OBJ_LAS} -o $@ ${LIBS} $(INCLUDE)
	cp $@ ../lib

.cpp.o:
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $< -o $@

.c.o: 
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $< -o $@
