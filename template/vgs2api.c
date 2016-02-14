/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - application program interface
 *    Platform: Common
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#include "vgs2.h"
#include "vgsdec.h"

/*
 *----------------------------------------------------------------------------
 * Global variables
 *----------------------------------------------------------------------------
 */
int BN;
struct _BINREC* BR;
struct _SLOT _slot[256];
struct _EFF _eff[256];
void* _BGM[256];
static size_t _BGMSIZE[256];
struct _VRAM _vram;
struct _TOUCH _touch;
unsigned char _mute;
unsigned char _pause;
static int _bstop;
static int _bkoff[6];
unsigned char _interlace = 1;
int _PAL[256] = {0x00000000, 0x00003F00, 0x00005F00, 0x00007F00, 0x00009F00, 0x0000BF00, 0x0000DF00, 0x0000FF00};
extern void* _psg;

/*
 *----------------------------------------------------------------------------
 * file static variables
 *----------------------------------------------------------------------------
 */
static unsigned int seed;
static short sin256[628] = {
    0,    2,    5,    7,    10,   12,   15,   17,   20,   23,   25,   28,   30,   33,   35,   38,   40,   43,   45,   48,   50,   53,   55,   58,   60,   63,
    65,   68,   70,   73,   75,   78,   80,   82,   85,   87,   90,   92,   94,   97,   99,   102,  104,  106,  109,  111,  113,  115,  118,  120,  122,  124,
    127,  129,  131,  133,  135,  138,  140,  142,  144,  146,  148,  150,  152,  154,  156,  158,  160,  162,  164,  166,  168,  170,  172,  174,  176,  178,
    180,  181,  183,  185,  187,  188,  190,  192,  194,  195,  197,  198,  200,  202,  203,  205,  206,  208,  209,  211,  212,  214,  215,  216,  218,  219,
    220,  222,  223,  224,  225,  226,  228,  229,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239,  240,  241,  242,  242,  243,  244,  245,  245,
    246,  247,  247,  248,  249,  249,  250,  250,  251,  251,  252,  252,  253,  253,  253,  254,  254,  254,  254,  255,  255,  255,  255,  255,  255,  255,
    255,  255,  255,  255,  255,  255,  255,  255,  255,  255,  254,  254,  254,  254,  253,  253,  253,  252,  252,  251,  251,  250,  250,  249,  249,  248,
    248,  247,  246,  246,  245,  244,  243,  243,  242,  241,  240,  239,  238,  237,  236,  235,  234,  233,  232,  231,  230,  229,  228,  227,  225,  224,
    223,  222,  220,  219,  218,  217,  215,  214,  212,  211,  209,  208,  206,  205,  203,  202,  200,  199,  197,  195,  194,  192,  190,  189,  187,  185,
    183,  182,  180,  178,  176,  174,  172,  171,  169,  167,  165,  163,  161,  159,  157,  155,  153,  151,  149,  146,  144,  142,  140,  138,  136,  134,
    131,  129,  127,  125,  123,  120,  118,  116,  114,  111,  109,  107,  104,  102,  100,  97,   95,   92,   90,   88,   85,   83,   80,   78,   76,   73,
    71,   68,   66,   63,   61,   58,   56,   53,   51,   48,   46,   43,   41,   38,   36,   33,   31,   28,   25,   23,   20,   18,   15,   13,   10,   8,
    5,    2,    0,    -2,   -4,   -7,   -9,   -12,  -14,  -17,  -20,  -22,  -25,  -27,  -30,  -32,  -35,  -37,  -40,  -42,  -45,  -47,  -50,  -52,  -55,  -57,
    -60,  -62,  -65,  -67,  -70,  -72,  -75,  -77,  -80,  -82,  -84,  -87,  -89,  -92,  -94,  -96,  -99,  -101, -104, -106, -108, -110, -113, -115, -117, -120,
    -122, -124, -126, -129, -131, -133, -135, -137, -139, -142, -144, -146, -148, -150, -152, -154, -156, -158, -160, -162, -164, -166, -168, -170, -172, -174,
    -176, -177, -179, -181, -183, -185, -186, -188, -190, -192, -193, -195, -197, -198, -200, -201, -203, -204, -206, -207, -209, -210, -212, -213, -215, -216,
    -217, -219, -220, -221, -223, -224, -225, -226, -227, -229, -230, -231, -232, -233, -234, -235, -236, -237, -238, -239, -240, -241, -241, -242, -243, -244,
    -245, -245, -246, -247, -247, -248, -249, -249, -250, -250, -251, -251, -252, -252, -253, -253, -253, -254, -254, -254, -254, -255, -255, -255, -255, -255,
    -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -254, -254, -254, -253, -253, -253, -252, -252, -251, -251, -251, -250, -249,
    -249, -248, -248, -247, -246, -246, -245, -244, -243, -243, -242, -241, -240, -239, -238, -237, -237, -236, -235, -233, -232, -231, -230, -229, -228, -227,
    -226, -224, -223, -222, -221, -219, -218, -217, -215, -214, -213, -211, -210, -208, -207, -205, -204, -202, -201, -199, -197, -196, -194, -192, -191, -189,
    -187, -185, -184, -182, -180, -178, -176, -175, -173, -171, -169, -167, -165, -163, -161, -159, -157, -155, -153, -151, -149, -147, -145, -143, -140, -138,
    -136, -134, -132, -130, -127, -125, -123, -121, -118, -116, -114, -112, -109, -107, -105, -102, -100, -98,  -95,  -93,  -90,  -88,  -86,  -83,  -81,  -78,
    -76,  -73,  -71,  -69,  -66,  -64,  -61,  -59,  -56,  -54,  -51,  -49,  -46,  -44,  -41,  -39,  -36,  -33,  -31,  -28,  -26,  -23,  -21,  -18,  -16,  -13,
    -11,  -8,   -5,   -3};
