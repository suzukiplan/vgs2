#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include "vgs2.h"

static unsigned short ADPAL[256];
static pthread_mutex_t LCKOBJ=PTHREAD_MUTEX_INITIALIZER;
static int REQ;

int main(int argc,char* argv[])
{
	SDL_Surface* surface;
	SDL_Event event;
	FILE* fp;
	int i,j;
	char s[4];
	char path[16];
	int cn,pn,bn;

	puts("Start VGS mk-II SR for Linux.");

	/* Load ROMDATA.BIN */
	fp=fopen("ROMDATA.BIN","rb");
	if(NULL==fp) {
		fprintf(stderr,"ROMDATA.BIN was not found.\n");
		return 1;
	}

	/* allocate table */
	BN=0;
	BR=NULL;
	fread(&BN,sizeof(BN),1,fp);
	if(BN<1) {
		fprintf(stderr,"Inalid ROMDATA.BIN: record number\n");
		fclose(fp);
		return 1;
	}
	if(NULL==(BR=(struct _BINREC*)malloc(BN * sizeof(struct _BINREC)))) {
		fprintf(stderr,"No memory.\n");
		fclose(fp);
		return 1;
	}
	memset(BR,0,BN * sizeof(struct _BINREC));

	/* read filename */
	for(i=0;i<BN;i++) {
		memset(BR[i].name,0,16);
		fread(&BR[i].name,16,1,fp);
		for(j=0;'\0'!=BR[i].name[j];j++) {
			if(15==j) {
				fprintf(stderr,"ROM data is broken.\n");
				fclose(fp);
				return 1;
			}
			BR[i].name[j]^=0xAA;
		}
	}

	/* read data */
	for(i=0;i<BN;i++) {
		fread(s,4,1,fp);
		BR[i].size=s[0];
		BR[i].size<<=8;
		BR[i].size|=s[1];
		BR[i].size<<=8;
		BR[i].size|=s[2];
		BR[i].size<<=8;
		BR[i].size|=s[3];
		if(NULL==(BR[i].data=(char*)malloc(BR[i].size))) {
			fprintf(stderr,"Could not allocate ROM data area: errno=%d\n",errno);
			fclose(fp);
			return 1;
		}
		fread(BR[i].data,BR[i].size,1,fp);
	}

	fclose(fp);

	/* Extract data */
	cn=0;
	pn=0;
	bn=0;
	for(i=0;i<256;i++) {
		sprintf(path,"GSLOT%03d.CHR",i);
		if(0==gload(i,path)) cn++;
		sprintf(path,"ESLOT%03d.PCM",i);
		if(0==eload(i,path)) pn++;
		sprintf(path,"BSLOT%03d.BGM",i);
		if(0==bload(i,path)) bn++;
	}

	printf("Extracted ROM data: CHR=%d PCM=%d BGM=%d\n",cn,pn,bn);

	/* Initialize SDL */
	if(SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr,"Failed: SDL_Init(SDL_INIT_VIDEO)\n");
		return 1;
	}
	SDL_WM_SetCaption("VGS mk-II SR for Linux",NULL);

	/* Initialize Surface */
	surface=SDL_SetVideoMode(XSIZE*2,YSIZE*2,16,SDL_SWSURFACE);
	if(NULL==surface) {
		fprintf(stderr,"Could not create the surface.\n");
		return 1;
	}

	/* Initialize user program */
	if(vgs2_init()) {
		fprintf(stderr,"Initialization of the app has failed.\n");
		return 2;
	}

	do {
		SDL_PollEvent(&event);
	} while(event.type!=SDL_QUIT);
	SDL_Quit();
	return 0;
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


