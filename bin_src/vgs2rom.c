/* Binaly Builder */
#include <Windows.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int makelist(char* dpath);
int makelist2(char* path);
void addlist(char* fname);

char g_name[4096][16];
int g_num=0;

int main(int argc,char* argv[])
{
	FILE* fpW;
	FILE* fpR;
	char* buf;
	char* np;
	char nbuf[16];
	char s[4];
	char path[4096];
	int n;
	int i;
	int j;

	if(argc<3) {
		puts("usage: vgsrom input-dir output-file");
		return 1;
	}

	if(NULL==(fpW=fopen(argv[2],"wb"))) {
		puts("write open error");
		return 1;
	}

	/* ファイル数書き出し */
	n=makelist(argv[1]);
	if(n==0) {
		puts("[ERROR] no data.");
		return 1;
	}
	fwrite(&n,sizeof(n),1,fpW);

	/* ファイル名書き出し */
	for(i=0;i<n;i++) {
		np=g_name[i];
		if(15<strlen(np)) {
			printf("too long file name: %s\n",np);
			exit(-1);
		}
		memset(nbuf,0,sizeof(nbuf));
		strcpy(nbuf,np);
		for(j=0;'\0'!=nbuf[j];j++) nbuf[j]^=0xAA;
		fwrite(nbuf,16,1,fpW);
	}

	/* サイズ + 本体 */
	for(i=0;i<n;i++) {
		sprintf(path,"%s\\%s",argv[1],g_name[i]);
		printf("write %s ... ",g_name[i]);
		if(NULL==(fpR=fopen(path,"rb"))) {
			puts("open error");
			exit(-1);
		}
		fseek(fpR,0,SEEK_END);
		j=ftell(fpR);
		fseek(fpR,0,SEEK_SET);
		printf("%d byte ... ",j);
		if(NULL==(buf=(char*)malloc(j))) {
			puts("malloc error");
			exit(-1);
		}
		fread(buf,j,1,fpR);
		fclose(fpR);
		s[0]=(j>>24)&0xFF;
		s[1]=(j>>16)&0xFF;
		s[2]=(j>>8)&0xFF;
		s[3]=j & 0xFF;
		fwrite(s,4,1,fpW);
		fwrite(buf,j,1,fpW);
		free(buf);
		puts("wrote");
	}
	fclose(fpW);

	return 0;
}

int makelist(char* dpath)
{
	char path[4096];
	int ret=0;

	strcpy(path,dpath);
	strcat(path,"\\GSLOT???.CHR");
	ret+=makelist2(path);

	strcpy(path,dpath);
	strcat(path,"\\ESLOT???.PCM");
	ret+=makelist2(path);

	strcpy(path,dpath);
	strcat(path,"\\BSLOT???.BGM");
	ret+=makelist2(path);

	strcpy(path,dpath);
	strcat(path,"\\DSLOT???.DAT");
	ret+=makelist2(path);

	return ret;
}

int makelist2(char* path)
{
	WIN32_FIND_DATA data;
	DWORD err;
	int ret=0;
	HANDLE hList=FindFirstFile(path,&data);
	if(NULL==hList || INVALID_HANDLE_VALUE==hList) {
		err=GetLastError();
		if(ERROR_NO_MORE_FILES==err || ERROR_FILE_NOT_FOUND==err) {
			CloseHandle(hList);
			return ret;
		}
		else {
			printf("ERROR: FindFistFile error. (%d)\n",err);
			exit(-1);
		}
	}
	addlist(data.cFileName);
	ret++;
	while(FindNextFile(hList,&data)) {
		addlist(data.cFileName);
		ret++;
	}
	err=GetLastError();
	if(ERROR_NO_MORE_FILES!=err) {
		printf("ERROR: FindFistFile error. (%d)\n",err);
		exit(-1);
	}
	CloseHandle(hList);
	return ret;
}

void addlist(char* fname)
{
	int i;
	for(i=0;fname[i];i++) {
		fname[i]=toupper(fname[i]);
	}
	if(!isdigit(fname[5])
	|| !isdigit(fname[6])
	|| !isdigit(fname[7])
	|| 255<atoi(&fname[5])) {
		return;
	}
	strcpy(g_name[g_num++],fname);
}