static short cos256[628] = {
    256,  255,  255,  255,  255,  255,  255,  255,  255,  254,  254,  254,  254,  253,  253,  253,  252,  252,  251,  251,  250,  250,  249,  249,  248,  248,
    247,  246,  246,  245,  244,  243,  243,  242,  241,  240,  239,  238,  237,  236,  235,  234,  233,  232,  231,  230,  229,  228,  227,  225,  224,  223,
    222,  220,  219,  218,  216,  215,  214,  212,  211,  209,  208,  206,  205,  203,  202,  200,  199,  197,  195,  194,  192,  190,  189,  187,  185,  183,
    181,  180,  178,  176,  174,  172,  170,  168,  167,  165,  163,  161,  159,  157,  155,  153,  150,  148,  146,  144,  142,  140,  138,  136,  133,  131,
    129,  127,  125,  122,  120,  118,  116,  113,  111,  109,  106,  104,  102,  99,   97,   95,   92,   90,   87,   85,   83,   80,   78,   75,   73,   70,
    68,   66,   63,   61,   58,   56,   53,   51,   48,   46,   43,   40,   38,   35,   33,   30,   28,   25,   23,   20,   18,   15,   12,   10,   7,    5,
    2,    0,    -2,   -4,   -7,   -10,  -12,  -15,  -17,  -20,  -22,  -25,  -27,  -30,  -32,  -35,  -38,  -40,  -43,  -45,  -48,  -50,  -53,  -55,  -58,  -60,
    -63,  -65,  -68,  -70,  -73,  -75,  -77,  -80,  -82,  -85,  -87,  -89,  -92,  -94,  -97,  -99,  -101, -104, -106, -108, -111, -113, -115, -118, -120, -122,
    -124, -127, -129, -131, -133, -135, -137, -140, -142, -144, -146, -148, -150, -152, -154, -156, -158, -160, -162, -164, -166, -168, -170, -172, -174, -176,
    -178, -179, -181, -183, -185, -187, -188, -190, -192, -193, -195, -197, -198, -200, -201, -203, -205, -206, -208, -209, -211, -212, -213, -215, -216, -218,
    -219, -220, -221, -223, -224, -225, -226, -228, -229, -230, -231, -232, -233, -234, -235, -236, -237, -238, -239, -240, -241, -242, -242, -243, -244, -245,
    -245, -246, -247, -247, -248, -249, -249, -250, -250, -251, -251, -252, -252, -253, -253, -253, -254, -254, -254, -254, -255, -255, -255, -255, -255, -255,
    -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -254, -254, -254, -253, -253, -253, -252, -252, -251, -251, -250, -250, -249, -249,
    -248, -248, -247, -246, -246, -245, -244, -243, -243, -242, -241, -240, -239, -238, -237, -236, -235, -234, -233, -232, -231, -230, -229, -228, -227, -226,
    -224, -223, -222, -221, -219, -218, -217, -215, -214, -212, -211, -210, -208, -207, -205, -204, -202, -200, -199, -197, -196, -194, -192, -191, -189, -187,
    -185, -184, -182, -180, -178, -176, -174, -173, -171, -169, -167, -165, -163, -161, -159, -157, -155, -153, -151, -149, -147, -145, -142, -140, -138, -136,
    -134, -132, -129, -127, -125, -123, -121, -118, -116, -114, -111, -109, -107, -104, -102, -100, -97,  -95,  -93,  -90,  -88,  -85,  -83,  -81,  -78,  -76,
    -73,  -71,  -68,  -66,  -63,  -61,  -58,  -56,  -53,  -51,  -48,  -46,  -43,  -41,  -38,  -36,  -33,  -31,  -28,  -26,  -23,  -21,  -18,  -15,  -13,  -10,
    -8,   -5,   -3,   0,    1,    4,    7,    9,    12,   14,   17,   19,   22,   24,   27,   30,   32,   35,   37,   40,   42,   45,   47,   50,   52,   55,
    57,   60,   62,   65,   67,   70,   72,   75,   77,   79,   82,   84,   87,   89,   92,   94,   96,   99,   101,  103,  106,  108,  110,  113,  115,  117,
    119,  122,  124,  126,  128,  131,  133,  135,  137,  139,  141,  144,  146,  148,  150,  152,  154,  156,  158,  160,  162,  164,  166,  168,  170,  172,
    174,  175,  177,  179,  181,  183,  184,  186,  188,  190,  191,  193,  195,  196,  198,  200,  201,  203,  204,  206,  207,  209,  210,  212,  213,  215,
    216,  217,  219,  220,  221,  223,  224,  225,  226,  227,  229,  230,  231,  232,  233,  234,  235,  236,  237,  238,  239,  240,  241,  241,  242,  243,
    244,  245,  245,  246,  247,  247,  248,  249,  249,  250,  250,  251,  251,  252,  252,  253,  253,  253,  254,  254,  254,  254,  255,  255,  255,  255,
    255,  255,  255,  255};

/*
 *----------------------------------------------------------------------------
 * file static functions
 *----------------------------------------------------------------------------
 */
static char* getbin(const char* name, int* size);
static int gclip(unsigned char n, int* sx, int* sy, int* xs, int* ys, int* dx, int* dy);
static float myatan2(int a, int b);
static float mysqrt(float x);

