/* (C)2014, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: GameDaddy - kernel and API header
 *    Platform: Common
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *        Date: 7-Sept-2014
 * FileVersion: 1.06
 *----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *----------------------------------------------------------------------------
 * definition
 *----------------------------------------------------------------------------
 */
#define APPNAME "{Project}"

#define XSIZE 160
#define YSIZE 200

#define SAMPLE_RATE	22050
#define SAMPLE_BITS	16
#define SAMPLE_CH	1
#define SAMPLE_BUFS	4410

/*
 *----------------------------------------------------------------------------
 * internal tables
 *----------------------------------------------------------------------------
 */
struct _BINREC {
	char name[16];
	int size;
	char* data;
};

struct _SLOT {
	int xs;
	int ys;
	unsigned char* dat;
};

struct _EFF {
	unsigned int size;
	unsigned short flag;
	unsigned int pos;
	unsigned char* dat;
};

struct _VRAM {
	unsigned char bg[0x10000];
	unsigned char sp[0x10000];
};

struct _TOUCH {
	int s;
	int t;
	int dx;
	int dy;
	int cx;
	int cy;
	int px;
	int py;
};

/*
 *----------------------------------------------------------------------------
 * extern
 *----------------------------------------------------------------------------
 */
extern int BN;
extern struct _BINREC* BR;
extern struct _VRAM _vram;
extern struct _TOUCH _touch;
extern unsigned char _mute;
extern unsigned char _pause;
extern unsigned char _interlace;
extern int _PAL[256];

/*
 *----------------------------------------------------------------------------
 * Internal functions
 *----------------------------------------------------------------------------
 */
void sndbuf(char* buf,size_t size);
void eff_flag(struct _EFF* e,unsigned int f);
void eff_pos(struct _EFF* e,unsigned int f);
void lock();
void unlock();
void lock2();
void unlock2();
void make_pallet();
int gload(unsigned char n,const char* name);
int eload(unsigned char n,const char* name);

/*
 *----------------------------------------------------------------------------
 * Interface functions
 *----------------------------------------------------------------------------
 */
int vgs2_init();
int vgs2_loop();
int vgs2_pause();
void vgs2_term();

/*
 *----------------------------------------------------------------------------
 * API functions
 *----------------------------------------------------------------------------
 */

/* input */
void vgs2_touch(int* s,int* cx,int* cy,int* dx,int* dy);
void vgs2_setPause(unsigned char c);

/* file */
FILE* vgs2_fopen(const char* n,const char* p);

/* math */
#define vgs2_abs(x) (x>=0?(x):-(x))
#define vgs2_sgn(x) (x>=0?(1):(-1))
int vgs2_rad(int x1,int y1,int x2,int y2);
int vgs2_deg(int x1,int y1,int x2,int y2);
int vgs2_deg2rad(int deg);
int vgs2_rad2deg(int rad);
void vgs2_rands();
int vgs2_rand();
int vgs2_sin(int r);
int vgs2_cos(int r);

/* sound effect */
void vgs2_eff(unsigned char n);
void vgs2_effstop(unsigned char n);
void vgs2_effstopA();
void vgs2_setmute(unsigned char n);
unsigned char vgs2_getmute();

/* Set pallet base colour */
/* 1:blue, 2:red, 3:magenta, 4:green, 5:cyan, 6:yellow, 7:white */
void vgs2_pallet(int c);

/* Set 8bit color pallet */
int vgs2_pallet256(unsigned char n);

/* clear screen */
void vgs2_clear(unsigned char c);

/* raster scroll */
void vgs2_scroll(int x,int y);

/* draw pixel */
void vgs2_pixel(unsigned char* p,int x,int y,unsigned char c);
#define vgs2_pixelBG(X,Y,C) vgs2_pixel(_vram.bg,X,Y,C)
#define vgs2_pixelSP(X,Y,C) vgs2_pixel(_vram.sp,X,Y,C)

/* draw line */
void vgs2_line(unsigned char* p,int fx,int fy,int tx,int ty,unsigned char c);
#define vgs2_lineBG(FX,FY,TX,TY,C) vgs2_line(_vram.bg,FX,FY,TX,TY,C)
#define vgs2_lineSP(FX,FY,TX,TY,C) vgs2_line(_vram.sp,FX,FY,TX,TY,C)

/* draw circle */
void vgs2_circle(char*p, int x,int y,int r, unsigned char c);
#define vgs2_circleBG(X,Y,R,C) vgs2_circle(_vram.bg,X,Y,R,C)
#define vgs2_circleSP(X,Y,R,C) vgs2_circle(_vram.sp,X,Y,R,C)

/* draw box */
#define vgs2_boxBG(FX,FY,TX,TY,C) \
	vgs2_line(_vram.bg,FX,FY,TX,FY,C);\
	vgs2_line(_vram.bg,FX,TY,TX,TY,C);\
	vgs2_line(_vram.bg,FX,FY,FX,TY,C);\
	vgs2_line(_vram.bg,TX,FY,TX,TY,C)
#define vgs2_boxSP(FX,FY,TX,TY,C) \
	vgs2_line(_vram.sp,FX,FY,TX,FY,C);\
	vgs2_line(_vram.sp,FX,TY,TX,TY,C);\
	vgs2_line(_vram.sp,FX,FY,FX,TY,C);\
	vgs2_line(_vram.sp,TX,FY,TX,TY,C)

/* draw filled box */
void vgs2_boxf(unsigned char* p,int fx,int fy,int tx,int ty,unsigned char c);
#define vgs2_boxfBG(FX,FY,TX,TY,C) vgs2_boxf(_vram.bg,FX,FY,TX,TY,C)
#define vgs2_boxfSP(FX,FY,TX,TY,C) vgs2_boxf(_vram.sp,FX,FY,TX,TY,C)

/* background and sprite */
void vgs2_putBG(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy);
void vgs2_putBG2(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy);
void vgs2_putSP(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy);
void vgs2_putSPM(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy,unsigned char c);
void vgs2_putSPH(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy);
void vgs2_putSPMH(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy,unsigned char c);
void vgs2_putSPR(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy,int r);
void vgs2_putSPE(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy,int dxs,int dys);
void vgs2_putSPRE(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy,int r,int dxs,int dys);

/* get data */
const char* vgs2_getdata(unsigned char n,unsigned int* size);

/* set interlace */
void vgs2_interlace(int i);

#ifdef __cplusplus
};
#endif
