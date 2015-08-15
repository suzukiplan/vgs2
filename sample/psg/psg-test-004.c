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
    int wav;
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
                if(0==PSG.ch[0].env.st) continue;
                wav=(square(PSG.ch[0].bs.r/100) * 8192 / 256);
                wav*=PSG.ch[0].env.pw/256;
                wav/=256;
                *ptr+=(short)wav;
                PSG.ch[0].bs.r += SCALE[PSG.ch[0].bs.key];
                switch(PSG.ch[0].env.st) {
                    case 1:
                        PSG.ch[0].env.pw+=PSG.ch[0].env.a;
                        if(65535<=PSG.ch[0].env.pw) {
                            PSG.ch[0].env.pw=65535;
                            PSG.ch[0].env.et=PSG.ch[0].env.dt;
                            PSG.ch[0].env.a=65535;
							PSG.ch[0].env.a-=PSG.ch[0].env.sl<<8;
							PSG.ch[0].env.a/=PSG.ch[0].env.dt;
							PSG.ch[0].env.a=-PSG.ch[0].env.a;
                            PSG.ch[0].env.st++;
                        }
                        break;
                    case 2:
                        PSG.ch[0].env.pw+=PSG.ch[0].env.a;
                        PSG.ch[0].env.et--;
                        if(0==PSG.ch[0].env.et || PSG.ch[0].env.pw<0) {
                            if(PSG.ch[0].env.pw<0) PSG.ch[0].env.pw=0;
                            PSG.ch[0].env.st++;
                        }
                        break;
                    case 4:
                        if(-PSG.ch[0].env.a<PSG.ch[0].env.pw) {
                            PSG.ch[0].env.pw+=PSG.ch[0].env.a;
                        } else {
                            PSG.ch[0].env.pw=0;
                            PSG.ch[0].env.st=0;
                        }
                        break;
                }
            }
        }
    }
    unlock();
}