/*
 *----------------------------------------------------------------------------
 * sound buffering
 *----------------------------------------------------------------------------
 */
void sndbuf(void* data, size_t size)
{
    static int an;
    char* buf = (char*)data;
    int i, j;
    int wav;
    int cs;
    short* bp;

    an = 1 - an;

    memset(buf, 0, size);
    if (_pause || _mute) {
        return;
    }

    for (i = 0; i < 256; i++) {
        if (_eff[i].flag) {
            if (1 < _eff[i].flag) {
                eff_pos(&_eff[i], 0);
                eff_flag(&_eff[i], 1);
            }
            /* calc copy size */
            cs = _eff[i].size - _eff[i].pos;
            if (size < (size_t)cs) {
                cs = (int)size;
            }
            /* buffering */
            for (j = 0; j < cs; j += 2) {
                bp = (short*)(&buf[j]);
                wav = *bp;
                wav += *((short*)&(_eff[i].dat[_eff[i].pos + j]));
                if (32767 < wav)
                    wav = 32767;
                else if (wav < -32768)
                    wav = -32768;
                (*bp) = (short)wav;
            }
            /* change position */
            eff_pos(&_eff[i], _eff[i].pos + cs);
            if (_eff[i].size <= _eff[i].pos) {
                eff_flag(&_eff[i], 0); /* end sound */
            }
        } else {
            eff_pos(&_eff[i], 0);
        }
    }

    if (_bstop || !vgsdec_get_value(_psg, VGSDEC_REG_PLAYING)) return;
    vgsdec_execute(_psg, buf, size);
}

/*
 *----------------------------------------------------------------------------
 * set sound-effect flag
 *----------------------------------------------------------------------------
 */
void eff_flag(struct _EFF* e, unsigned int f)
{
    lock();
    e->flag = f;
    if (!f) e->pos = 0;
    unlock();
}

/*
 *----------------------------------------------------------------------------
 * set sound-effect position
 *----------------------------------------------------------------------------
 */
void eff_pos(struct _EFF* e, unsigned int f)
{
    lock();
    e->pos = f;
    unlock();
}

/*
 *----------------------------------------------------------------------------
 * extract a CHR file to a GSLOT (INTERNAL FUNCION)
 *----------------------------------------------------------------------------
 */
int gload(unsigned char n, const char* name)
{
    unsigned char* bin;
    int rc = -1;
    int gsize;
    int size;

    /* dispose old data if existing */
    if (_slot[n].dat) {
        free(_slot[n].dat);
        _slot[n].dat = NULL;
    }

    /* get romdata */
    if (NULL == (bin = (unsigned char*)getbin(name, &size))) {
        goto ENDPROC;
    }

    /* check eye-catch and size */
    if ('S' != bin[0] || 'Y' != bin[1]) {
        goto ENDPROC;
    }
    _slot[n].xs = bin[2] + 1;
    _slot[n].ys = bin[3] + 1;
    gsize = (_slot[n].xs) * (_slot[n].ys);

    /* allocate buffer */
    if (NULL == (_slot[n].dat = (unsigned char*)malloc(gsize))) {
        goto ENDPROC;
    }

    /* load graphics data */
    bin += 4;
    memcpy(_slot[n].dat, bin, gsize);

    rc = 0;
ENDPROC:
    if (rc) {
        if (_slot[n].dat) {
            free(_slot[n].dat);
            _slot[n].dat = NULL;
        }
        _slot[n].xs = 0;
        _slot[n].ys = 0;
    }
    return rc;
}

/*
 *----------------------------------------------------------------------------
 * extract a PCM file to an ESLOT (INTERNAL FUNCION)
 *----------------------------------------------------------------------------
 */
int eload(unsigned char n, const char* name)
{
    unsigned char* bin;
    int size;
    int rc = -1;

    /* dispose old data */
    if (_eff[n].dat) {
        free(_eff[n].dat);
        _eff[n].dat = NULL;
        _eff[n].size = 0;
    }

    /* get romdata */
    if (NULL == (bin = (unsigned char*)getbin(name, &size))) {
        goto ENDPROC;
    }

    /* check eye-catch */
    if ('E' != bin[0] || 'F' != bin[1] || 'F' != bin[2] || '\0' != bin[3]) {
        goto ENDPROC;
    }

    /* change byte order */
    _eff[n].size = 0;
    _eff[n].size |= bin[4];
    _eff[n].size <<= 8;
    _eff[n].size |= bin[5];
    _eff[n].size <<= 8;
    _eff[n].size |= bin[6];
    _eff[n].size <<= 8;
    _eff[n].size |= bin[7];
    bin += 8;

    /* allocate buffer */
    if (NULL == (_eff[n].dat = (unsigned char*)malloc(_eff[n].size))) {
        goto ENDPROC;
    }

    /* copy */
    memcpy(_eff[n].dat, bin, _eff[n].size);

    rc = 0;
ENDPROC:
    if (rc) {
        if (_eff[n].dat) {
            free(_eff[n].dat);
            _eff[n].dat = NULL;
        }
        _eff[n].size = 0;
    }
    return rc;
}

/*
 *----------------------------------------------------------------------------
 * extract a BGM data
 *----------------------------------------------------------------------------
 */
int bload(unsigned char n, const char* name)
{
    int size;
    _BGM[n] = (char*)getbin(name, &size);
    if (NULL == _BGM[n]) {
        _BGMSIZE[n] = 0;
        return -1;
    }
    _BGMSIZE[n] = (size_t)size;
    return 0;
}

