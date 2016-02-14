/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - emulator
 *    Platform: Android
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <jni.h>
#include <sys/timeb.h>
#include <android/log.h>
#include <android/bitmap.h>
#include "vgs2.h"
#include "vgsdec.h"
#include "vgsspu.h"

void* _psg;
static void* _spu;
static pthread_mutex_t lckobj = PTHREAD_MUTEX_INITIALIZER;

/*
 *----------------------------------------------------------------------------
 * file static variables
 *----------------------------------------------------------------------------
 */
static unsigned short ADPAL[256];
static int REQ;

/*
 *----------------------------------------------------------------------------
 * internal functions
 *----------------------------------------------------------------------------
 */
void putlog(const char* fn, int ln, const char* msg, ...);
int isFullScreen();

/*
 *----------------------------------------------------------------------------
 * initialize rom data
 *----------------------------------------------------------------------------
 */
JNIEXPORT jint JNICALL Java_com_{Company}_{Project}_{Project}_setRomData(JNIEnv* env, jclass jc, jbyteArray src)
{
    jboolean b;
    int fsize;
    char* bin;
    int i, j;
    int cn, pn, bn;
    char s[4];
    char path[16];

    putlog(__FILE__, __LINE__, "*** SUZUKI PLAN - Video Game System mk-II (GameDaddy) ***");

    /* initialize vgs-bgm-decoder */
    _psg = vgsdec_create_context();
    if (NULL == _psg) {
        putlog(__FILE__, __LINE__, "Could not initialize vgs-bgm-decoder: errno=%d", errno);
        return -1;
    }

    /* get romdata from raw area */
    BN = 0;
    BR = NULL;
    bin = (char*)((*env)->GetByteArrayElements(env, src, &b));
    fsize = (int)((*env)->GetArrayLength(env, src));

    /* get number of files */
    memcpy(&BN, bin, sizeof(BN));
    bin += sizeof(BN);

    /* allocate buffer */
    if (NULL == (BR = (struct _BINREC*)malloc(BN * sizeof(struct _BINREC)))) {
        putlog(__FILE__, __LINE__, "Could not allocate ROM data area: errno=%d", errno);
        return -1;
    }
    memset(BR, 0, BN * sizeof(struct _BINREC));

    /* read filename */
    for (i = 0; i < BN; i++) {
        memset(BR[i].name, 0, 16);
        memcpy(BR[i].name, bin, 16);
        bin += 16;
        for (j = 0; '\0' != BR[i].name[j]; j++) {
            if (15 == j) {
                putlog(__FILE__, __LINE__, "ROM data is broken.");
                return -1;
            }
            BR[i].name[j] ^= 0xAA;
        }
    }

    /* read data */
    for (i = 0; i < BN; i++) {
        memcpy(s, bin, 4);
        bin += 4;
        BR[i].size = s[0];
        BR[i].size <<= 8;
        BR[i].size |= s[1];
        BR[i].size <<= 8;
        BR[i].size |= s[2];
        BR[i].size <<= 8;
        BR[i].size |= s[3];
        if (NULL == (BR[i].data = (char*)malloc(BR[i].size))) {
            putlog(__FILE__, __LINE__, "Could not allocate ROM data area: errno=%d", errno);
            return -1;
        }
        memcpy(BR[i].data, bin, BR[i].size);
        bin += BR[i].size;
    }
    putlog(__FILE__, __LINE__, "ROMDATA has extracted.");

    cn = 0;
    pn = 0;
    bn = 0;
    for (i = 0; i < 256; i++) {
        sprintf(path, "GSLOT%03d.CHR", i);
        if (0 == gload(i, path)) cn++;
        sprintf(path, "ESLOT%03d.PCM", i);
        if (0 == eload(i, path)) pn++;
        sprintf(path, "BSLOT%03d.BGM", i);
        if (0 == bload(i, path)) bn++;
    }
    putlog(__FILE__, __LINE__, "Data has extracted. (CHR=%d, PCM=%d, BGM=%d)", cn, pn, bn);

     /* initialize vgs-spu */
    _spu = vgsspu_start(sndbuf);
    if (NULL == _spu) {
        putlog(__FILE__, __LINE__, "Could not initialize vgs-spu: errno=%d", errno);
        return -1;
    }

   return 0;
}

