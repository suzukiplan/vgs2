#include <stdio.h>
#include "vgs2.h"
#include "psg.h"

struct PsgContext PSG;
extern struct Operation waon[];

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
    PSG.op=waon;
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
    int n;
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
                for(n=0;n<MAX_CHANNEL;n++) {
                    if(0==PSG.ch[n].env.st) continue;
                    wav=(square(PSG.ch[n].bs.r/100) * 8192 / 256);
                    wav*=PSG.ch[n].env.pw/256;
                    wav/=256;
                    wav+=*ptr;
                    if(32727<wav) wav=32727;
                    else if(wav<-32768) wav=-32768;
                    *ptr=(short)wav;
                    PSG.ch[n].bs.r += SCALE[PSG.ch[n].bs.key];
                    switch(PSG.ch[n].env.st) {
                        case 1:
                            PSG.ch[n].env.pw+=PSG.ch[n].env.a;
                            if(65535<=PSG.ch[n].env.pw) {
                                PSG.ch[n].env.pw=65535;
                                PSG.ch[n].env.et=PSG.ch[n].env.dt;
                                PSG.ch[n].env.a=65535;
                                PSG.ch[n].env.a-=PSG.ch[n].env.sl<<8;
                                PSG.ch[n].env.a/=PSG.ch[n].env.dt;
                                PSG.ch[n].env.a=-PSG.ch[n].env.a;
                                PSG.ch[n].env.st++;
                            }
                            break;
                        case 2:
                            PSG.ch[n].env.pw+=PSG.ch[n].env.a;
                            PSG.ch[n].env.et--;
                            if(0==PSG.ch[n].env.et || PSG.ch[n].env.pw<0) {
                                if(PSG.ch[n].env.pw<0) PSG.ch[n].env.pw=0;
                                PSG.ch[n].env.st++;
                            }
                            break;
                        case 4:
                            if(-PSG.ch[n].env.a<PSG.ch[n].env.pw) {
                                PSG.ch[n].env.pw+=PSG.ch[n].env.a;
                            } else {
                                PSG.ch[n].env.pw=0;
                                PSG.ch[n].env.st=0;
                            }
                            break;
                    }
                }
            }
        }
    }
    unlock();
}
