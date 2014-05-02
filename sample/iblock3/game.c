/* (C)2014, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: Invader Block 3
 *    Platform: Common
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *        Date: 6 Mar 2014
 * FileVersion: 1.00
 *----------------------------------------------------------------------------
 */
#include "vgs2.h"

/* hitcheck(macro) */
#define HITCHK(X1,Y1,XS1,YS1,X2,Y2,XS2,YS2) (X1<X2+XS2 && X2<X1+XS1 && Y1<Y2+YS2 && Y2<Y1+YS1)

/* effect number */
#define EFF_COIN	0
#define EFF_TAMA	1
#define EFF_MOVE0	2
#define EFF_MOVE1	3
#define EFF_MOVE2	4
#define EFF_MOVE3	5
#define EFF_KAMI	6
#define EFF_DEAD	7
#define EFF_HIT		8
#define EFF_DAMAGE	9
#define EFF_SELECT	10
#define EFF_START	11

/* MAX VALUE */
#define MAX_DOT		256
#define MAX_ENEM	37
#define MAX_COIN	MAX_ENEM
#define MAX_MSG		1
#define MAX_MSG2	16
#define MAX_EXP		16

struct Player {
	int dead;
	int x;
	int y;
};

struct Ball {
	int x;
	int y;
	int fx;
	int fy;
	int vx;
	int vy;
	int wt;
	int iv;
};

struct Enemy {
	int flag;
	int type;
	int hp;
	int t;
	int x;
	int y;
	int xs;
	int an;
};

struct Dot {
	int flag;
	int x;
	int y;
	int fx;
	int fy;
	int vx;
	int vy;
};

struct Message {
	int flag;
	char tx[16];
	int x;
	int y;
	int fx;
	int fy;
	int vx;
	int vy;
};

struct Explode {
	int flag;
	int type;
	int x;
	int y;
};

struct Coin {
	int flag;
	int an;
	int x;
	int y;
	int fy;
	int vy;
};

/* Game table */
struct GameTable {
	unsigned short cnt;
	struct Player p;
	struct Ball b;
	struct Dot d[MAX_DOT];
	int di;
	struct Message m[MAX_MSG];
	int mi;
	struct Message m2[MAX_MSG2];
	int m2i;
	struct Explode x[MAX_EXP];
	int xi;
	struct Coin c[MAX_COIN];
	int ci;
	struct Enemy e[MAX_ENEM];
	int en;		/* left */
	int et[4];	/* left group */
	int ew;		/* move wait */
	int ec;		/* move group */
	int ev;		/* move vector */
	int pts;
	int hts;
	int mhts;
} TBL;

/* input information */
struct InputInf {
	int s;
	int cx;
	int cy;
	int dx;
	int dy;
};

/* system table */
struct SystemTable {
	int flag;
	int round;
	int coin;
	unsigned int sc;
	unsigned int hi;
} ST;

/* utility functions */
static void myprintBG(int x,int y,const char* msg,...);
static void myprintSP(int x,int y,const char* msg,...);
static void myprintSP1(int x,int y,const char* msg,...);
static void myprintSBG(int x,int y,const char* msg,...);
static void myprintSSP(int x,int y,const char* msg,...);
static void myprintSSPB(int x,int y,const char* msg,...);

/* game functions */
static int game_sound();
static int game_menu0();
static int game_menu();
static int game_main0();
static int game_main();
static int game_clear();
static int (*gfunc[6])() = {
	 game_sound
	,game_menu0
	,game_menu
	,game_main0
	,game_main
	,game_clear
};

/* game sub functions */
static void score();
static void dot_add(int x,int y,int vx,int vy);
static void msg_add(int x,int y,int vx,int vy,const char* tx);
static void msg2_add(int x,int y,int vx,int vy,const char* tx);
static void exp_add(int x,int y,int type);
static void coin_add(int x,int y);

/*
 *----------------------------------------------------------------------------
 * Initialize procedure
 * Return value: 0 = succeed, not 0 = failed
 *----------------------------------------------------------------------------
 */
