#include <windows.h>
#include <stdio.h>
#include "vgs2.h"

static int _initialized=0;

extern struct _PSG _psg;

int bload_direct(unsigned char n,const char* name);
void bfree_direct(unsigned char n);

__declspec(dllexport) int __stdcall bgmInitWithHwnd(HWND hwnd)
{
	if(_initialized) return -1;
	if(0!=init_sound(hwnd)) {
		return -1;
	}
	_initialized=1;
	return 0;
}

__declspec(dllexport) int __stdcall bgmInit()
{
	if(_initialized) return -1;
	if(0!=init_sound_cli()) {
		return -1;
	}
	_initialized=1;
	return 0;
}

__declspec(dllexport) void __stdcall bgmTerm()
{
	if(!_initialized) return;
	term_sound();
}

__declspec(dllexport) int __stdcall bgmLoad(int slot,const char* filename)
{
	if(!_initialized) return -1;
	return bload_direct(slot&0xff,filename);
}

__declspec(dllexport) void __stdcall bgmFree(int slot)
{
	if(!_initialized) return;
	bfree_direct(slot&0xff);
}

__declspec(dllexport) int __stdcall bgmIsExit(int slot)
{
	return vgs2_bchk(slot&0xff);
}

__declspec(dllexport) void __stdcall bgmPlay(int slot)
{
	if(!_initialized) return;
	vgs2_bplay(slot&0xff);
}

__declspec(dllexport) double __stdcall bgmGetTime()
{
	double ret = _psg.timeP / 22050.0;
	ret += _psg.loop * (_psg.timeL / 22050.0);
	return ret;
}

double __stdcall bgmGetIntroLength()
{
	return _psg.timeI / 22050.0;
}

double __stdcall bgmGetLoopLength()
{
	return _psg.timeL / 22050.0;
}

__declspec(dllexport) void __stdcall bgmStop()
{
	if(!_initialized) return;
	vgs2_bstop();
}

__declspec(dllexport) void __stdcall bgmResume()
{
	if(!_initialized) return;
	vgs2_bresume();
}

__declspec(dllexport) void __stdcall bgmFadeout()
{
	if(!_initialized) return;
	vgs2_bfade2();
}

__declspec(dllexport) void __stdcall bgmJump(int sec)
{
	if(!_initialized) return;
	vgs2_bjump(sec);
}

__declspec(dllexport) void __stdcall bgmMuteChannel(int cn)
{
	if(!_initialized) return;
	vgs2_bmute(cn);
}

__declspec(dllexport) void __stdcall bgmChangeKey(int n)
{
	if(!_initialized) return;
	vgs2_bkey(n);
}

__declspec(dllexport) void __stdcall bgmMaskChangeKey(int n,int off)
{
	if(!_initialized) return;
	vgs2_bkoff(n,off);
}

__declspec(dllexport) void __stdcall bgmMasterVolume(int rate)
{
	if(!_initialized) return;
	vgs2_bmvol(rate);
}

__declspec(dllexport) void __stdcall bgmChannelVolume(int cn,int rate)
{
	if(!_initialized) return;
	vgs2_bcvol(cn,rate);
}

void putlog(const char* fn,int ln,const char* msg,...)
{
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
