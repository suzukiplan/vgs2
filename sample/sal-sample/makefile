all:
	make all-`uname`

all-Linux:
	make build SOUNDLIB=-lasound

all-Darwin:
	make build SOUNDLIB="-framework OpenAL"

build:
	gcc -o saltest \
	-I$(VGS2_HOME)/template \
	saltest.c \
	$(VGS2_HOME)/template/vgs2sound.c \
	$(SOUNDLIB) \
	-lpthread