int vgs2_init()
{
	FILE* fp;
	fp=vgs2_fopen("SCORE.DAT","rb");
	if(fp) {
		fread(&ST.hi,sizeof(ST.hi),1,fp);
		fclose(fp);
	}

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
	return gfunc[ST.flag]();
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
	myprintSP(45,46,"- PAUSE -");
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
	myprintSP(57,70,"RESUME");
	myprintSP(61,103,"RESET");
	myprintSP(65,136,"EXIT");
	if(push) {
		memset(&pi,0,sizeof(pi));
		switch(push) {
			case 1:
				break;
			case 2:
				if(ST.flag!=0) {
					ST.flag=1;
					vgs2_clear(0);
				}
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
	FILE* fp;
	fp=vgs2_fopen("SCORE.DAT","wb");
	if(fp) {
		fwrite(&ST.hi,sizeof(ST.hi),1,fp);
		fclose(fp);
	}
}

/*
 *----------------------------------------------------------------------------
 * print text (BG)
 *----------------------------------------------------------------------------
 */
static void myprintBG(int x,int y,const char* msg,...)
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
		vgs2_putBG2(0,(c-(d<<5))<<3,d<<3,8,8,x,y);
	}
}

/*
 *----------------------------------------------------------------------------
 * print text (sprite)
 *----------------------------------------------------------------------------
 */
static void myprintSP(int x,int y,const char* msg,...)
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

/*
 *----------------------------------------------------------------------------
 * print text mask 1 (sprite)
 *----------------------------------------------------------------------------
 */
static void myprintSP1(int x,int y,const char* msg,...)
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
		vgs2_putSPM(0,(c-(d<<5))<<3,d<<3,8,8,x,y,0);
	}
}

/*
 *----------------------------------------------------------------------------
 * print small text (BG)
 *----------------------------------------------------------------------------
 */
static void myprintSBG(int x,int y,const char* msg,...)
{
	char buf[64];
	int i;
	char c;
	va_list args;

	va_start(args,msg);
	vsprintf(buf,msg,args);
	va_end(args);

	for(i=0;'\0'!=(c=buf[i]);i++) {
		if('0'<=c && c<='9') {
			c-='0';
			vgs2_putBG2(0,c*4,64,4,8,x+i*4,y);
		}
		else if('A'<=c && c<='Z') {
			c-='A';
			vgs2_putBG2(0,40+c*4,64,4,8,x+i*4,y);
		}
		else if('.'==c) {
			vgs2_putBG2(0,144,64,4,8,x+i*4,y);
		}
	}
}

/*
 *----------------------------------------------------------------------------
 * print small text (sprite)
 *----------------------------------------------------------------------------
 */
static void myprintSSP(int x,int y,const char* msg,...)
{
	char buf[64];
	int i;
	char c;
	va_list args;

	va_start(args,msg);
	vsprintf(buf,msg,args);
	va_end(args);

	for(i=0;'\0'!=(c=buf[i]);i++) {
		if('0'<=c && c<='9') {
			c-='0';
			vgs2_putSP(0,c*4,64,4,8,x+i*4,y);
		}
		else if('A'<=c && c<='Z') {
			c-='A';
			vgs2_putSP(0,40+c*4,64,4,8,x+i*4,y);
		}
		else if('.'==c) {
			vgs2_putSP(0,144,64,4,8,x+i*4,y);
		}
	}
}

/*
 *----------------------------------------------------------------------------
 * print small text / bold (sprite)
 *----------------------------------------------------------------------------
 */
static void myprintSSPB(int x,int y,const char* msg,...)
{
	char buf[64];
	int i;
	char c;
	va_list args;

	va_start(args,msg);
	vsprintf(buf,msg,args);
	va_end(args);

	for(i=0;'\0'!=(c=buf[i]);i++) {
		if('0'<=c && c<='9') {
			c-='0';
			vgs2_putSPM(0,c*4,64,4,8,x+i*4+1,y+1,2);
			vgs2_putSP(0,c*4,64,4,8,x+i*4,y);
		}
		else if('A'<=c && c<='Z') {
			c-='A';
			vgs2_putSPM(0,40+c*4,64,4,8,x+i*4+1,y+1,2);
			vgs2_putSP(0,40+c*4,64,4,8,x+i*4,y);
		}
		else if('.'==c) {
			vgs2_putSPM(0,144,64,4,8,x+i*4+1,y+1,2);
			vgs2_putSP(0,144,64,4,8,x+i*4,y);
		}
	}
}

