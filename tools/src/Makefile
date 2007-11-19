# makefile for SM and PS example use
#
#COPTS    = -g
COPTS     = -O3
#COMPILER  = CC
COMPILER  = g++
LINKER  = g++
#BITS     = -64

#LIBS     = -L/usr/lib64
LIBS     = -L/usr/lib32
INCLUDE  = -I/usr/include

LASLIBS     = -L../../lib
LASINCLUDE  = -I../../inc

all: lasinfo las2txt txt2las las2las lasdiff lasview lasmerge

#lasinfo: lasinfo.o
#	${LINKER} ${BITS} ${COPTS} lasinfo.o -llas -o $@ ${LIBS} ${LASLIBS} $(INCLUDE) $(LASINCLUDE)
#	mv $@ ../../bin

lasinfo: lasinfo.o ../../src/lasreader.o
	${LINKER} ${BITS} ${COPTS} lasinfo.o ../../src/lasreader.o -o $@ ${LIBS} $(INCLUDE) $(LASINCLUDE)
	mv $@ ../../bin

#las2txt: las2txt.o
#	${LINKER} ${BITS} ${COPTS} las2txt.o -llas -o $@ ${LIBS} ${LASLIBS} $(INCLUDE) $(LASINCLUDE)
#	mv $@ ../../bin

las2txt: las2txt.o ../../src/lasreader.o
	${LINKER} ${BITS} ${COPTS} las2txt.o ../../src/lasreader.o -o $@ ${LIBS} $(INCLUDE) $(LASINCLUDE)
	mv $@ ../../bin

#txt2las: txt2las.o
#	${LINKER} ${BITS} ${COPTS} txt2las.o -llas -o $@ ${LIBS} ${LASLIBS} $(INCLUDE) $(LASINCLUDE)
#	mv $@ ../../bin

txt2las: txt2las.o ../../src/laswriter.o
	${LINKER} ${BITS} ${COPTS} txt2las.o ../../src/laswriter.o -o $@ ${LIBS} $(INCLUDE) $(LASINCLUDE)
	mv $@ ../../bin

#las2las: las2las.o
#	${LINKER} ${BITS} ${COPTS} las2las.o -llas -o $@ ${LIBS} ${LASLIBS} $(INCLUDE) $(LASINCLUDE)
#	mv $@ ../../bin

las2las: las2las.o ../../src/lasreader.o ../../src/laswriter.o
	${LINKER} ${BITS} ${COPTS} las2las.o ../../src/lasreader.o ../../src/laswriter.o -o $@ ${LIBS} $(INCLUDE) $(LASINCLUDE)
	mv $@ ../../bin

#lasdiff: lasdiff.o
#	${LINKER} ${BITS} ${COPTS} lasdiff.o -llas -o $@ ${LIBS} ${LASLIBS} $(INCLUDE) $(LASINCLUDE)
#	mv $@ ../../bin

lasdiff: lasdiff.o ../../src/lasreader.o
	${LINKER} ${BITS} ${COPTS} lasdiff.o ../../src/lasreader.o -o $@ ${LIBS} $(INCLUDE) $(LASINCLUDE)
	mv $@ ../../bin

#lasview: lasview.o
#	${LINKER} ${BITS} ${COPTS} lasview.o -llas -lGL -lGLU -lglut -o $@ ${LIBS} ${LASLIBS} $(INCLUDE) $(LASINCLUDE)
#	mv $@ ../../bin

lasview: lasview.o ../../src/lasreader.o
	${LINKER} ${BITS} ${COPTS} lasview.o ../../src/lasreader.o -lGL -lGLU -lglut -o $@ ${LIBS} $(INCLUDE) $(LASINCLUDE)
	mv $@ ../../bin

lasmerge: lasmerge.o ../../src/lasreader.o ../../src/laswriter.o
	${LINKER} ${BITS} ${COPTS} lasmerge.o ../../src/lasreader.o ../../src/laswriter.o -o $@ ${LIBS} $(INCLUDE) $(LASINCLUDE)
	mv $@ ../../bin

.cpp.o: 
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $(LASINCLUDE) $< -o $@

.c.o: 
	${COMPILER} ${BITS} -c ${COPTS} ${INCLUDE} $(LASINCLUDE) $< -o $@
