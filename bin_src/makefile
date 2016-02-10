all:
	make all-`uname`

all-Linux:
	make allcom SOUNDLIB=-lasound

all-Darwin:
	make allcom SOUNDLIB="-framework OpenAL"

allcom: ../bin\
	../bin/vgs2play\
	../bin/vgs2bmp\
	../bin/vgs2mkpj\
	../bin/vgs2mml\
	../bin/vgs2rom\
	../bin/vgs2wav\
	../bin/vgs2tar\
	../bin/vgs2pack\
	../bin/vgs2play

../bin:
	mkdir ../bin

../bin/vgs2bmp: vgs2bmp.c
	gcc -o ../bin/vgs2bmp vgs2bmp.c

../bin/vgs2mkpj: vgs2mkpj.c
	gcc -ansi -std=c89 -o ../bin/vgs2mkpj vgs2mkpj.c

../bin/vgs2mml: vgs2mml.c vgsmml.c
	gcc -o ../bin/vgs2mml -I../vgs-mml-compiler/src -I../vgs-bgm-decoder/src vgs2mml.c vgsmml.c 

vgs2mml.c:
	ln -s ../vgs-mml-compiler/src/cli.c vgs2mml.c

vgsmml.c:
	ln -s ../vgs-mml-compiler/src/vgsmml.c vgsmml.c

../vgs-mml-compiler/vgs2mml:
	cd ../vgs-mml-compiler && make

../bin/vgs2rom: vgs2rom.c
	gcc -o ../bin/vgs2rom vgs2rom.c

../bin/vgs2wav: vgs2wav.c
	gcc -o ../bin/vgs2wav vgs2wav.c

../bin/vgs2tar: vgs2tar.c
	gcc -o ../bin/vgs2tar vgs2tar.c

../bin/vgs2pack: vgs2pack.c vgs2pack.h ../template/vgs2api.c ../template/vgs2.h
	gcc -o ../bin/vgs2pack -I../template vgs2pack.c ../template/vgs2sound.c ../template/vgs2api.c ../template/vgs2tone.c $(SOUNDLIB) -lpthread

../bin/vgs2play: vgs2play.c ../template/vgs2sound.c ../template/vgs2api.c ../vgs-bgm-decoder/src/vgs2tone.c ../vgs-bgm-decoder/src/vgsdec.c ../vgs-mml-compiler/src/vgsmml.c ../template/vgs2.h
	gcc -o ../bin/vgs2play -I../template -I../vgs-bgm-decoder/src -I../vgs-mml-compiler/src vgs2play.c ../template/vgs2sound.c ../template/vgs2api.c ../vgs-bgm-decoder/src/vgs2tone.c $(SOUNDLIB) -lpthread

