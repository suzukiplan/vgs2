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
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include "vgs2.h"

#define kOutputBus 0
#define kInputBus 1
#define BUFNUM 2
#undef SAMPLE_BITS
#define SAMPLE_BITS	AL_FORMAT_MONO16	/* redefine sampling bits */

static int STALIVE=1;
static pthread_t TID;
static ALCdevice* sndDev;
static ALCcontext* sndCtx;
static ALuint sndABuf;
static ALuint sndASrc;
static char SBUF[SAMPLE_BUFS];
typedef ALvoid AL_APIENTRY (*alBufferDataStaticProcPtr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
static alBufferDataStaticProcPtr alBufferDataStaticProc;

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
    alcDestroyContext(sndCtx);
    alcCloseDevice(sndDev);
    sndCtx=NULL;
    sndDev=NULL;
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
}
