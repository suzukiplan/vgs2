/*
 *----------------------------------------------------------------------------
 * Description: Game procedure of Env
 *    Platform: Common
 *      Author: suzukiplan
 * FileVersion: 1.00
 *----------------------------------------------------------------------------
 */
#include "vgs2.h"

/*
 *----------------------------------------------------------------------------
 * Macro definition
 *----------------------------------------------------------------------------
 */
#define HITCHK(X1,Y1,XS1,YS1,X2,Y2,XS2,YS2) (X1<X2+XS2 && X2<X1+XS1 && Y1<Y2+YS2 && Y2<Y1+YS1)

/*
 *----------------------------------------------------------------------------
 * Structure definition
 *----------------------------------------------------------------------------
 */
struct InputInf {
	int s;
	int cx;
	int cy;
	int dx;
	int dy;
};

/*
 *----------------------------------------------------------------------------
 * Static variables
 *----------------------------------------------------------------------------
 */
static unsigned char* _kanji;

/*
 *----------------------------------------------------------------------------
 * Static functions
 *----------------------------------------------------------------------------
 */
static void myprint(int x,int y,const char* msg,...);

/*
 *----------------------------------------------------------------------------
 * Initialize procedure
 * Return value: 0 = succeed, not 0 = failed
 *----------------------------------------------------------------------------
 */
int vgs2_init()
{
	int i;
	vgs2_pallet256(0);
	vgs2_interlace(0);
	for(i=20;i<160;i+=2) vgs2_pixelBG(20,i,58);
	for(i=20;i<160;i+=2) vgs2_pixelBG(50,i,58);
	for(i=20;i<160;i+=2) vgs2_pixelBG(70,i,58);
	for(i=20;i<160;i+=2) vgs2_pixelBG(110,i,58);
	for(i=20;i<160;i+=2) vgs2_pixelBG(140,i,58);
	vgs2_lineBG(20,160,20,170, 111);
	vgs2_lineBG(110,160,110,170, 111);
	vgs2_lineBG(20,160,50,40, 47);
	vgs2_lineBG(50,40, 70,70, 47);
	vgs2_lineBG(70,70,110,70, 47);
	vgs2_lineBG(110,70,140,160,47);
	vgs2_lineBG(0,160,240,160, 73);
	vgs2_scroll(0,10);
	return 0;
}

/*
 *----------------------------------------------------------------------------
 * Main loop
 * Return value: 0 = continue, not 0 = finish
 *----------------------------------------------------------------------------
 */
int vgs2_loop()
{
	myprint(4,4,"Envelope");
	myprint(27,20,"AT");
	myprint(53,20,"DT");
	myprint(82,20,"ST");
	myprint(117,20,"RT");
	myprint(13,182,"ON");
	myprint(99,182,"OFF");
	return 0;
}

/*
 *----------------------------------------------------------------------------
 * Main loop (when paused)
 * Return value: 0 = continue, not 0 = finish
 *----------------------------------------------------------------------------
 */
int vgs2_pause()
{
	static struct InputInf pi;
	struct InputInf ci;
	int push_flag=0;
	int push=0;
	int i;

	vgs2_touch(&ci.s,&ci.cx,&ci.cy,&ci.dx,&ci.dy);
	if(0==ci.s && 0!=pi.s) {
		push_flag=1;
		ci.s=1;
		ci.cx=pi.cx;
		ci.cy=pi.cy;
	}
	memcpy(&pi,&ci,sizeof(pi));

	vgs2_boxfSP(0,0,XSIZE,YSIZE,5);

	vgs2_boxfSP(30,40,130,160,12);
	vgs2_boxSP(28,38,132,162,105);
	myprint(45,46,"- PAUSE -");
	for(i=0;i<3;i++) {
		if(ci.s && HITCHK(40,60+i*33,80,25,ci.cx-4,ci.cy-4,8,8)) {
			vgs2_boxfSP(40,60+i*33,120,85+i*33,60);
			vgs2_boxSP(40,60+i*33,120,85+i*33,111);
			if(push_flag) {
				push=i+1;
			}
		} else {
			vgs2_boxfSP(40,60+i*33,120,85+i*33,8);
			vgs2_boxSP(40,60+i*33,120,85+i*33,107);
		}
	}
	myprint(57,70,"RESUME");
	myprint(61,103,"RESET");
	myprint(65,136,"EXIT");
	if(push) {
		memset(&pi,0,sizeof(pi));
		switch(push) {
			case 1:
				break;
			case 2:
				/* TODO: implement reset proceure */
				break;
			case 3:
				return 1;
		}
		vgs2_setPause(0);
	}
	return 0;
}

/*
 *----------------------------------------------------------------------------
 * Terminate procedure
 *----------------------------------------------------------------------------
 */
void vgs2_term()
{
}

/*
 *----------------------------------------------------------------------------
 * print text (sprite)
 *----------------------------------------------------------------------------
 */
static void myprint(int x,int y,const char* msg,...)
{
	char buf[256];
	int i;
	int c;
	int d;
	va_list args;

	va_start(args,msg);
	vsprintf(buf,msg,args);
	va_end(args);

	for(i=0;'\0'!=(c=(int)buf[i]);i++,x+=8) {
		c-=0x20;
		c&=0x7f;
		d=c>>5;
		vgs2_putSP(0,(c-(d<<5))<<3,d<<3,8,8,x,y);
	}
}