/*
 *----------------------------------------------------------------------------
 * initialize system
 *----------------------------------------------------------------------------
 */
JNIEXPORT jint JNICALL Java_com_{Company}_{Project}_{Project}_init(JNIEnv* env, jclass jc, jint sx, jint sy)
{
    /* change current directory */
    putlog(__FILE__, __LINE__, "Using data directory...");
    if (-1 == chdir("/data/data/com.{Company}.{Project}")) {
        putlog(__FILE__, __LINE__, "Could not change to the save-directory. (errno=%d)", errno);
        return -1;
    } else {
        putlog(__FILE__, __LINE__, "Change directory: /data/data/com.{Company}.{Project}");
    }

    /* initialize AP */
    jint rc = (jint)vgs2_init();
    if (0 == rc) {
        make_pallet();
    }
    return rc;
}

/*
 *----------------------------------------------------------------------------
 * create android bitmap from vram
 *----------------------------------------------------------------------------
 */
JNIEXPORT jint JNICALL Java_com_{Company}_{Project}_{Project}_setVram(JNIEnv* env, jclass jc, jobject bmp)
{
    unsigned short* ptr;
    int vx, vy, px, py, vp, pp;
    jint rc;

    if (_pause) {
        rc = vgs2_pause();
    } else {
        rc = vgs2_loop();
    }

    if (AndroidBitmap_lockPixels(env, bmp, (void**)&ptr) < 0) {
        return;
    }
    vp = 0;
    pp = 0;
    if (_interlace) {
        for (vy = 0, py = 0; vy < 200; vy++, py += 2) {
            for (vx = 0, px = 0; vx < 160; vx++, px += 2) {
                if (_vram.sp[vp]) {
                    ptr[pp] = ADPAL[_vram.sp[vp]];
                    ptr[pp + 1] = ADPAL[_vram.sp[vp]];
                    ptr[pp + 320] = 0;
                    ptr[pp + 321] = 0;
                    _vram.sp[vp] = 0;
                } else {
                    ptr[pp] = ADPAL[_vram.bg[vp]];
                    ptr[pp + 1] = ADPAL[_vram.bg[vp]];
                    ptr[pp + 320] = 0;
                    ptr[pp + 321] = 0;
                }
                vp++;
                pp += 2;
            }
            pp += 320;
        }
    } else {
        for (vy = 0, py = 0; vy < 200; vy++, py += 2) {
            for (vx = 0, px = 0; vx < 160; vx++, px += 2) {
                if (_vram.sp[vp]) {
                    ptr[pp] = ADPAL[_vram.sp[vp]];
                    ptr[pp + 1] = ADPAL[_vram.sp[vp]];
                    ptr[pp + 320] = ADPAL[_vram.sp[vp]];
                    ptr[pp + 321] = ADPAL[_vram.sp[vp]];
                    _vram.sp[vp] = 0;
                } else {
                    ptr[pp] = ADPAL[_vram.bg[vp]];
                    ptr[pp + 1] = ADPAL[_vram.bg[vp]];
                    ptr[pp + 320] = ADPAL[_vram.bg[vp]];
                    ptr[pp + 321] = ADPAL[_vram.bg[vp]];
                }
                vp++;
                pp += 2;
            }
            pp += 320;
        }
    }
    AndroidBitmap_unlockPixels(env, bmp);

    return rc;
}

/*
 *----------------------------------------------------------------------------
 * set touched info
 *----------------------------------------------------------------------------
 */
