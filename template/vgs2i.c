/* (C)2014, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: GameDaddy - emulator
 *    Platform: iOS
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *        Date: 15-Mar-2014
 * FileVersion: 1.01
 *----------------------------------------------------------------------------
 */
#include <pthread.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <CoreAudio/CoreAudioTypes.h>
#include <AudioUnit/AudioUnit.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "vgs2.h"

#define kOutputBus 0
#define kInputBus 1
#define BUFNUM 2

#undef SAMPLE_BITS
#define SAMPLE_BITS	AL_FORMAT_MONO16	/* redefine sampling bits */

/*
 *-----------------------------------------------------------------------------
 * Variables
 *-----------------------------------------------------------------------------
 */
static pthread_mutex_t sndLock;
static pthread_t sndTid;
static ALCdevice* sndDev;
static ALCcontext* sndCtx;
static ALuint sndABuf;
static ALuint sndASrc;
static char SBUF[SAMPLE_BUFS];
unsigned short ADPAL[256];
static char pathbuf[4096];
int isIphone5;

/*
 *-----------------------------------------------------------------------------
 * Functions
 *-----------------------------------------------------------------------------
 */
static int init_openAL();
static void* sound_thread(void* args);
typedef ALvoid AL_APIENTRY (*alBufferDataStaticProcPtr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
static alBufferDataStaticProcPtr alBufferDataStaticProc;

/*
 *-----------------------------------------------------------------------------
 * Initialize procedure of VGS
 *-----------------------------------------------------------------------------
 */
int vgsint_init(const char* rompath)
{
    char log[1024];
    static char* rom=NULL;
    char* bin;
    FILE* fp;
    int fsize;
    int i,j,size;
    int cn,pn,bn;
    unsigned char s[4];
    char path[80];
    struct stat stbuf;
    struct sched_param param;

    /* initialize mutex */
    pthread_mutex_init(&sndLock,NULL);

    /* get rom file size */
    stat(rompath,&stbuf);
    fsize=(int)stbuf.st_size;

    /* open */
    fp=fopen(rompath,"rb");
    if(NULL==fp) {
        return -1;
    }

    /* allocate buffer */
    rom=(char*)malloc(fsize);
    if(NULL==rom) {
        fclose(fp);
        return -1;
    }

    /* read */
    fread(rom,fsize,1,fp);
    fclose(fp);
    bin=rom;

    /* get the records number */
    memcpy(&BN,bin,4);
    bin+=4;

    /* get records buffer */
    BR=(struct _BINREC*)malloc(BN*sizeof(struct _BINREC));
    if(NULL==BR) {
        return -1;
    }
    memset(BR,0,BN*sizeof(struct _BINREC));

	/* set record name */
	for(i=0;i<BN;i++) {
		memset(BR[i].name,0,16);
		memcpy(BR[i].name,bin,16);
		bin+=16;
		for(j=0;'\0'!=BR[i].name[j];j++) {
			if(15==j) {
				return -1;
			}
			BR[i].name[j]^=0xAA;
		}
	}

	/* set record buffer */
	for(i=0;i<BN;i++) {
		memcpy(s,bin,4);
		bin+=4;
		size=s[0];
		size<<=8;
		size|=s[1];
		size<<=8;
		size|=s[2];
		size<<=8;
		size|=s[3];
		BR[i].size=size;
		BR[i].data=(char*)malloc(BR[i].size);
		if(NULL==BR[i].data) {
			return -1;
		}
		memcpy(BR[i].data,bin,BR[i].size);
		bin+=BR[i].size;
	}

	/* load to slot */
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
	sprintf(log,"Data has extracted. (CHR=%d, PCM=%d, BGM=%d)",cn,pn,bn);
    puts(log);

	/* initialize UP */
	vgs2_pallet(7);
	if(0!=vgs2_init()) {
		return -1;
	}

	/* create 16bit color palette */
	make_pallet();

	/* initialize OpenAL */
	if(0!=init_openAL()) {
		return -1;
	}

	/* start sound ctrl thread */
	pthread_create(&sndTid, NULL, sound_thread, NULL);
	memset(&param,0,sizeof(param));
	param.sched_priority = 46;
	pthread_setschedparam(sndTid,SCHED_OTHER,&param);
	return 0;
}

/*
 *-----------------------------------------------------------------------------
 * Initialize OpenAL
 *-----------------------------------------------------------------------------
 */
static int init_openAL()
{
    sndDev=alcOpenDevice(NULL);
    if(NULL==sndDev) {
        return -1;
    }
    sndCtx=alcCreateContext(sndDev, NULL);
    if(NULL==sndCtx) {
        return -1;
    }
    if(!alcMakeContextCurrent(sndCtx)) {
        return -1;
    }
    alBufferDataStaticProc=(alBufferDataStaticProcPtr)alcGetProcAddress(nil,(const ALCchar *)"alBufferDataStatic");
    return 0;
}

/*
 *-----------------------------------------------------------------------------
 * sound ctrl thread procedure
 *-----------------------------------------------------------------------------
 */
static void* sound_thread(void* args)
{
    ALint st;
    alGenSources(1,&sndASrc);
    memset(SBUF,0,SAMPLE_BUFS);
    while(1) {
        alGetSourcei(sndASrc, AL_BUFFERS_QUEUED, &st);
        if (st < BUFNUM) {
            alGenBuffers(1, &sndABuf);
        } else {
            alGetSourcei(sndASrc, AL_SOURCE_STATE, &st);
            if (st != AL_PLAYING) {
                alSourcePlay(sndASrc);
            }
            while (alGetSourcei(sndASrc, AL_BUFFERS_PROCESSED, &st), st == 0) {
                usleep(1000);
            }
            alSourceUnqueueBuffers(sndASrc, 1, &sndABuf);
            alDeleteBuffers(1,&sndABuf);
            alGenBuffers(1, &sndABuf);
        }
        sndbuf(SBUF,SAMPLE_BUFS);
        alBufferData(sndABuf,SAMPLE_BITS,SBUF,SAMPLE_BUFS,SAMPLE_RATE);
        alSourceQueueBuffers(sndASrc, 1, &sndABuf);
    }
    return NULL;
}


/*
 *-----------------------------------------------------------------------------
 * inter thread lock
 *-----------------------------------------------------------------------------
 */
void lock()
{
    pthread_mutex_lock(&sndLock);
}

/*
 *-----------------------------------------------------------------------------
 * unlock inter thread lock
 *-----------------------------------------------------------------------------
 */
void unlock()
{
    pthread_mutex_unlock(&sndLock);
}

/*
 *----------------------------------------------------------------------------
 * make 16bit pallet table (24bit -> 15bit/5:5:5)
 *----------------------------------------------------------------------------
 */
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
		g>>=3;
		b&=0xF8;
		b>>=3;
		ADPAL[i]=r;
		ADPAL[i]<<=5;
		ADPAL[i]|=g;
		ADPAL[i]<<=5;
		ADPAL[i]|=b;
	}
	ADPAL[0]=0x5;
}

/*
 *-----------------------------------------------------------------------------
 * set data directory
 *-----------------------------------------------------------------------------
 */
void vgsint_setdir(const char* dir)
{
    strcpy(pathbuf,dir);
    strcat(pathbuf,"/");
}

/*
 *-----------------------------------------------------------------------------
 * fopen
 *-----------------------------------------------------------------------------
 */
FILE* vgs2_fopen(const char* fname,const char* mode)
{
    char path[4096];
    strcpy(path,pathbuf);
    strcat(path,fname);
    return fopen(path,mode);
}

/*
 *-----------------------------------------------------------------------------
 * delete ads (*no operation)
 *-----------------------------------------------------------------------------
 */
void vgs2_deleteAds()
{
    ;
}

/*
 *-----------------------------------------------------------------------------
 * show ads (*no operation)
 *-----------------------------------------------------------------------------
 */
void vgs2_showAds()
{
    ;
}
