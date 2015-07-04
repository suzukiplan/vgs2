/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - sound system abstraction layer for UNIX
 *    Platform: Android, Linux, iOS and Mac OS X
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#if defined(__ANDROID__)
#   include <SLES/OpenSLES.h>
#   include <SLES/OpenSLES_Android.h>
#elif defined(__linux__)
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

static pthread_mutex_t LCKOBJ=PTHREAD_MUTEX_INITIALIZER;

#ifdef __ANDROID__
static pthread_mutex_t LCKOBJ2=PTHREAD_MUTEX_INITIALIZER;
static SLObjectItf g_slEngObj=NULL;
static SLEngineItf g_slEng;
static SLObjectItf g_slMixObj=NULL;
static SLEnvironmentalReverbItf g_slRev;
static const SLEnvironmentalReverbSettings g_slRevSet=SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;
static SLObjectItf g_slPlayObj=NULL;
static SLPlayItf g_slPlay;
static SLAndroidSimpleBufferQueueItf g_slBufQ;
static short g_sndBuf[SAMPLE_BUFS/2];
#else
static int STALIVE=1;
static pthread_t TID;
#endif

#ifdef __APPLE__
static ALCdevice* sndDev;
static ALCcontext* sndCtx;
static ALuint sndABuf;
static ALuint sndASrc;
static char SBUF[SAMPLE_BUFS];
typedef ALvoid AL_APIENTRY (*alBufferDataStaticProcPtr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
static alBufferDataStaticProcPtr alBufferDataStaticProc;
#endif

#ifndef __ANDROID__
static void* sound_thread(void* args);
#endif

/*
 *-----------------------------------------------------------------------------
 * lock / unlock
 *-----------------------------------------------------------------------------
 */
void lock()
{
    pthread_mutex_lock(&LCKOBJ);
}

void unlock()
{
    pthread_mutex_unlock(&LCKOBJ);
}

#ifdef __ANDROID__
static void lock2()
{
    pthread_mutex_lock(&LCKOBJ2);
}

static void unlock2()
{
    pthread_mutex_unlock(&LCKOBJ2);
}
#endif

/*
 *-----------------------------------------------------------------------------
 * initialize sound for CLI
 *-----------------------------------------------------------------------------
 */
int init_sound_cli()
{
    return init_sound();
}

#ifdef __ANDROID__
/*
 *-----------------------------------------------------------------------------
 * Initialize sound system of the OpenSL/ES(Android)
 *-----------------------------------------------------------------------------
 */

/* buffering (callback) */
static void cbSound(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    lock2();
    if(g_slBufQ) {
        sndbuf((char*)g_sndBuf,sizeof(g_sndBuf));
        (*g_slBufQ)->Enqueue(g_slBufQ,g_sndBuf,sizeof(g_sndBuf));
    }
    unlock2();
}

/* initialize OpenSL/ES phase 2 */
static int init_soundPh2()
{
    SLresult res;
    SLDataFormat_PCM format_pcm = {
        SL_DATAFORMAT_PCM
        ,1								/* 1ch */
        ,SL_SAMPLINGRATE_22_05			/* 22050Hz */
        ,SL_PCMSAMPLEFORMAT_FIXED_16	/* 16bit */
        ,SL_PCMSAMPLEFORMAT_FIXED_16	/* 16bit */
        ,SL_SPEAKER_FRONT_CENTER		/* center */
        ,SL_BYTEORDER_LITTLEENDIAN		/* little-endian */
    };
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {
        SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE
        ,2
    };
    SLDataSource aSrc = {&loc_bufq, &format_pcm};
    SLDataLocator_OutputMix loc_outmix={SL_DATALOCATOR_OUTPUTMIX,g_slMixObj};
    SLDataSink aSnk = {&loc_outmix, NULL};
    const SLInterfaceID ids[2] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_EFFECTSEND};
    const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    
    res=(*g_slEng)->CreateAudioPlayer(g_slEng,&g_slPlayObj,&aSrc,&aSnk,2,ids,req);
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"CreateAudioPlayer error: result=%d",(int)res);
        return -1;
    }
    
    res=(*g_slPlayObj)->Realize(g_slPlayObj,SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"Realize(Player) error: result=%d",(int)res);
        return -1;
    }
    
    res=(*g_slPlayObj)->GetInterface(g_slPlayObj,SL_IID_PLAY,&g_slPlay);
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"GetInterface(Player) error: result=%d",(int)res);
        return -1;
    }
    
    res=(*g_slPlayObj)->GetInterface(g_slPlayObj,SL_IID_ANDROIDSIMPLEBUFFERQUEUE,&g_slBufQ);
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"GetInterface(BufQ) error: result=%d",(int)res);
        return -1;
    }
    
    res=(*g_slBufQ)->RegisterCallback(g_slBufQ,cbSound,NULL);
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"RegisterCallback error: result=%d",(int)res);
        return -1;
    }
    
    res=(*g_slPlay)->SetPlayState(g_slPlay,SL_PLAYSTATE_PLAYING);
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"SetPlayState error: result=%d",(int)res);
        return -1;
    }
    
    memset(g_sndBuf,0,sizeof(g_sndBuf));
    res=(*g_slBufQ)->Enqueue(g_slBufQ,g_sndBuf,sizeof(g_sndBuf));
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"Enqueue(1st) error: result=%d",(int)res);
        return -1;
    }
    return 0;
}

