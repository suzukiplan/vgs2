/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - sound system abstraction layer for NT
 *    Platform: Windows
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#include <Windows.h>
#include <process.h>
#include <dsound.h>
#include "vgs2.h"

/*
 *----------------------------------------------------------------------------
 * macros
 *----------------------------------------------------------------------------
 */
#define SND_INIT	0
#define SND_READY	1
#define SND_EQ		254
#define SND_END		255

/*
 *----------------------------------------------------------------------------
 * file static variables
 *----------------------------------------------------------------------------
 */
static LPDIRECTSOUND8 _lpDS=NULL;
static LPDIRECTSOUNDBUFFER8 _lpSB=NULL;
static LPDIRECTSOUNDNOTIFY8 _lpNtfy=NULL;
static DSBPOSITIONNOTIFY _dspn;
static uintptr_t _uiSnd=-1;
static BYTE _SndCTRL=SND_INIT;
static CRITICAL_SECTION _lock;

/*
 *----------------------------------------------------------------------------
 * Function prototypes
 *----------------------------------------------------------------------------
 */
static void sound_thread(void* arg);
static int ds_wait(BYTE wctrl);

/*
 *----------------------------------------------------------------------------
 * get console window handler
 *----------------------------------------------------------------------------
 */
HWND get_current_hwnd(void)
{
	HWND hwndFound;
	char pszNewWindowTitle[1024];
	char pszOldWindowTitle[1024];
	GetConsoleTitle(pszOldWindowTitle, 1024);
	wsprintf(pszNewWindowTitle,"%d/%d",GetTickCount(),GetCurrentProcessId());
	SetConsoleTitle(pszNewWindowTitle);
	Sleep(40);
	hwndFound=FindWindow(NULL, pszNewWindowTitle);
	SetConsoleTitle(pszOldWindowTitle);
	return hwndFound;
}

/*
 *-----------------------------------------------------------------------------
 * initialize sound for CLI
 *-----------------------------------------------------------------------------
 */
int init_sound_cli()
{
    return init_sound(NULL);
}

/*
 *----------------------------------------------------------------------------
 * initialize DirectSound
 *----------------------------------------------------------------------------
 */
extern "C" int init_sound(HWND hWnd)
{
	DSBUFFERDESC desc;
	LPDIRECTSOUNDBUFFER tmp=NULL;
	HRESULT res;
	WAVEFORMATEX wFmt;
	InitializeCriticalSection(&_lock);

	if(NULL==hWnd) {
		hWnd=get_current_hwnd();
	}

	res=DirectSoundCreate8(NULL,&_lpDS,NULL);
	if(FAILED(res)) {
		return -1;
	}

	res=_lpDS->SetCooperativeLevel(hWnd,DSSCL_NORMAL);
	if(FAILED(res)) {
		return -1;
	}

	memset(&wFmt,0,sizeof(wFmt));
	wFmt.wFormatTag = WAVE_FORMAT_PCM;
	wFmt.nChannels = SAMPLE_CH;
	wFmt.nSamplesPerSec = SAMPLE_RATE;
	wFmt.wBitsPerSample = SAMPLE_BITS;
	wFmt.nBlockAlign = wFmt.nChannels * wFmt.wBitsPerSample / 8;
	wFmt.nAvgBytesPerSec = wFmt.nSamplesPerSec * wFmt.nBlockAlign;
	wFmt.cbSize = 0;
	memset(&desc,0,sizeof(desc));
	desc.dwSize=(DWORD)sizeof(desc);
	desc.dwFlags=DSBCAPS_CTRLPOSITIONNOTIFY;
	desc.dwBufferBytes=SAMPLE_BUFS;
	desc.lpwfxFormat=&wFmt;
	desc.guid3DAlgorithm=GUID_NULL;
	res=_lpDS->CreateSoundBuffer(&desc,&tmp,NULL);
	if(FAILED(res)) {
		return -1;
	}

	res=tmp->QueryInterface(IID_IDirectSoundBuffer8,(void**)&_lpSB);
	tmp->Release();
	if(FAILED(res)) {
		return -1;
	}

	res=_lpSB->QueryInterface(IID_IDirectSoundNotify,(void**)&_lpNtfy);
	if(FAILED(res)) {
		return -1;
	}

	_dspn.dwOffset=SAMPLE_BUFS-1;
	_dspn.hEventNotify=CreateEvent(NULL,FALSE,FALSE,NULL);
	if((HANDLE)-1==_dspn.hEventNotify || NULL==_dspn.hEventNotify) {
		return -1;
	}
	res=_lpNtfy->SetNotificationPositions(1,&_dspn);
	if(FAILED(res)) {
		return -1;
	}

	/* start sound controller thread */
	_uiSnd=_beginthread(sound_thread,65536,NULL);
	if(-1L==_uiSnd) {
		return -1;
	}

	/* wait for ready */
	if(ds_wait(SND_READY)) {
		return -1;
	}
	return 0;
}