/*
 *----------------------------------------------------------------------------
 * game function: select sound mode
 *----------------------------------------------------------------------------
 */
static int game_sound()
{
	static int logo=0;
	static struct InputInf pi;
	struct InputInf ci;
	int push_flag=0;

	if(logo<120) {
		if(0==logo) {
			vgs2_putBG(0,0,112,120,40,20,80);
		}
		logo++;
		if(120==logo) vgs2_clear(0);
		return 0;
	}

	vgs2_touch(&ci.s,&ci.cx,&ci.cy,&ci.dx,&ci.dy);
	if(0==ci.s && 0!=pi.s) {
		push_flag=1;
		ci.cx=pi.cx;
		ci.cy=pi.cy;
	}
	memcpy(&pi,&ci,sizeof(pi));

	myprintSP(33,70,"SELECT SOUND");
	vgs2_boxSP(28,88,72,122,1);
	vgs2_boxSP(30,90,70,120,2);
	vgs2_boxSP(88,88,132,122,1);
	vgs2_boxSP(90,90,130,120,2);

	if(ci.s || push_flag) {
		if(HITCHK(28,88,44,34,ci.cx-4,ci.cy-4,8,8)) {
			vgs2_boxfSP(31,91,69,119,3);
			if(push_flag) {
				vgs2_setmute(1);
				ST.flag++;
			}
		}
		if(HITCHK(88,88,44,34,ci.cx-4,ci.cy-4,8,8)) {
			vgs2_boxfSP(91,91,129,119,3);
			if(push_flag) {
				vgs2_setmute(0);
				ST.flag++;
			}
		}
	}
	myprintSP(39,102,"OFF");
	myprintSP(103,102,"ON");
	return 0;
}

/*
 *----------------------------------------------------------------------------
 * game function: initialize menu
 *----------------------------------------------------------------------------
 */
static int game_menu0()
{
	int i;
	vgs2_clear(0);
	for(i=0;i<160;i+=2) {
		vgs2_lineBG(i,33,i,90,1);
	}
	for(i=0;i<4;i++) {
		vgs2_lineBG(0,30+i,160,30+i,5-i);
		vgs2_lineBG(0,93-i,160,93-i,5-i);
	}
	vgs2_putBG2(0, 0,72, 144,40, 8,40);
	myprintSBG(2,22,"GAMEDADDY EDITION");
	myprintBG(4,190,"(C)2014,SUZUKIPLAN.");
	score();
	ST.flag++;
	return 0;
}

/*
 *----------------------------------------------------------------------------
 * game function: menu
 *----------------------------------------------------------------------------
 */
static int game_menu()
{
	static struct InputInf pi;
	static unsigned short cnt;
	struct InputInf ci;
	int push_flag=0;

	cnt++;

	vgs2_touch(&ci.s,&ci.cx,&ci.cy,&ci.dx,&ci.dy);
	if(0==ci.s && 0!=pi.s) {
		push_flag=1;
		ci.s=1;
		ci.cx=pi.cx;
		ci.cy=pi.cy;
	}
	memcpy(&pi,&ci,sizeof(pi));

	if(cnt%40<20) {
		myprintSP(24,134,"TOUCH TO START");
	}
	score();

	if(push_flag) {
		vgs2_eff(EFF_START);
		ST.flag++;
		ST.coin=0;
		ST.sc=0;
		ST.round=0;
		memset(&pi,0,sizeof(pi));
	}

	return 0;
}

/*
 *----------------------------------------------------------------------------
 * game function: initialize main
 *----------------------------------------------------------------------------
 */
