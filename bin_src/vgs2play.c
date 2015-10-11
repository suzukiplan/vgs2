/* VGS BGM player cli */
#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timeb.h>
#endif
#include <stdio.h>
#include <time.h>
#include "vgs2.h"

int bload_direct(unsigned char n,const char* name);
void bfree_direct(unsigned char n);
extern struct _PSG _psg;

/**
 * get secound from text
 */
int getsec(const char* text)
{
    char* cp;
    int ret=0;
    cp=strchr(text,':');
    if(NULL==cp) {
        ret=atoi(text);
    } else {
        ret=atoi(text)*60;
        ret+=atoi(cp+1);
    }
    return ret;
}

/**
 * main procedure
 */
int main(int argc,char* argv[])
{
    char buf[1024];
    int isPlaying=1;
    int i,j;
    int st=0;
    char* cp;
    int show=0;
    
    /* check argument */
    if(argc<2) {
        puts("usage: vgs2play bgm-file [mm:ss]");
        return 1;
    }
    if(3<=argc) {
        st=getsec(argv[2]);
    }
    
    /* intialize sound system */
    if(init_sound_cli()) {
        fprintf(stderr,"Could not initialize the sound system.\n");
        return 2;
    }

RELOAD:
    /* load BGM data and play */
    if(bload_direct(0,argv[1])) {
        fprintf(stderr,"Load error.\n");
        return 2;
    }
    vgs2_bplay(0);
    if(st) vgs2_bjump(st);

    if(!show) {
        /* show song info */
        puts("Song info:");
        printf("- number of notes = %d\n",_psg.idxnum);
        if(_psg.timeI) {
            printf("- intro time = %02u:%02u\n",_psg.timeI/22050/60, _psg.timeI/22050%60);
        } else {
            printf("- acyclic song\n");
        }
        printf("- play time = %02u:%02u\n",_psg.timeL/22050/60, _psg.timeL/22050%60);
        puts("");
        
        /* show reference */
        puts("Command Reference:");
        puts("- p            : pause / resume");
        puts("- v rate       : master volume");
        puts("- c ch rate    : channel volume");
        puts("- j{sec|mm:ss} : jump");
        puts("- k{+up|-down} : key change");
        puts("- m[ch]...     : mute channel");
        puts("- f            : fadeout");
        puts("- r            : reload");
        puts("- q            : quit playing");
        puts("");
        show=1;
    }
    
    /* main loop */
    memset(buf,0,sizeof(buf));
    printf("command: ");
    while(NULL!=fgets(buf,sizeof(buf)-1,stdin)) {
        for(i=0;buf[i];i++) {
            if('A'<=buf[i] && buf[i]<='Z') {
                buf[i]-='a'-'A';
            }
            if(' '==buf[i] || '\t'==buf[i]) {
                for(j=i;;j++) {
                    buf[j]=buf[j+1];
                    if(!buf[j]) break;
                }
            }
        }
        if(buf[0]=='p') {
            if(isPlaying) {
                vgs2_bstop();
                isPlaying=0;
            } else {
                vgs2_bresume();
                isPlaying=1;
            }
        } else if(buf[0]=='v') {
            vgs2_bmvol(atoi(buf+1));
        } else if(buf[0]=='c') {
            if(3<=strlen(buf)) {
                vgs2_bcvol((int)buf[1]-'0',atoi(buf+2));
            }
        } else if(buf[0]=='j') {
            vgs2_bjump(getsec(buf+1));
        } else if(buf[0]=='k') {
            if(buf[1]=='+') {
                vgs2_bkey(atoi(&buf[2]));
            } else if(buf[1]=='-') {
                vgs2_bkey(0-atoi(&buf[2]));
            }
        } else if(buf[0]=='m') {
            for(i=1;buf[i];i++) {
                if('0'<=buf[i] && buf[i]<='5') {
                    vgs2_bmute((int)(buf[i]-'0'));
                }
            }
        } else if(buf[0]=='r') {
            bfree_direct(0);
            goto RELOAD;
        } else if(buf[0]=='f') {
            vgs2_bfade2();
        } else if(buf[0]=='q') {
            break;
        }
        memset(buf,0,sizeof(buf));
        printf("command: ");
    }
    
    /* terminate procedure */
    term_sound();
    bfree_direct(0);
    
    return 0;
}

void putlog(const char* fn,int ln,const char* msg,...)
{
}

void make_pallet()
{
}

FILE* vgs2_fopen(const char* n,const char* p)
{
    return fopen(n,p);
}

void vgs2_showAds()
{
}

void vgs2_deleteAds()
{
}