/* initialize OpenSL/ES phase 1 */
int init_sound()
{
    SLresult res;
    const SLInterfaceID ids[1]={SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1]={SL_BOOLEAN_FALSE};
    
    res=slCreateEngine(&g_slEngObj,0,NULL,0,NULL,NULL);
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"slCreateEngine error: result=%d",(int)res);
        return -1;
    }
    
    res=(*g_slEngObj)->Realize(g_slEngObj,SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"Realize(Engine) error: result=%d",(int)res);
        return -1;
    }
    
    res=(*g_slEngObj)->GetInterface(g_slEngObj,SL_IID_ENGINE,&g_slEng);
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"GetInterface(Engine) error: result=%d",(int)res);
        return -1;
    }
    
    res=(*g_slEng)->CreateOutputMix(g_slEng,&g_slMixObj,1,ids,req);
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"CreateOutputMix error: result=%d",(int)res);
        return -1;
    }
    
    res=(*g_slMixObj)->Realize(g_slMixObj,SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"Realize(MixObj) error: result=%d",(int)res);
        return -1;
    }
    
    res=(*g_slMixObj)->GetInterface(g_slMixObj,SL_IID_ENVIRONMENTALREVERB,&g_slRev);
    if(SL_RESULT_SUCCESS!=res) {
        putlog(__FILE__,__LINE__,"GetInterface(Reverb) error: result=%d (continue)",(int)res);
    } else {
        res=(*g_slRev)->SetEnvironmentalReverbProperties(g_slRev,&g_slRevSet);
        if(SL_RESULT_SUCCESS!=res) {
            putlog(__FILE__,__LINE__,"SetEnvironmentalReverbProperties(default) error: result=%d (continue)",(int)res);
        }
    }
    
    if(init_soundPh2()) {
        return -1;
    }
    putlog(__FILE__,__LINE__,"GameDaddy sound system has initialized.");
    return 0;
}

#else
/*
 *-----------------------------------------------------------------------------
 * Initialize sound system of the ALSA(Linux) or OpenAL(iOS and Mac OS X)
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
#endif

/*
 *-----------------------------------------------------------------------------
 * terminate sound system
 *-----------------------------------------------------------------------------
 */
void term_sound()
{
#ifdef __ANDROID__
    SLresult res;
    lock2();
    if(g_slBufQ) {
        res=(*g_slBufQ)->Clear(g_slBufQ);
        if(SL_RESULT_SUCCESS!=res) {
            putlog(__FILE__,__LINE__,"Clear error: result=%d",(int)res);
        }
    }
    if(g_slPlay) {
        res=(*g_slPlay)->SetPlayState(g_slPlay,SL_PLAYSTATE_STOPPED);
        if(SL_RESULT_SUCCESS!=res) {
            putlog(__FILE__,__LINE__,"SetPlayState error: result=%d",(int)res);
        }
    }
    g_slBufQ=NULL;
    unlock2();
    
    if(g_slPlayObj) {
        (*g_slPlayObj)->Destroy(g_slPlayObj);
        g_slPlayObj=NULL;
    }
    
    if(g_slMixObj) {
        (*g_slMixObj)->Destroy(g_slMixObj);
        g_slMixObj=NULL;
    }
    
    if(g_slEngObj) {
        (*g_slEngObj)->Destroy(g_slEngObj);
        g_slEngObj=NULL;
    }
#else
    STALIVE=0;
    pthread_join(TID,NULL);
#ifdef __APPLE__
    alcDestroyContext(sndCtx);
    alcCloseDevice(sndDev);
    sndCtx=NULL;
    sndDev=NULL;
#endif
#endif
}

/*
 *-----------------------------------------------------------------------------
 * sound ctrl thread procedure
 *-----------------------------------------------------------------------------
 */
#ifndef __ANDROID__
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
#endif