/*
 *----------------------------------------------------------------------------
 * get rom data (INTERNAL FUNCTION)
 *----------------------------------------------------------------------------
 */
static char* getbin(const char* name, int* size)
{
    int i;
    for (i = 0; i < BN; i++) {
        if (0 == strcmp(BR[i].name, name)) {
            *size = BR[i].size;
            return BR[i].data;
        }
    }
    return NULL;
}

/*
 *----------------------------------------------------------------------------
 * clipping (INTERNAL FUNCTION)
 *----------------------------------------------------------------------------
 */
static int gclip(unsigned char n, int* sx, int* sy, int* xs, int* ys, int* dx, int* dy)
{
    /* slot available? */
    if (NULL == _slot[n].dat) {
        return -1;
    }

    /* in the screen */
    if ((*sx) < 0 || _slot[n].xs < (*sx) + (*xs) || (*sy) < 0 || _slot[n].ys < (*sy) + (*ys) || (*dx) + (*xs) < 0 || XSIZE <= *dx || (*dy) + (*ys) < 0 ||
        YSIZE <= *dy) {
        return -1;
    }

    /* clipping */
    if ((*dx) < 0) {
        (*sx) -= (*dx);
        (*xs) += (*dx);
        (*dx) = 0;
    }
    if (XSIZE < (*dx) + (*xs)) {
        (*xs) -= ((*dx) + (*xs)) - XSIZE;
    }
    if ((*dy) < 0) {
        (*sy) -= (*dy);
        (*ys) += (*dy);
        (*dy) = 0;
    }
    if (YSIZE < (*dy) + (*ys)) {
        (*ys) -= ((*dy) + (*ys)) - YSIZE;
    }
    return 0;
}

/*
 *----------------------------------------------------------------------------
 * atan2
 *----------------------------------------------------------------------------
 */
static float myatan2(int a, int b)
{
    float u;
    float ret;
    float v;
    float d;
    int c;

    if (a == 0x80000000)
        a++;
    else if (a == 0)
        a--;
    if (b == 0x80000000)
        b++;
    else if (b == 0)
        b--;

    if (a < 0) {
        if (b < 0) {
            a = -a;
            b = -b;
            return 270 - (90 - myatan2(a, b));
        } else {
            return 360 - myatan2(-a, b);
        }
    } else if (b < 0) {
        return 180 - myatan2(a, -b);
    } else if (a > b) {
        return 90 - myatan2(b, a);
    }

    u = (float)a / b;

    if (a * 3 >= b * 2) {
        v = (mysqrt(1.0f + u * u) - 1.0f) / u;
        return (float)(2.0 * myatan2((int)(v * 2000 + 0.5), 2000));
    }

    ret = 0.0f;
    v = u;
    d = 99.0f;
    c = 1;

    while (d >= 0.1 || d <= -0.1) {
        d = 57.295778965948f * v / c;
        ret += d;
        v *= u * u;
        c = (c < 0 ? 2 : -2) - c;
    }
    return ret;
}

/*
 *----------------------------------------------------------------------------
 * sqrt
 *----------------------------------------------------------------------------
 */
static float mysqrt(float x)
{
    float s, last;

    if (x <= 0.0f) {
        return 0.0f;
    }

    if (x > 1) {
        s = x;
    } else {
        s = 1;
    }

    do {
        last = s;
        s = (x / s + s) * 0.5f;
    } while (s < last);

    return last;
}

/*
 *----------------------------------------------------------------------------
 * get the status of touch pannel
 *----------------------------------------------------------------------------
 */
void vgs2_touch(int* s, int* cx, int* cy, int* dx, int* dy)
{
    *s = _touch.s;
    *cx = _touch.cx;
    *cy = _touch.cy;
    *dx = _touch.dx;
    *dy = _touch.dy;
    _touch.px = _touch.cx;
    _touch.py = _touch.cy;
    _touch.dx = 0;
    _touch.dy = 0;
}

/*
 *----------------------------------------------------------------------------
 * set the status of pause
 *----------------------------------------------------------------------------
 */
void vgs2_setPause(unsigned char c)
{
    _pause = c;
}

/*
 *----------------------------------------------------------------------------
 * get radian
 *----------------------------------------------------------------------------
 */
int vgs2_rad(int x1, int y1, int x2, int y2)
{
    float k = myatan2(x2 - x1, y2 - y1);
    return (int)(k * 3.1415f / 1.80f);
}

/*
 *----------------------------------------------------------------------------
 * get degree
 *----------------------------------------------------------------------------
 */
int vgs2_deg(int x1, int y1, int x2, int y2)
{
    return (int)myatan2(x2 - x1, y2 - y1);
}

/*
 *----------------------------------------------------------------------------
 * degree to radian
 *----------------------------------------------------------------------------
 */
int vgs2_deg2rad(int deg)
{
    float k;
    deg %= 360;
    if (deg < 0) deg += 360;
    k = (float)deg;
    k *= 3.1415f;
    k /= 1.80f;
    while (627 < k)
        k -= 628;
    return (int)k;
}

/*
 *----------------------------------------------------------------------------
 * radian to degree
 *----------------------------------------------------------------------------
 */
int vgs2_rad2deg(int rad)
{
    float r;
    rad %= 628;
    if (rad < 0) rad += 628;
    r = (float)rad;
    r *= 1.80f;
    r /= 3.1415f;
    while (359 < r)
        r -= 360;
    return (int)r;
}

/*
 *----------------------------------------------------------------------------
 * initialize random value
 *----------------------------------------------------------------------------
 */
