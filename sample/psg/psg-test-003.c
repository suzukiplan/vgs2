#include <stdio.h>
#include "vgs2.h"
#include "psg.h"

struct PsgContext PSG;
extern struct Operation kaeru[];

int main()
{
    char buf[1024];
    
    puts("Initialize sound-system");
    if(init_sound_cli()) {
        puts("Failed!");
        return -1;
    }
    make_sin_table();
    make_scale_table();
    lock();
    memset(&PSG,0,sizeof(PSG));
    PSG.op=kaeru;
    unlock();
    
    fgets(buf,sizeof(buf),stdin);
    
    puts("Terminate sound-system.");
    term_sound();
    return 0;
}


void sndbuf(char* buf,size_t size)
{
    short* ptr=(short*)buf;
    int size16=(int)size/2;
    int hz=0;
    memset(buf,0,size);
    lock();
    if(PSG.op) {
        while(hz<size16) {
            if(0==PSG.wt) {
                if(0==apc_operation(&PSG)) {
                    break;
                }
            }
            for(;hz<size16 && PSG.wt;hz++,ptr++,PSG.wt--) {
                if(PSG.ch[0].bs.on) {
                    *ptr+=(short)(square(PSG.ch[0].bs.r/100) * 8192 / 256);
                    PSG.ch[0].bs.r += SCALE[PSG.ch[0].bs.key];
                }
            }
        }
    }
    unlock();
}
