#include <stdio.h>
#include <math.h>
#include "vgs2.h"

#define PI2 6.2831853071795864

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
        r += PI2 / (22050 / 440); /* 440Hz */
    }
    unlock();
}