static int game_main0()
{
	int i,j,n,an;
	int y;
	/* variables */
	memset(&TBL,0,sizeof(TBL));
	TBL.p.x=68;
	TBL.p.y=185;
	TBL.b.wt=120;
	TBL.b.iv=1;
	TBL.pts=1;
	TBL.en=MAX_ENEM;
	TBL.ew=TBL.en;
	TBL.ev=4;
	n=0;
	an=0;
	TBL.et[3]=13;
	y=20;
	if(6<=ST.round) {
		y+=(ST.round-5)<<2;
		if(64<y) y=64;
	}
	for(j=0;j<13;j++,n++) {
		TBL.e[n].flag=1;
		TBL.e[n].type=0;
		TBL.e[n].xs=8;
		TBL.e[n].hp=1;
		TBL.e[n].t=3;
		TBL.e[n].x=8+j*9;
		TBL.e[n].y=y;
		TBL.e[n].an=an;
		an=1-an;
	}
	TBL.et[2]=7;
	TBL.et[1]=7;
	y+=10;
	for(i=0;i<2;i++) {
		for(j=0;j<7;j++,n++) {
			TBL.e[n].flag=1;
			TBL.e[n].type=1;
			TBL.e[n].xs=16;
			TBL.e[n].hp=1;
			TBL.e[n].t=2-i;
			TBL.e[n].x=4+j*18;
			TBL.e[n].y=y+i*10;
			TBL.e[n].an=an;
			an=1-an;
		}
	}
	TBL.et[0]=10;
	y+=20;
	for(i=0;i<2;i++) {
		for(j=0;j<5;j++,n++) {
			TBL.e[n].flag=1;
			TBL.e[n].type=2;
			TBL.e[n].xs=24;
			TBL.e[n].hp=1;
			TBL.e[n].t=0;
			TBL.e[n].x=4+j*25;
			TBL.e[n].y=y+i*10;
			TBL.e[n].an=an;
			an=1-an;
		}
	}
	if(1<=ST.round) {
		for(i=32;i<37;i++) {
			TBL.e[i].hp++;
		}
	}
	if(2<=ST.round) {
		for(i=27;i<32;i++) {
			TBL.e[i].hp++;
		}
	}
	if(3<=ST.round) {
		for(i=20;i<27;i++) {
			TBL.e[i].hp++;
		}
	}
	if(4<=ST.round) {
		for(i=13;i<20;i++) {
			TBL.e[i].hp++;
		}
	}
	if(5<=ST.round) {
		for(i=0;i<13;i++) {
			TBL.e[i].hp++;
		}
	}

	vgs2_rands();
	/* graphic */
	vgs2_clear(0);
	vgs2_boxBG(0,10,159,200,3);
	vgs2_boxBG(1,11,158,200,5);
	vgs2_boxBG(2,12,157,200,7);
	score();
	ST.flag++;
	return 0;
}

/*
 *----------------------------------------------------------------------------
 * game function: main
 *----------------------------------------------------------------------------
 */