void vgs2_rands()
{
    seed = 0x12345678;
}

/*
 *----------------------------------------------------------------------------
 * get random value
 *----------------------------------------------------------------------------
 */
int vgs2_rand()
{
    seed = seed * 48828125 + 1;
    return seed & 0x7FFFFFFF;
}

/*
 *----------------------------------------------------------------------------
 * get sin x 256
 *----------------------------------------------------------------------------
 */
int vgs2_sin(int r)
{
    r %= 628;
    if (r < 0) r += 628;
    return sin256[r];
}

/*
 *----------------------------------------------------------------------------
 * get cos x 256
 *----------------------------------------------------------------------------
 */
int vgs2_cos(int r)
{
    r %= 628;
    if (r < 0) r += 628;
    return cos256[r];
}

/*
 *----------------------------------------------------------------------------
 * play a sound effect
 *----------------------------------------------------------------------------
 */
void vgs2_eff(unsigned char n)
{
    if (_eff[n].dat) {
        eff_flag(&_eff[n], 1 + _eff[n].flag);
    }
}

/*
 *----------------------------------------------------------------------------
 * stop a sound effect
 *----------------------------------------------------------------------------
 */
void vgs2_effstop(unsigned char n)
{
    if (_eff[n].dat) {
        eff_flag(&_eff[n], 0);
    }
}

/*
 *----------------------------------------------------------------------------
 * stop the all of sound effects
 *----------------------------------------------------------------------------
 */
void vgs2_effstopA()
{
    int i;
    for (i = 0; i < 256; i++) {
        if (_eff[i].dat) {
            eff_flag(&_eff[i], 0);
        }
    }
}

/*
 *----------------------------------------------------------------------------
 * set mute
 *----------------------------------------------------------------------------
 */
void vgs2_setmute(unsigned char n)
{
    _mute = n;
}

/*
 *----------------------------------------------------------------------------
 * get mute
 *----------------------------------------------------------------------------
 */
unsigned char vgs2_getmute()
{
    return _mute;
}

/*
 *----------------------------------------------------------------------------
 * Set pallet base colour
 * 1:blue, 2:red, 3:magenta, 4:green, 5:cyan, 6:yellow, 7:white
 *----------------------------------------------------------------------------
 */
void vgs2_pallet(int c)
{
    int i;
    int b = 255;
    c &= 7;
    if (0 == c) return;
    memset(_PAL, 0, sizeof(_PAL));
    for (i = 7; i; i--, b -= 32) {
        switch (c) {
            case 1:
                _PAL[i] = b;
                break;
            case 2:
                _PAL[i] = b << 16;
                break;
            case 3:
                _PAL[i] = (b << 16) | b;
                break;
            case 4:
                _PAL[i] = b << 8;
                break;
            case 5:
                _PAL[i] = (b << 8) | b;
                break;
            case 6:
                _PAL[i] = (b << 16) | (b << 8);
                break;
            case 7:
                _PAL[i] = (b << 16) | (b << 8) | b;
                break;
        }
    }
    make_pallet();
}

/*
 *----------------------------------------------------------------------------
 * Set 8 bit color mode
 *----------------------------------------------------------------------------
 */
int vgs2_pallet256(unsigned char n)
{
    const char* data;
    unsigned int size;
    data = vgs2_getdata(n, &size);
    if (NULL == data) {
        return -1;
    }
    if (size != 1024) {
        return -1;
    }
    memcpy(_PAL, data, size);
    make_pallet();
    return 0;
}

/*
 *----------------------------------------------------------------------------
 * clear screen
 *----------------------------------------------------------------------------
 */
void vgs2_clear(unsigned char c)
{
    memset(_vram.bg, c, sizeof(_vram.bg));
}

/*
 *----------------------------------------------------------------------------
 * raster scroll
 *----------------------------------------------------------------------------
 */
void vgs2_scroll(int x, int y)
{
    int i;
    if (XSIZE <= x || YSIZE <= y || XSIZE <= (-x) || YSIZE <= (-y)) {
        vgs2_clear(0);
    }
    if (0 < x) {
        for (i = 0; i < YSIZE; i++) {
            memmove(&_vram.bg[i * XSIZE + x], &_vram.bg[i * XSIZE], XSIZE - x);
            memset(&_vram.bg[i * XSIZE], 0, x);
        }
    } else if (x < 0) {
        for (i = 0; i < YSIZE; i++) {
            memmove(&_vram.bg[i * XSIZE], &_vram.bg[i * XSIZE - x], XSIZE + x);
            memset(&_vram.bg[i * XSIZE + (XSIZE + x)], 0, -x);
        }
    }
    if (0 < y) {
        memmove(&_vram.bg[y * XSIZE], &_vram.bg[0], (YSIZE - y) * XSIZE);
        memset(&_vram.bg[0], 0, y * XSIZE);
    } else if (y < 0) {
        memmove(&_vram.bg[0], &_vram.bg[(-y) * XSIZE], (YSIZE + y) * XSIZE);
        memset(&_vram.bg[(YSIZE + y) * XSIZE], 0, (-y) * XSIZE);
    }
}

/*
 *----------------------------------------------------------------------------
 * draw pixel
 *----------------------------------------------------------------------------
 */
void vgs2_pixel(unsigned char* p, int x, int y, unsigned char c)
{
    if (0 <= x && x < XSIZE && 0 <= y && y < YSIZE) {
        p[y * XSIZE + x] = c;
    }
}

/*
 *----------------------------------------------------------------------------
 * draw line
 *----------------------------------------------------------------------------
 */
