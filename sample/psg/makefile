DESTS=psg-test-001\
	psg-test-002\
	psg-test-003\
	psg-test-004\
	psg-test-005

OBJ001=\
	psg-test-001.o\
	square.o\
	scale.o\
	vgs2sound.o

OBJ002=\
	psg-test-002.o\
	square.o\
	scale.o\
	vgs2sound.o

OBJ003=\
	psg-test-003.o\
	kaeru.o\
	apc-test-003.o\
	square.o\
	scale.o\
	vgs2sound.o

OBJ004=\
	psg-test-004.o\
	kaeru2.o\
	apc-test-004.o\
	square.o\
	scale.o\
	vgs2sound.o

OBJ005=\
	psg-test-005.o\
	waon.o\
	apc-test-005.o\
	square.o\
	scale.o\
	vgs2sound.o

CFLAG=\
	-O3\
	-I$(VGS2_HOME)/template\
	-c

OSLIBS=\
	-lpthread

all:
	make all-`uname`

clean:
	-rm -f $(DESTS)
	-rm -f *.o

all-Linux:
	make build SOUNDLIB=-lasound

all-Darwin:
	make build SOUNDLIB="-framework OpenAL"

build: $(DESTS)

psg-test-001: $(OBJ001)
	gcc -o $@ $^ $(INC) $(SAL) $(SOUNDLIB) $(OSLIBS) 

psg-test-002: $(OBJ002)
	gcc -o $@ $^ $(INC) $(SAL) $(SOUNDLIB) $(OSLIBS) 

psg-test-003: $(OBJ003)
	gcc -o $@ $^ $(INC) $(SAL) $(SOUNDLIB) $(OSLIBS) 

psg-test-004: $(OBJ004)
	gcc -o $@ $^ $(INC) $(SAL) $(SOUNDLIB) $(OSLIBS) 

psg-test-005: $(OBJ005)
	gcc -o $@ $^ $(INC) $(SAL) $(SOUNDLIB) $(OSLIBS) 

vgs2sound.o: $(VGS2_HOME)/template/vgs2sound.c
	gcc -o vgs2sound.o $(CFLAG) $<

.SUFFIXES: .c .o

.c.o:
	gcc -o $@ $(CFLAG) $<

