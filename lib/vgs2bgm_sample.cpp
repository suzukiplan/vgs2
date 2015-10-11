#include <windows.h>
#include <process.h>
#include <stdio.h>
#include "vgs2bgm.h"

void timer_thread(void* args)
{
	while(1) {
		Sleep(30);
		printf("Current: %.3f sec\r",bgmGetTime());
	}
}

int main(int argc,char* argv[])
{
	char buf[80];

	if(argc<2) {
		puts("usage: vgs2bgm_sample bgm-file");
		return 1;
	}

	if(bgmInit()) {
		puts("init error.");
		return -1;
	}

	if(bgmLoad(0,argv[1])) {
		puts("load error.");
		bgmTerm();
		return -1;
	}

	bgmPlay(0);

	printf(" Length: %.3f sec\n",bgmGetIntroLength()+bgmGetLoopLength());
	printf("  Intro: %.3f sec\n",bgmGetIntroLength());
	printf("   Loop: %.3f sec\n",bgmGetLoopLength());
	_beginthread(timer_thread,0,NULL);
	fgets(buf,sizeof(buf)-1,stdin);

	bgmTerm();
	return 0;
}

