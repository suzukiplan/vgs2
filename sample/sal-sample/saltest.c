#include <stdio.h>
#include "vgs2.h"

int main()
{
    char buf[1024];

    puts("Initialize sound-system");
    if(init_sound_cli()) {
        puts("Failed!");
        return -1;
    }

	puts("Enter to finish program.");
	while(fgets(buf,sizeof(buf),stdin)) {
		break;
	}

    puts("Terminate sound-system.");
    term_sound();
    return 0;
}

void sndbuf(char* buf,size_t size)
{
	static double r=0.0;
	short* ptr=(short*)buf; 
	int i;
	int size16=size/2;

	lock();
	for(i=0;i<size16;i++,ptr++) {
		*ptr=(short)(sin(r)*16384);
		r += M_PI*2 / (22050 / 440); /* 440Hz */  
	}
	unlock();
}