void vgs2_line(unsigned char* p, int fx, int fy, int tx, int ty, unsigned char c)
{
    int idx, idy;
    int ia, ib, ie;
    int w;
    idx = tx - fx;
    idy = ty - fy;
    if (!idx || !idy) {
        if (tx < fx) {
            w = fx;
            fx = tx;
            tx = w;
        }
        if (ty < fy) {
            w = fy;
            fy = ty;
            ty = w;
        }
        if (0 == idy)
            for (; fx <= tx; fx++)
                vgs2_pixel(p, fx, fy, c);
        else
            for (; fy <= ty; fy++)
                vgs2_pixel(p, fx, fy, c);
        return;
    }
    w = 1;
    ia = vgs2_abs(idx);
    ib = vgs2_abs(idy);
    if (ia >= ib) {
        ie = -vgs2_abs(idy);
        while (w) {
            vgs2_pixel(p, fx, fy, c);
            if (fx == tx) break;
            fx += vgs2_sgn(idx);
            ie += 2 * ib;
            if (ie >= 0) {
                fy += vgs2_sgn(idy);
                ie -= 2 * ia;
            }
        }
    } else {
        ie = -vgs2_abs(idx);
        while (w) {
            vgs2_pixel(p, fx, fy, c);
            if (fy == ty) break;
            fy += vgs2_sgn(idy);
            ie += 2 * ia;
            if (ie >= 0) {
                fx += vgs2_sgn(idx);
                ie -= 2 * ib;
            }
        }
    }
}

/*
 *----------------------------------------------------------------------------
 * draw circle
 *----------------------------------------------------------------------------
 */
void vgs2_circle(char* p, int x, int y, int r, unsigned char c)
{
    float x1, y1, x2, y2;
    int flg;

    flg = 0;
    x1 = (float)r;
    y1 = 0.0;

    while (!flg) {
        x2 = x1 - (y1 / 64.0f);
        y2 = y1 + (x2 / 64.0f);
        vgs2_pixel((unsigned char*)p, (int)x2 + x, (int)y2 + y, c);
        x1 = x2;
        y1 = y2;
        flg = (((x2 > r - 1.0f) && (x2 < r)) && ((y2 > -1.0f) && (y2 < 0.0f)));
    }
}

/*
 *----------------------------------------------------------------------------
 * draw filled box
 *----------------------------------------------------------------------------
 */
void vgs2_boxf(unsigned char* p, int fx, int fy, int tx, int ty, unsigned char c)
{
    int w;
    if (tx < fx) {
        w = fx;
        fx = tx;
        tx = w;
    }
    if (ty < fy) {
        w = fy;
        fy = ty;
        ty = w;
    }
    if (XSIZE <= fx || YSIZE <= fy || tx < 0 || ty < 0) {
        return;
    }
    if (fx < 0) {
        fx = 0;
    }
    if (fy < 0) {
        fy = 0;
    }
    if (XSIZE <= tx) {
        tx = XSIZE - 1;
    }
    if (YSIZE <= ty) {
        ty = YSIZE - 1;
    }
    w = tx - fx;
    w++;
    for (; fy <= ty; fy++) {
        memset(&p[fy * XSIZE + fx], c, w);
    }
}

/*
 *----------------------------------------------------------------------------
 * draw background from slot
 *----------------------------------------------------------------------------
 */
void vgs2_putBG(unsigned char n, int sx, int sy, int xs, int ys, int dx, int dy)
{
    int i;
    int posT;
    int posF;
    if (gclip(n, &sx, &sy, &xs, &ys, &dx, &dy)) {
        return;
    }
    posT = dy * XSIZE + dx;
    posF = sy * _slot[n].xs + sx;
    for (i = 0; i < ys; i++) {
        memcpy(&(_vram.bg[(posT & 0xffff)]), &(_slot[n].dat[posF]), xs);
        posT += XSIZE;
        posF += _slot[n].xs;
    }
}

/*
 *----------------------------------------------------------------------------
 * draw background from slot without pallet 0
 *----------------------------------------------------------------------------
 */
void vgs2_putBG2(unsigned char n, int sx, int sy, int xs, int ys, int dx, int dy)
{
    int i, j;
    int posT;
    int posF;
    if (gclip(n, &sx, &sy, &xs, &ys, &dx, &dy)) {
        return;
    }
    posT = dy * XSIZE + dx;
    posF = sy * _slot[n].xs + sx;
    for (j = 0; j < ys; j++) {
        for (i = 0; i < xs; i++) {
            if (_slot[n].dat[posF]) {
                _vram.bg[(posT & 0xffff)] = _slot[n].dat[posF];
            }
            posT++;
            posF++;
        }
        posT += XSIZE - xs;
        posF += _slot[n].xs - xs;
    }
}

/*
 *----------------------------------------------------------------------------
 * draw sprite from slot
 *----------------------------------------------------------------------------
 */
void vgs2_putSP(unsigned char n, int sx, int sy, int xs, int ys, int dx, int dy)
{
    int i, j;
    int posT;
    int posF;
    if (gclip(n, &sx, &sy, &xs, &ys, &dx, &dy)) {
        return;
    }
    posT = dy * XSIZE + dx;
    posF = sy * _slot[n].xs + sx;
    for (j = 0; j < ys; j++) {
        for (i = 0; i < xs; i++) {
            if (_slot[n].dat[posF]) {
                _vram.sp[(posT & 0xffff)] = _slot[n].dat[posF];
            }
            posT++;
            posF++;
        }
        posT += XSIZE - xs;
        posF += _slot[n].xs - xs;
    }
}

