/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - emulator / sound system
 *    Platform: iOS and Mac OS X
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#if defined(__linux__)
#   include <alsa/asoundlib.h>
#elif defined(__APPLE__)
#   include <OpenAL/al.h>
#   include <OpenAL/alc.h>
#else
#   error "unsupported platform"
#endif
#include "vgs2.h"

#ifdef __APPLE__
#define kOutputBus 0
#define kInputBus 1
#define BUFNUM 2
#undef SAMPLE_BITS
#define SAMPLE_BITS	AL_FORMAT_MONO16	/* redefine sampling bits */
#endif

static int STALIVE=1;
static pthread_t TID;

#ifdef __APPLE__
static ALCdevice* sndDev;
static ALCcontext* sndCtx;
static ALuint sndABuf;
static ALuint sndASrc;
static char SBUF[SAMPLE_BUFS];
typedef ALvoid AL_APIENTRY (*alBufferDataStaticProcPtr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
static alBufferDataStaticProcPtr alBufferDataStaticProc;
#endif

static void* sound_thread(void* args);

/*
 *-----------------------------------------------------------------------------
 * Initialize sound system
 *-----------------------------------------------------------------------------
 */
int init_sound()
{
    struct sched_param param;
    int i;

#ifdef __APPLE__
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
#endif

    i=pthread_create(&TID,NULL,sound_thread,NULL);
    if(0!=i) {
        fprintf(stderr,"Could not create the sound thread.\n");
        return 1;
    }
    memset(&param,0,sizeof(param));
    param.sched_priority = 46;
    pthread_setschedparam(TID,SCHED_OTHER,&param);
    
    return 0;
}

/*
 *-----------------------------------------------------------------------------
 * terminate sound system
 *-----------------------------------------------------------------------------
 */
void term_sound()
{
    STALIVE=0;
    pthread_join(TID,NULL);
#ifdef __APPLE__
    alcDestroyContext(sndCtx);
    alcCloseDevice(sndDev);
    sndCtx=NULL;
    sndDev=NULL;
#endif
}

/*
 *-----------------------------------------------------------------------------
 * sound ctrl thread procedure
 *-----------------------------------------------------------------------------
 */
static void* sound_thread(void* args)
{
#if defined(__linux__)
    //=========================================================================
    // for Linux
    //=========================================================================
    int i;
    snd_pcm_t* snd;
    snd_pcm_hw_params_t* hwp;
    int rate=SAMPLE_RATE;
    int periods=3;
    short buf[SAMPLE_BUFS/2];
    puts("Started sound thread.");
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
    puts("Sound thread now be ended.");
    snd_pcm_drain(snd);
    snd_pcm_close(snd);
#elif defined(__APPLE__)
    //=========================================================================
    // for iOS and Mac OS X
    //=========================================================================
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
#endif
    return NULL;
}
