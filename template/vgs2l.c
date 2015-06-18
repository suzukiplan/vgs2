#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timeb.h>
#include <stdio.h>
#include "vgs2.h"

static unsigned short ADPAL[256];
static pthread_mutex_t LCKOBJ=PTHREAD_MUTEX_INITIALIZER;
static int REQ;

int main(int argc,char* argv[])
{
	puts("DUMMY");
}

void putlog(const char* fn,int ln,const char* msg,...)
{
        va_list args;
        time_t t1;
        struct tm* t2;
        struct timeb tb;
        char buf[1024];

        ftime(&tb);
        t1=tb.time;
        t2=localtime(&t1);
        sprintf(buf,"%04d/%02d/%02d %02d:%02d:%02d.%03d "
                ,t2->tm_year+1900
                ,t2->tm_mon+1
                ,t2->tm_mday
                ,t2->tm_hour
                ,t2->tm_min
                ,t2->tm_sec
                ,tb.millitm
                );
        va_start(args,msg);
        vsprintf(buf+strlen(buf),msg,args);
        va_end(args);
        fprintf(stderr,"%s",buf);
}

void lock()
{
	pthread_mutex_lock(&LCKOBJ);
}

void unlock()
{
	pthread_mutex_unlock(&LCKOBJ);
}

void make_pallet()
{
        int i,j;
        int r,g,b;
        for(j=0,i=0;i<256;i++) {
                r=(_PAL[i]&0x00FF0000)>>16;
                g=(_PAL[i]&0x0000FF00)>>8;
                b=(_PAL[i]&0x000000FF);
                r&=0xF8;
                r>>=3;
                g&=0xFC;
                g>>=2;
                b&=0xF8;
                b>>=3;
                ADPAL[i]=r;
                ADPAL[i]<<=6;
                ADPAL[i]|=g;
                ADPAL[i]<<=5;
                ADPAL[i]|=b;
        }
        ADPAL[0]=0x1865;
}

FILE* vgs2_fopen(const char* n,const char* p)
{
	return fopen(n,p);
}

void vgs2_showAds()
{
        REQ=1;
}

void vgs2_deleteAds()
{
        REQ=2;
}