static int game_main()
{
	char buf[16];
	struct InputInf ci;
	int i,j;
	FILE* fp;

	TBL.cnt++;

	/* input */
	vgs2_touch(&ci.s,&ci.cx,&ci.cy,&ci.dx,&ci.dy);

	/* draw dot effects */
	dot_add(vgs2_rand()%154+3,vgs2_rand()%170+13,0,vgs2_rand()%5*50+100);
	for(i=0;i<MAX_DOT;i++) {
		if(TBL.d[i].flag) {
			vgs2_pixelSP(TBL.d[i].x,TBL.d[i].y,1+(TBL.d[i].flag>>3));
			TBL.d[i].flag--;
			if(TBL.d[i].flag) {
				TBL.d[i].fx+=TBL.d[i].vx;
				TBL.d[i].fy+=TBL.d[i].vy;
				TBL.d[i].x=TBL.d[i].fx>>8;
				TBL.d[i].y=TBL.d[i].fy>>8;
				if(TBL.d[i].x<3 || 156<TBL.d[i].x
				|| TBL.d[i].y<13|| 199<TBL.d[i].y) {
					TBL.d[i].flag=0;
				}
			}
		}
	}

	/* player */
	if(TBL.p.dead==0 && ci.s) {
		if(ci.cx<3) ci.cx=3;
		else if(156<ci.cx) ci.cx=156;
		vgs2_lineSP(ci.cx,13,ci.cx,199,3);
		if(TBL.p.x+12<ci.cx) {
			TBL.b.iv=-1;
			TBL.p.x+=4;
			if(ci.cx<TBL.p.x+12) TBL.p.x=ci.cx-12;
			if(134<TBL.p.x) TBL.p.x=134;
		} else if(ci.cx<TBL.p.x+12) {
			TBL.b.iv=1;
			TBL.p.x-=4;
			if(TBL.p.x+12<ci.cx) TBL.p.x=ci.cx-12;
			if(TBL.p.x<3) TBL.p.x=3;
		}
	}
	if(TBL.p.dead) {
		if(1==TBL.p.dead) {
			vgs2_eff(EFF_DEAD);
		}
		if(TBL.p.dead<60) {
			vgs2_putSP(0,48+TBL.p.dead%6/3*24,24,24,8,TBL.p.x,TBL.p.y);
		}
		TBL.p.dead++;
	} else {
		vgs2_putSP(0,24,24,24,8,TBL.p.x,TBL.p.y);
	}

	/* ball */
	if(TBL.b.wt) {
		TBL.b.x=TBL.p.x+10;
		TBL.b.y=TBL.p.y-4;
		TBL.b.fx=TBL.b.x<<8;
		TBL.b.fy=TBL.b.y<<8;
		TBL.b.wt--;
		if(0==TBL.b.wt) {
			TBL.b.vx=TBL.b.iv<<8;
			TBL.b.vy=-256;
		}
	} else if(TBL.b.y<200) {
		TBL.b.fx+=TBL.b.vx;
		TBL.b.fy+=TBL.b.vy;
		TBL.b.x=TBL.b.fx>>8;
		TBL.b.y=TBL.b.fy>>8;
		if(TBL.b.vx<0) {
			if(TBL.b.x<3) {
				TBL.b.x=3;
				TBL.b.vx=-TBL.b.vx;
				for(i=0;i<8;i++) {
					dot_add(TBL.b.x+i/2,TBL.b.y+4,vgs2_rand()%512,vgs2_rand()%512-256);
				}
				vgs2_eff(EFF_TAMA);
			}
		} else {
			if(TBL.b.x>153) {
				TBL.b.x=153;
				TBL.b.vx=-TBL.b.vx;
				for(i=0;i<8;i++) {
					dot_add(TBL.b.x+i/2,TBL.b.y,-vgs2_rand()%512,vgs2_rand()%512-256);
				}
				vgs2_eff(EFF_TAMA);
			}
		}
		if(TBL.b.vy<0) {
			if(TBL.b.y<13) {
				TBL.b.y=13;
				TBL.b.vy=-TBL.b.vy;
				for(i=0;i<8;i++) {
					dot_add(TBL.b.x,TBL.b.y+i/2,vgs2_rand()%512-256,vgs2_rand()%512);
				}
				vgs2_eff(EFF_TAMA);
			}
		} else {
			if(0==TBL.p.dead && HITCHK(TBL.p.x,TBL.p.y,24,4,TBL.b.x,TBL.b.y,4,4)) {
				TBL.hts=0;
				TBL.b.vy=-(TBL.b.vy+23);
				i=TBL.b.x-TBL.p.x;
				if(i<2) {
					TBL.b.vx-=200;
				} else if(i<4) {
					TBL.b.vx-=150;
				} else if(i<6) {
					TBL.b.vx-=100;
				} else if(i<10) {
					TBL.b.vx-=50;
				} else if(22<i) {
					TBL.b.vx+=200;
				} else if(20<i) {
					TBL.b.vx+=150;
				} else if(18<i) {
					TBL.b.vx+=100;
				} else if(16<i) {
					TBL.b.vx+=50;
				}
				vgs2_eff(EFF_TAMA);
				for(i=0;i<8;i++) {
					dot_add(TBL.b.x+4,TBL.b.y+i/2,vgs2_rand()%512-256,-vgs2_rand()%512);
				}
			}
		}
	} else if(0==TBL.p.dead) {
		TBL.p.dead=1;
	}
	vgs2_putSP(0,8,32,4,4,TBL.b.x,TBL.b.y);
	if(0==TBL.b.wt && 0==(TBL.cnt&3)) {
		dot_add(TBL.b.x+1,TBL.b.y+1,0,0);
		dot_add(TBL.b.x+2,TBL.b.y+1,0,0);
		dot_add(TBL.b.x+1,TBL.b.y+2,0,0);
		dot_add(TBL.b.x+2,TBL.b.y+2,0,0);
	}

	/* explode */
	for(i=0;i<MAX_EXP;i++) {
		if(TBL.x[i].flag) {
			switch(TBL.x[i].type) {
				case 0:
					vgs2_putSP(0,24+((TBL.x[i].flag>>4)<<3),40,8,8,TBL.x[i].x,TBL.x[i].y);
					break;
				case 1:
					vgs2_putSP(0,40+((TBL.x[i].flag>>4)<<4),40,16,8,TBL.x[i].x,TBL.x[i].y);
					break;
				case 2:
					vgs2_putSP(0,72+((TBL.x[i].flag>>4)*24),40,24,8,TBL.x[i].x,TBL.x[i].y);
					break;
			}
			TBL.x[i].flag++;
			TBL.x[i].flag&=31;
		}
	}

	/* enemy move */
	if(TBL.en) {
		TBL.ew--;
		if(0==TBL.ew) {
			TBL.ew=TBL.en;
			while(0==TBL.et[TBL.ec]) {
				TBL.ec++;
				TBL.ec&=3;
			}
			vgs2_eff(EFF_MOVE0+TBL.ec);
			for(i=0;i<MAX_ENEM;i++) {
				if(TBL.ec==TBL.e[i].t && TBL.e[i].flag) {
					TBL.e[i].an=1-TBL.e[i].an;
					TBL.e[i].x+=TBL.ev;
					if((0<TBL.ev && TBL.e[i].x+TBL.e[i].xs>156)
					|| (TBL.ev<0 && TBL.e[i].x<3)) {
						break;
					}
				}
			}
			if(i!=MAX_ENEM) {
				for(;TBL.ec==TBL.e[i].t;i--) {
					TBL.e[i].x-=TBL.ev;
				}
				TBL.ev=-TBL.ev;
				for(i=0;i<MAX_ENEM;i++) {
					if(TBL.e[i].flag) {
						TBL.e[i].y+=8;
					}
				}
				TBL.ec=0;
			} else {
				TBL.ec++;
				TBL.ec&=3;
			}
		}
	}

	/* enemy put */
	for(i=0;i<MAX_ENEM;i++) {
		if(TBL.e[i].flag) {
			j=0;
			if(TBL.b.y<200) {
				if(HITCHK(TBL.e[i].x,TBL.e[i].y,1,8,TBL.b.x,TBL.b.y,4,4)
				|| HITCHK(TBL.e[i].x+TBL.e[i].xs-1,TBL.e[i].y,1,8,TBL.b.x,TBL.b.y,4,4)) {
					TBL.b.vx=-TBL.b.vx;
					j=1;
				}else if(HITCHK(TBL.e[i].x,TBL.e[i].y,TBL.e[i].xs,8,TBL.b.x,TBL.b.y,4,4)) {
					TBL.b.vy=-TBL.b.vy;
					j=1;
				}
			}
			if(j) {
				TBL.e[i].hp--;
				if(0==TBL.e[i].hp) {
					for(j=0;j<8+TBL.e[i].type*8;j++) {
						dot_add(TBL.e[i].x+vgs2_rand()%TBL.e[i].xs,TBL.e[i].y+vgs2_rand()%8,vgs2_rand()%256-128,vgs2_rand()%256-128);
					}
					exp_add(TBL.e[i].x,TBL.e[i].y,TBL.e[i].type);
					TBL.e[i].flag=0;
					TBL.en--;
					TBL.et[TBL.e[i].t]--;
					TBL.hts++;
					ST.sc+=TBL.pts;
					coin_add(TBL.e[i].x-4+(TBL.e[i].type<<2),TBL.e[i].y-4);
					if(TBL.mhts<TBL.hts) TBL.mhts=TBL.hts;
					sprintf(buf,"%dHIT",TBL.hts);
					msg_add(((TBL.e[i].xs-((int)strlen(buf)<<2))>>1)+TBL.e[i].x,TBL.e[i].y,0,-32,buf);
					vgs2_eff(EFF_HIT);
				} else {
					vgs2_eff(EFF_DAMAGE);
				}
				continue;
			}
			switch(TBL.e[i].type) {
				case 0:
					vgs2_putSP(0,24+(TBL.e[i].an<<3)+96*(TBL.e[i].hp-1),32,8,8,TBL.e[i].x,TBL.e[i].y);
					break;
				case 1:
					vgs2_putSP(0,40+(TBL.e[i].an<<4)+96*(TBL.e[i].hp-1),32,16,8,TBL.e[i].x,TBL.e[i].y);
					break;
				case 2:
					vgs2_putSP(0,72+(TBL.e[i].an*24)+96*(TBL.e[i].hp-1),32,24,8,TBL.e[i].x,TBL.e[i].y);
					break;
			}
			if(0==TBL.p.dead
			&& HITCHK(TBL.e[i].x,TBL.e[i].y,TBL.e[i].xs,8,TBL.p.x,TBL.p.y,24,8)) {
				TBL.p.dead=1;
			}
		}
	}

	/* coin */
	for(i=0;i<MAX_COIN;i++) {
		if(TBL.c[i].flag) {
			if(200<TBL.c[i].y) {
				TBL.c[i].flag=0;
			}
			if(0==TBL.p.dead && HITCHK(TBL.c[i].x,TBL.c[i].y,16,16,TBL.p.x,TBL.p.y,24,8)) {
				TBL.c[i].flag=0;
				ST.coin++;
				j=10+TBL.mhts;
				if(256<j) j=256;
				ST.sc+=j;
				sprintf(buf,"%d0PTS",j);
				msg2_add(TBL.c[i].x-((16-(((int)strlen(buf))<<2))>>1),TBL.c[i].y+8,0,-128,buf);
				vgs2_eff(EFF_COIN);
			}
			vgs2_putSP(0,(TBL.c[i].an>>2)<<4,48,16,16,TBL.c[i].x,TBL.c[i].y);
			TBL.c[i].an++;
			TBL.c[i].an&=15;
			TBL.c[i].fy+=TBL.c[i].vy;
			if(TBL.c[i].vy<512) {
				TBL.c[i].vy+=32;
			}
			TBL.c[i].y=TBL.c[i].fy>>8;
		}
	}

	/* message */
	for(i=0;i<MAX_MSG;i++) {
		if(TBL.m[i].flag) {
			myprintSSPB(TBL.m[i].x,TBL.m[i].y,"%s",TBL.m[i].tx);
			TBL.m[i].flag--;
			TBL.m[i].fx+=TBL.m[i].vx;
			TBL.m[i].fy+=TBL.m[i].vy;
			TBL.m[i].x=TBL.m[i].fx>>8;
			TBL.m[i].y=TBL.m[i].fy>>8;
		}
	}
	for(i=0;i<MAX_MSG2;i++) {
		if(TBL.m2[i].flag) {
			myprintSSPB(TBL.m2[i].x,TBL.m2[i].y,"%s",TBL.m2[i].tx);
			TBL.m2[i].flag--;
			TBL.m2[i].fx+=TBL.m2[i].vx;
			TBL.m2[i].fy+=TBL.m2[i].vy;
			TBL.m2[i].x=TBL.m2[i].fx>>8;
			TBL.m2[i].y=TBL.m2[i].fy>>8;
		}
	}

	/* clear */
	if(0==TBL.en && 0==TBL.p.dead) {
		for(i=0;i<MAX_COIN;i++) if(TBL.c[i].flag) break;
		if(i==MAX_COIN) {
			ST.flag++;
			TBL.cnt=0;
		}
	}

	/* info */
	score();
	myprintSSP(4,192,"%dHIT",TBL.mhts);
	myprintSSP(128,192,"%3d0PTS",TBL.pts);
	if(20<TBL.p.dead%40) {
		myprintSP(44,100,"GAME OVER");
		if(170<TBL.p.dead) {
			if(ST.sc==ST.hi) {
				fp=vgs2_fopen("SCORE.DAT","wb");
				if(fp) {
					fwrite(&ST.hi,sizeof(ST.hi),1,fp);
					fclose(fp);
				}
			}
			ST.flag=1;
		}
	}
	return 0;
}