/*
 *----------------------------------------------------------------------------
 * draw sprite from slot (mask)
 *----------------------------------------------------------------------------
 */
void vgs2_putSPM(unsigned char n, int sx, int sy, int xs, int ys, int dx, int dy, unsigned char c)
{
    int i, j;
    int posT;
    int posF;
    if (gclip(n, &sx, &sy, &xs, &ys, &dx, &dy)) {
        return;
    }
    posT = dy * XSIZE + dx;
    posF = sy * _slot[n].xs + sx;
    for (j = 0; j < ys; j++) {
        for (i = 0; i < xs; i++) {
            if (_slot[n].dat[posF]) {
                _vram.sp[(posT & 0xffff)] = c;
            }
            posT++;
            posF++;
        }
        posT += XSIZE - xs;
        posF += _slot[n].xs - xs;
    }
}

/*
 *----------------------------------------------------------------------------
 * draw sprite from slot (half)
 *----------------------------------------------------------------------------
 */
void vgs2_putSPH(unsigned char n, int sx, int sy, int xs, int ys, int dx, int dy)
{
    int i, j;
    int posT;
    int posF;
    xs /= 2;
    ys /= 2;
    if (gclip(n, &sx, &sy, &xs, &ys, &dx, &dy)) {
        return;
    }
    posT = dy * XSIZE + dx;
    posF = sy * _slot[n].xs + sx;
    for (j = 0; j < ys; j++) {
        for (i = 0; i < xs; i++) {
            if (_slot[n].dat[posF]) {
                _vram.sp[(posT & 0xffff)] = _slot[n].dat[posF];
            }
            posT++;
            posF += 2;
        }
        posT += XSIZE - xs;
        posF += (_slot[n].xs) * 2 - xs * 2;
    }
}

/*
 *----------------------------------------------------------------------------
 * draw sprite from slot (mask + half)
 *----------------------------------------------------------------------------
 */
void vgs2_putSPMH(unsigned char n, int sx, int sy, int xs, int ys, int dx, int dy, unsigned char c)
{
    int i, j;
    int posT;
    int posF;
    xs /= 2;
    ys /= 2;
    if (gclip(n, &sx, &sy, &xs, &ys, &dx, &dy)) {
        return;
    }
    posT = dy * XSIZE + dx;
    posF = sy * _slot[n].xs + sx;
    for (j = 0; j < ys; j++) {
        for (i = 0; i < xs; i++) {
            if (_slot[n].dat[posF]) {
                _vram.sp[(posT & 0xffff)] = c;
            }
            posT++;
            posF += 2;
        }
        posT += XSIZE - xs;
        posF += (_slot[n].xs) * 2 - xs * 2;
    }
}

/*
 *----------------------------------------------------------------------------
 * draw sprite from slot (rotate)
 *----------------------------------------------------------------------------
 */
void vgs2_putSPR(unsigned char n, int sx, int sy, int xs, int ys, int dx, int dy, int r)
{
    int x1, y1, x2, y2;
    int scx, scy, dcx, dcy;
    int s;
    int c;
    int p;

    scx = xs >> 1;
    scy = ys >> 1;
    dcx = scx;
    dcy = scy;
    dx += scx;
    dy += scy;
    s = vgs2_sin(r);
    c = vgs2_cos(r);

    p = sy * _slot[n].xs + sx;
    for (y2 = 0; y2 < ys; y2++, p += _slot[n].xs) {
        for (x2 = 0; x2 < xs; x2++) {
            if (_slot[n].dat[p + x2]) {
                x1 = (((x2 - dcx) * c - (y2 - dcy) * s) >> 8) + dx;
                y1 = (((x2 - dcx) * s + (y2 - dcy) * c) >> 8) + dy;
                vgs2_pixelSP(x1, y1, _slot[n].dat[p + x2]);
                vgs2_pixelSP(x1 + 1, y1, _slot[n].dat[p + x2]);
            }
        }
    }
}

/*
 *----------------------------------------------------------------------------
 * draw sprite from slot (expansion)
 *----------------------------------------------------------------------------
 */
void vgs2_putSPE(unsigned char n, int sx, int sy, int xs, int ys, int dx, int dy, int dxs, int dys)
{
    int i, j;
    int posF0;
    int posF1;
    int posF2;

    double ex = (double)xs / dxs;
    double ey = (double)ys / dys;
    double px;
    double py;

    posF0 = sy * _slot[n].xs + sx;

    for (j = 0, py = 0.0; j < dys; j++, py += ey) {
        posF1 = posF0 + ((int)py) * _slot[n].xs;
        for (i = 0, px = 0.0; i < dxs; i++, px += ex) {
            posF2 = posF1 + (int)px;
            if (_slot[n].dat[posF2]) {
                vgs2_pixelSP(dx + i, dy + j, _slot[n].dat[posF2]);
            }
        }
    }
}

/*
 *----------------------------------------------------------------------------
 * draw sprite from slot (rotate + expansion)
 *----------------------------------------------------------------------------
 */