JNIEXPORT void JNICALL Java_com_{Company}_{Project}_{Project}_setInput(JNIEnv* env, jclass jc, jint isOn, jfloat x, jfloat y)
{
    _touch.s = (int)isOn;
    if (_touch.s) {
        _touch.px = _touch.cx;
        _touch.py = _touch.cy;
        _touch.cx = (int)x;
        _touch.cy = (int)y;
        if (_touch.s && _touch.px && _touch.py) {
            _touch.dx = _touch.cx - _touch.px;
            _touch.dy = _touch.cy - _touch.py;
        } else {
            _touch.dx = 0;
            _touch.dy = 0;
        }
    } else {
        memset(&_touch, 0, sizeof(_touch));
    }
}

/*
 *----------------------------------------------------------------------------
 * set pause
 *----------------------------------------------------------------------------
 */
JNIEXPORT void JNICALL Java_com_{Company}_{Project}_{Project}_setPause(JNIEnv* env, jclass jc, jint p)
{
    _pause = (int)p;
}

/*
 *----------------------------------------------------------------------------
 * terminate procedure
 *----------------------------------------------------------------------------
 */
JNIEXPORT void JNICALL Java_com_{Company}_{Project}_{Project}_term(JNIEnv* env, jclass jc)
{
    vgs2_term();
    vgsspu_end(_spu);
    vgsdec_release_context(_psg);
}

/*
 *----------------------------------------------------------------------------
 * get request
 *----------------------------------------------------------------------------
 */
JNIEXPORT jint JNICALL Java_com_{Company}_{Project}_{Project}_getreq(JNIEnv* env, jclass jc)
{
    int ret = REQ;
    REQ = 0;
    return (jint)ret;
}

/*
 *----------------------------------------------------------------------------
 * logging
 *----------------------------------------------------------------------------
 */
void putlog(const char* fn, int ln, const char* msg, ...)
{
    va_list args;
    time_t t1;
    struct tm* t2;
    struct timeb tb;
    char buf[1024];

    ftime(&tb);
    t1 = tb.time;
    t2 = localtime(&t1);
    sprintf(buf, "%04d/%02d/%02d %02d:%02d:%02d.%03d ", t2->tm_year + 1900, t2->tm_mon + 1, t2->tm_mday, t2->tm_hour, t2->tm_min, t2->tm_sec, tb.millitm);
    va_start(args, msg);
    vsprintf(buf + strlen(buf), msg, args);
    va_end(args);
    __android_log_write(ANDROID_LOG_INFO, "GameDaddy", buf);
}

/*
 *----------------------------------------------------------------------------
 * make pallet table for Android
 *----------------------------------------------------------------------------
 */
void make_pallet()
{
    int i, j;
    int r, g, b;
    for (j = 0, i = 0; i < 256; i++) {
        r = (_PAL[i] & 0x00FF0000) >> 16;
        g = (_PAL[i] & 0x0000FF00) >> 8;
        b = (_PAL[i] & 0x000000FF);
        r &= 0xF8;
        r >>= 3;
        g &= 0xFC;
        g >>= 2;
        b &= 0xF8;
        b >>= 3;
        ADPAL[i] = r;
        ADPAL[i] <<= 6;
        ADPAL[i] |= g;
        ADPAL[i] <<= 5;
        ADPAL[i] |= b;
    }
    ADPAL[0] = 0x1865;
}

/*
 *----------------------------------------------------------------------------
 * open file
 *----------------------------------------------------------------------------
 */
FILE* vgs2_fopen(const char* n, const char* p)
{
    return fopen(n, p);
}

/*
 *----------------------------------------------------------------------------
 * show ads
 *----------------------------------------------------------------------------
 */
void vgs2_showAds()
{
    REQ = 1;
}

/*
 *----------------------------------------------------------------------------
 * delete ads
 *----------------------------------------------------------------------------
 */
void vgs2_deleteAds()
{
    REQ = 2;
}

void lock()
{
    pthread_mutex_lock(&lckobj);
}

void unlock()
{
    pthread_mutex_unlock(&lckobj);
}

