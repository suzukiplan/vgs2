all: ../bin\
	../bin/vgs2bmp\
	../bin/vgs2mkpj\
	../bin/vgs2mml\
	../bin/vgs2rom\
	../bin/vgs2wav

../bin:
	mkdir ../bin

../bin/vgs2bmp: vgs2bmp.c
	gcc -o ../bin/vgs2bmp vgs2bmp.c

../bin/vgs2mkpj: vgs2mkpj.c
	gcc -ansi -std=c90 -o ../bin/vgs2mkpj vgs2mkpj.c

../bin/vgs2mml: vgs2mml.c
	gcc -o ../bin/vgs2mml vgs2mml.c

../bin/vgs2rom: vgs2rom.c
	gcc -o ../bin/vgs2rom vgs2rom.c

../bin/vgs2wav: vgs2wav.c
	gcc -o ../bin/vgs2wav vgs2wav.c