void vgs2_putSPRE(unsigned char n, int sx, int sy, int xs, int ys, int dx, int dy, int r, int dxs, int dys)
{
    int i, j;
    int posF0;
    int posF1;
    int posF2;
    int x1, y1;
    int scx, scy, dcx, dcy;
    int s;
    int c;

    scx = xs >> 1;
    scy = ys >> 1;
    dcx = dxs >> 1;
    dcy = dys >> 1;
    dx += dcx;
    dy += dcy;
    s = vgs2_sin(r);
    c = vgs2_cos(r);

    double ex = (double)xs / dxs;
    double ey = (double)ys / dys;
    double px;
    double py;

    posF0 = sy * _slot[n].xs + sx;

    for (j = 0, py = 0.0; j < dys; j++, py += ey) {
        posF1 = posF0 + ((int)py) * _slot[n].xs;
        for (i = 0, px = 0.0; i < dxs; i++, px += ex) {
            posF2 = posF1 + (int)px;
            if (_slot[n].dat[posF2]) {
                x1 = (((i - dcx) * c - (j - dcy) * s) >> 8) + dx;
                y1 = (((i - dcx) * s + (j - dcy) * c) >> 8) + dy;
                vgs2_pixelSP(x1, y1, _slot[n].dat[posF2]);
                vgs2_pixelSP(x1 + 1, y1, _slot[n].dat[posF2]);
            }
        }
    }
}

/*
 *----------------------------------------------------------------------------
 * get data slot
 *----------------------------------------------------------------------------
 */
const char* vgs2_getdata(unsigned char n, unsigned int* size)
{
    const char* ret;
    int size2;
    int* sp = (int*)size;
    char name[32];
    sprintf(name, "DSLOT%03d.DAT", (int)n);
    if (NULL == sp) {
        sp = &size2;
    }
    ret = getbin(name, sp);
    if (NULL == ret) {
        *sp = 0;
    }
    return ret;
}

/*
 *----------------------------------------------------------------------------
 * set interlace
 *----------------------------------------------------------------------------
 */
void vgs2_interlace(int i)
{
    _interlace = i ? 1 : 0;
}

/*
 *----------------------------------------------------------------------------
 * play BGM
 *----------------------------------------------------------------------------
 */
void vgs2_bplay(unsigned char n)
{
    void* data = _BGM[n];
    size_t size = _BGMSIZE[n];
    if (NULL == data || size < 1) {
        _bstop = 1;
        return;
    }
    vgsdec_load_bgm_from_memory(_psg, data, size);
    vgsdec_set_value(_psg, VGSDEC_REG_RESET, 1);
    vgsdec_set_value(_psg, VGSDEC_REG_SYNTHESIS_BUFFER, 1);
    _bstop = 0;
}

/*
 *----------------------------------------------------------------------------
 * check existing BGM
 *----------------------------------------------------------------------------
 */
int vgs2_bchk(unsigned char n)
{
    if (_BGM[n]) return 1;
    return 0;
}

/*
 *----------------------------------------------------------------------------
 * stop BGM
 *----------------------------------------------------------------------------
 */
void vgs2_bstop()
{
    _bstop = 1;
}

/*
 *----------------------------------------------------------------------------
 * resume playing the BGM
 *----------------------------------------------------------------------------
 */
void vgs2_bresume()
{
    _bstop = 0;
}

/*
 *----------------------------------------------------------------------------
 * fade out the BGM
 *----------------------------------------------------------------------------
 */
void vgs2_bfade(unsigned int hz)
{
    vgsdec_set_value(_psg, VGSDEC_REG_FADEOUT, 1);
}

/*
 *----------------------------------------------------------------------------
 * fade out the BGM (2)
 *----------------------------------------------------------------------------
 */
void vgs2_bfade2()
{
    vgsdec_set_value(_psg, VGSDEC_REG_FADEOUT, 1);
}

/*
 *----------------------------------------------------------------------------
 * chage key
 *----------------------------------------------------------------------------
 */
void vgs2_bkey(int n)
{
    int i;
    for (i = 0; i < 6; i++) {
        if (_bkoff[i]) {
            vgsdec_set_value(_psg, VGSDEC_REG_ADD_KEY_0 + i, 0);
        } else {
            vgsdec_set_value(_psg, VGSDEC_REG_ADD_KEY_0 + i, n);
        }
    }
}

/*
 *----------------------------------------------------------------------------
 * not chage key
 *----------------------------------------------------------------------------
 */
void vgs2_bkoff(int cn, int off)
{
    if (cn < 0 || 5 < cn) return;
    _bkoff[cn] = off;
}

/*
 *----------------------------------------------------------------------------
 * skip notes
 *----------------------------------------------------------------------------
 */
void vgs2_bjump(int sec)
{
    int length = vgsdec_get_value(_psg, VGSDEC_REG_TIME_LENGTH) / 22050;
    while (length < sec) {
        sec -= length;
    }
    vgsdec_set_value(_psg, VGSDEC_REG_TIME, sec * 22050);
}

/*
 *----------------------------------------------------------------------------
 * mute channel
 *----------------------------------------------------------------------------
 */
void vgs2_bmute(int ch)
{
    int v;
    if (ch < 0 || 5 < ch) return;
    v = !vgsdec_get_value(_psg, VGSDEC_REG_MUTE_0 + ch);
    vgsdec_set_value(_psg, VGSDEC_REG_MUTE_0 + ch, v);
}

/*
 *----------------------------------------------------------------------------
 * master volume
 *----------------------------------------------------------------------------
 */
void vgs2_bmvol(int rate)
{
    vgsdec_set_value(_psg, VGSDEC_REG_VOLUME_RATE, rate);
}

/*
 *----------------------------------------------------------------------------
 * channel volume
 *----------------------------------------------------------------------------
 */
void vgs2_bcvol(int ch, int rate)
{
    if (ch < 0 || 5 < ch) return;
    vgsdec_set_value(_psg, VGSDEC_REG_VOLUME_RATE_0 + ch, rate);
}