/*
 *----------------------------------------------------------------------------
 * game function: round clear
 *----------------------------------------------------------------------------
 */
static int game_clear()
{
	static int bns;
	static int d32;
	char buf[40];
	if(TBL.cnt==0) {
		ST.round++;
		if(99<ST.round) ST.round=99;
		if(ST.round<10) {
			myprintBG(28,90,"ROUND %d CLEAR",ST.round);
		} else {
			myprintBG(24,90,"ROUND %d CLEAR",ST.round);
		}
		bns=ST.round*128+ST.coin;
		if(99999<bns) bns=99999;
		d32=bns>>5;
	}
	sprintf(buf,"BONUS %d0PTS",bns);
	myprintSP((160-(strlen(buf)<<3))>>1,110,buf);

	if(120<TBL.cnt) {
		if(TBL.cnt%2) {
			if(bns) {
				if(bns<d32) {
					ST.sc+=bns;
					bns=0;
				} else {
					ST.sc+=d32;
					bns-=d32;
				}
			}
		}
	}

	score();
	TBL.cnt++;
	if(300<=TBL.cnt) {
		ST.flag-=2;
	}
	return 0;
}

/*
 *----------------------------------------------------------------------------
 * draw score
 *----------------------------------------------------------------------------
 */
static void score()
{
	if(99999999<ST.sc) ST.sc=99999999;
	if(ST.hi<ST.sc) ST.hi=ST.sc;
	TBL.pts=ST.coin>>2;
	TBL.pts++;
	if(256<TBL.pts) TBL.pts=256;
	myprintSSP(12,2,"SCORE %8d0  HISCORE %8d0",ST.sc,ST.hi);
}