/*
 *----------------------------------------------------------------------------
 * Sound controller thread
 *----------------------------------------------------------------------------
 */
static void sound_thread(void* arg)
{
	HRESULT res;
	LPVOID lpBuf;
	DWORD dwSize;
	char buf[SAMPLE_BUFS];

	_SndCTRL=SND_READY;

	while(1) {
		while(SND_READY==_SndCTRL) {
			sndbuf(buf,sizeof(buf));
			dwSize=SAMPLE_BUFS;
			while(1) {
				res=_lpSB->Lock(0
							,SAMPLE_BUFS
							,&lpBuf
							,&dwSize
							,NULL
							,NULL
							,DSBLOCK_FROMWRITECURSOR);
				if(!FAILED(res)) {
					break;
				}
				Sleep(1);
			}
			memcpy(lpBuf,buf,dwSize);
			res=_lpSB->Unlock(lpBuf,dwSize,NULL,NULL);
			if(FAILED(res)) {
				goto ENDPROC;
			}
			ResetEvent(_dspn.hEventNotify);
			res=_lpSB->SetCurrentPosition(0);
			if(FAILED(res)) {
				goto ENDPROC;
			}
			while(1) {
				res=_lpSB->Play(0,0,0);
				if(!FAILED(res)) break;
				Sleep(1);
			}
			WaitForSingleObject(_dspn.hEventNotify,INFINITE);
		}
		if(SND_EQ==_SndCTRL) break;
		_SndCTRL=SND_READY;
	}
	_SndCTRL=SND_END;
ENDPROC:
	return;
}

/*
 *----------------------------------------------------------------------------
 * wait for stopped of sound controller thread
 *----------------------------------------------------------------------------
 */
extern "C" void term_sound()
{
	if(-1==_uiSnd) {
		return;
	}
	if(ds_wait(SND_READY)) {
		return;
	}
	_SndCTRL=SND_EQ;
	if(ds_wait(SND_END)) {
		return;
	}
	WaitForSingleObject((HANDLE)_uiSnd,INFINITE);

	if(_lpNtfy) {
		_lpNtfy->Release();
		_lpNtfy=NULL;
	}
	if((HANDLE)-1==_dspn.hEventNotify || NULL==_dspn.hEventNotify) {
		CloseHandle(_dspn.hEventNotify);
		_dspn.hEventNotify=NULL;
	}
	if(_lpSB) {
		_lpSB->Release();
		_lpSB=NULL;
	}
	if(_lpDS) {
		_lpDS->Release();
		_lpDS=NULL;
	}
	DeleteCriticalSection(&_lock);
}

/*
 *----------------------------------------------------------------------------
 * wait for status modification of sound controller thread
 *----------------------------------------------------------------------------
 */
static int ds_wait(BYTE wctrl)
{
	DWORD ec;
	while(wctrl!=_SndCTRL) {
		Sleep(10);
		if(GetExitCodeThread((HANDLE)_uiSnd,&ec)) {
			if(STILL_ACTIVE!=ec) {
				return -1;
			}
		} else {
			return -1;
		}
	}
	return 0;
}

/*
 *----------------------------------------------------------------------------
 * inter thread lock
 *----------------------------------------------------------------------------
 */
extern "C" void lock()
{
	EnterCriticalSection(&_lock);
}

/*
 *----------------------------------------------------------------------------
 * inter thread unlock
 *----------------------------------------------------------------------------
 */
extern "C" void unlock()
{
	LeaveCriticalSection(&_lock);
}
