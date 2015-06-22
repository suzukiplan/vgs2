/* VGS BGM player cli */
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <time.h>
#include "vgs2.h"

int bload_direct(unsigned char n,const char* name);
extern struct _PSG _psg;

/* include os depended header files */
#if defined(__linux)
#   include <alsa/asoundlib.h>
#elif defined(__APPLE__)
#   include <OpenAL/al.h>
#   include <OpenAL/alc.h>
#   define kOutputBus 0
#   define kInputBus 1
#   define BUFNUM 2
#   undef SAMPLE_BITS
#   define SAMPLE_BITS	AL_FORMAT_MONO16	/* redefine sampling bits */
static ALCdevice* sndDev;
static ALCcontext* sndCtx;
static ALuint sndABuf;
static ALuint sndASrc;
static char SBUF[SAMPLE_BUFS];
typedef ALvoid AL_APIENTRY (*alBufferDataStaticProcPtr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
static alBufferDataStaticProcPtr alBufferDataStaticProc;
#else
#   error "Not supported platform."
#endif

/* platform common data */
static unsigned short ADPAL[256];
static pthread_mutex_t LCKOBJ=PTHREAD_MUTEX_INITIALIZER;
static int REQ;
static int STALIVE=1;

/**
 * initialize sound system
 */
static int sound_init()
{
#if defined(__linux)
    return 0;
#elif defined(__APPLE__)
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
    alBufferDataStaticProc=(alBufferDataStaticProcPtr)alcGetProcAddress(NULL,(const ALCchar *)"alBufferDataStatic");
    return 0;
#endif
}

/**
 * sound thread procedure
 */
static void* sound_thread(void* args)
{
#if defined(__linux)
    int i;
    snd_pcm_t* snd;
    snd_pcm_hw_params_t* hwp;
    int rate=SAMPLE_RATE;
    int periods=3;
    short buf[SAMPLE_BUFS/2];
    snd_pcm_hw_params_alloca(&hwp);
    i=snd_pcm_open(&snd,"default",SND_PCM_STREAM_PLAYBACK,0);
    if(i<0) {
        fprintf(stderr,"snd_pcm_open error: %d\n",i);
        return NULL;
    }
    i=snd_pcm_hw_params_any(snd,hwp);
    if(i<0) {
        fprintf(stderr,"snd_pcm_hw_params_any error: %d\n",i);
        return NULL;
    }
    i=snd_pcm_hw_params_set_format(snd,hwp,SND_PCM_FORMAT_S16_LE);
    if(i<0) {
        fprintf(stderr,"snd_pcm_hw_params_set_format error: %d\n",i);
        return NULL;
    }
    i=snd_pcm_hw_params_set_rate_near(snd,hwp,&rate,0);
    if(i<0) {
        fprintf(stderr,"snd_pcm_hw_params_set_rate_near error: %d\n",i);
        return NULL;
    }
    if(rate!=SAMPLE_RATE) {
        fprintf(stderr,"This sound hawdware does not supports %dHz mode.\n",SAMPLE_RATE);
        return NULL;
    }
    i=snd_pcm_hw_params_set_channels(snd,hwp,SAMPLE_CH);
    if(i<0) {
        fprintf(stderr,"snd_pcm_hw_params_set_channels error: %d\n",i);
        return NULL;
    }
    i=snd_pcm_hw_params_set_periods(snd,hwp,periods,0);
    if(i<0) {
        fprintf(stderr,"snd_pcm_hw_params_set_periods error: %d\n",i);
        return NULL;
    }
    i=snd_pcm_hw_params_set_buffer_size(snd,hwp,(periods*sizeof(buf))/4);
    if(i<0) {
        fprintf(stderr,"snd_pcm_hw_params_set_buffer_size error: %d\n",i);
        return NULL;
    }
    i=snd_pcm_hw_params(snd,hwp);
    if(i<0) {
        fprintf(stderr,"snd_pcm_hw_params error: %d\n",i);
        return NULL;
    }
    while(STALIVE) {
        sndbuf((char*)buf,sizeof(buf));
        while(STALIVE) {
            i=snd_pcm_writei(snd,buf,sizeof(buf)/2);
            if(i<0) {
                snd_pcm_prepare(snd);
                puts("Buffer underrun");
            } else {
                break;
            }
        }
    }
    snd_pcm_drain(snd);
    snd_pcm_close(snd);
    return NULL;
#elif defined(__APPLE__)
    ALint st;
    alGenSources(1,&sndASrc);
    memset(SBUF,0,SAMPLE_BUFS);
    while(STALIVE) {
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
#endif
}

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
    pthread_t tid;
    struct sched_param param;
    char buf[1024];
    int isPlaying=1;
    int i,j;
    int st=0;
    char* cp;
    
    /* check argument */
    if(argc<2) {
        puts("usage: vgs2play bgm-file [mm:ss]");
        return 1;
    }
    if(3<=argc) {
        st=getsec(argv[2]);
    }
    
    /* intialize sound system */
    if(sound_init()) {
        fprintf(stderr,"Could not initialize the sound system.\n");
        return 2;
    }
    if(0!=pthread_create(&tid,NULL,sound_thread,NULL)) {
        fprintf(stderr,"Could not create the sound thread.\n");
        return 2;
    }
    memset(&param,0,sizeof(param));
    param.sched_priority = 46;
    pthread_setschedparam(tid,SCHED_OTHER,&param);
    
    /* load BGM data and play */
    if(bload_direct(0,argv[1])) {
        fprintf(stderr,"Load error.\n");
        return 2;
    }
    vgs2_bplay(0);
    if(st) vgs2_bjump(st);
    
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
    puts("- j{sec|mm:ss} : jump");
    puts("- q            : quit playing");
    puts("");
    
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
        } else if(buf[0]=='j') {
            vgs2_bjump(getsec(buf+1));
        } else if(buf[0]=='q') {
            break;
        }
        memset(buf,0,sizeof(buf));
        printf("command: ");
    }
    
    /* terminate procedure */
    STALIVE=0;
    pthread_join(tid,NULL);
#if defined(__APPLE__)
    alcDestroyContext(sndCtx);
    alcCloseDevice(sndDev);
#endif
    
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