/*
 *----------------------------------------------------------------------------
 * add dot
 *----------------------------------------------------------------------------
 */
static void dot_add(int x,int y,int vx,int vy)
{
	TBL.d[TBL.di].flag=55;
	TBL.d[TBL.di].x=x;
	TBL.d[TBL.di].y=y;
	TBL.d[TBL.di].fx=x<<8;
	TBL.d[TBL.di].fy=y<<8;
	TBL.d[TBL.di].vx=vx;
	TBL.d[TBL.di].vy=vy;
	TBL.di++;
	TBL.di&=MAX_DOT-1;
}

/*
 *----------------------------------------------------------------------------
 * add message
 *----------------------------------------------------------------------------
 */
static void msg_add(int x,int y,int vx,int vy,const char* tx)
{
	TBL.m[TBL.mi].flag=60;
	TBL.m[TBL.mi].x=x;
	TBL.m[TBL.mi].y=y;
	TBL.m[TBL.mi].fx=x<<8;
	TBL.m[TBL.mi].fy=y<<8;
	TBL.m[TBL.mi].vx=vx;
	TBL.m[TBL.mi].vy=vy;
	strncpy(TBL.m[TBL.mi].tx,tx,sizeof(TBL.m[TBL.mi].tx)-1);
	TBL.mi++;
	TBL.mi&=MAX_MSG-1;
}

