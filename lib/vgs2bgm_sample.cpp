#include <stdio.h>
#include "vgs2bgm.h"

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
	fgets(buf,sizeof(buf)-1,stdin);

	bgmTerm();
	return 0;
}
