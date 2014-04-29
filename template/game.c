/*
 *----------------------------------------------------------------------------
 * Description: Game procedure of {Project}
 *    Platform: Common
 *      Author: {Company}
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
 * Static function
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
	vgs2_pallet(7);
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
	static int x,y;
	static struct InputInf pi;
	struct InputInf ci;
	vgs2_touch(&ci.s,&ci.cx,&ci.cy,&ci.dx,&ci.dy);
	if(pi.s==0 && ci.s!=0) vgs2_eff(0);
	memcpy(&pi,&ci,sizeof(ci));
	x+=ci.dx;
	y+=ci.dy;
	myprint(x,y,"Hello,World!");
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

	vgs2_boxfSP(0,0,XSIZE,YSIZE,1);
	vgs2_boxfSP(30,40,130,160,3);
	vgs2_boxSP(28,38,132,162,2);
	myprint(45,46,"- PAUSE -");
	for(i=0;i<3;i++) {
		if(ci.s && HITCHK(40,60+i*33,80,25,ci.cx-4,ci.cy-4,8,8)) {
			vgs2_boxfSP(40,60+i*33,120,85+i*33,4);
			vgs2_boxSP(40,60+i*33,120,85+i*33,5);
			if(push_flag) {
				push=i+1;
			}
		} else {
			vgs2_boxfSP(40,60+i*33,120,85+i*33,1);
			vgs2_boxSP(40,60+i*33,120,85+i*33,2);
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
