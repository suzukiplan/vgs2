#ifndef INCLUDE_VGS2BGM_H
#define INCLUDE_VGS2BGM_H
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int __stdcall bgmInitWithHwnd(HWND hwnd);
int __stdcall bgmInit();
void __stdcall bgmTerm();
int __stdcall bgmLoad(int slot,const char* filename);
void __stdcall bgmFree(int slot);
int __stdcall bgmIsExit(int slot);
void __stdcall bgmPlay(int slot);
double __stdcall bgmGetTime();
int __stdcall bgmGetLoopCount();
double __stdcall bgmGetIntroLength();
double __stdcall bgmGetLoopLength();
void __stdcall bgmStop();
void __stdcall bgmResume();
void __stdcall bgmFadeout();
void __stdcall bgmJump(int sec);
void __stdcall bgmMuteChannel(int cn);
void __stdcall bgmChangeKey(int n);
void __stdcall bgmMaskChangeKey(int n,int off);
void __stdcall bgmMasterVolume(int rate);
void __stdcall bgmChannelVolume(int cn,int rate);

#ifdef __cplusplus
};
#endif

#endif