/*
 *----------------------------------------------------------------------------
 * add message #2
 *----------------------------------------------------------------------------
 */
static void msg2_add(int x,int y,int vx,int vy,const char* tx)
{
	TBL.m2[TBL.m2i].flag=60;
	TBL.m2[TBL.m2i].x=x;
	TBL.m2[TBL.m2i].y=y;
	TBL.m2[TBL.m2i].fx=x<<8;
	TBL.m2[TBL.m2i].fy=y<<8;
	TBL.m2[TBL.m2i].vx=vx;
	TBL.m2[TBL.m2i].vy=vy;
	strncpy(TBL.m2[TBL.m2i].tx,tx,sizeof(TBL.m2[TBL.m2i].tx)-1);
	TBL.m2i++;
	TBL.m2i&=MAX_MSG2-1;
}

/*
 *----------------------------------------------------------------------------
 * add explode
 *----------------------------------------------------------------------------
 */
static void exp_add(int x,int y,int type)
{
	TBL.x[TBL.xi].flag=1;
	TBL.x[TBL.xi].type=type;
	TBL.x[TBL.xi].x=x;
	TBL.x[TBL.xi].y=y;
	TBL.xi++;
	TBL.xi&=MAX_EXP-1;
}

/*
 *----------------------------------------------------------------------------
 * add coin
 *----------------------------------------------------------------------------
 */
static void coin_add(int x,int y)
{
	TBL.c[TBL.ci].flag=1;
	TBL.c[TBL.ci].x=x;
	TBL.c[TBL.ci].y=y;
	TBL.c[TBL.ci].fy=y<<8;
	TBL.c[TBL.ci].vy=-512;
	TBL.ci++;
	TBL.ci&=MAX_COIN-1;
}
