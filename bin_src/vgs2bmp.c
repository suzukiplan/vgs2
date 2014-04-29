/* Windowsの256色bitmapを独自形式に変換するツール */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 情報ヘッダ */
struct DatHead {
	int isize;					/* 情報ヘッダサイズ */
	int width;					/* 幅 */
	int height;					/* 高さ */
	unsigned short planes;		/* プレーン数 */
	unsigned short bits;		/* 色ビット数 */
	unsigned int ctype;			/* 圧縮形式 */
	unsigned int gsize;			/* 画像データサイズ */
	int xppm;					/* X方向解像度 */
	int yppm;					/* Y方向解像度 */
	unsigned int cnum;			/* 使用色数 */
	unsigned int inum;			/* 重要色数 */
};

int main(int argc,char* argv[])
{
	FILE* fpR=NULL;
	FILE* fpW=NULL;
	int rc=0;
	char fh[14];
	int pal[256];
	struct DatHead dh;
	unsigned char r,g,b;
	unsigned char mh[4];
	int i;
	char* gbuf=NULL;

	/* 引数チェック */
	rc++;
	if(argc<3) {
		fprintf(stderr,"usage: vgsbmp input(bmp) output(chr)\n");
		goto ENDPROC;
	}

	/* 読み込みファイルをオープン */
	rc++;
	if(NULL==(fpR=fopen(argv[1],"rb"))) {
		fprintf(stderr,"ERROR: Could not open: %s\n",argv[1]);
		goto ENDPROC;
	}

	/* ファイルヘッダを読み込む */
	rc++;
	if(sizeof(fh)!=fread(fh,1,sizeof(fh),fpR)) {
		fprintf(stderr,"ERROR: Invalid file header.\n");
		goto ENDPROC;
	}

	/* 先頭2バイトだけ読む */
	rc++;
	if(strncmp(fh,"BM",2)) {
		fprintf(stderr,"ERROR: Invalid file header.\n");
		goto ENDPROC;
	}

	/* 情報ヘッダを読み込む */
	rc++;
	if(sizeof(dh)!=fread(&dh,1,sizeof(dh),fpR)) {
		fprintf(stderr,"ERROR: Invalid file header.\n");
		goto ENDPROC;
	}
#if 0
	printf("INPUT: width=%d, height=%d, bits=%d(%d), cmp=%d\n"
		,dh.width
		,dh.height
		,(int)dh.bits
		,dh.cnum
		,dh.ctype
		);
#endif

	/* 幅及び高さは、1ピクセル以上256ピクセル以下のみ */
	rc++;
	if(dh.width<1 || 256<dh.width || dh.height<1 || 256<dh.height) {
		fprintf(stderr,"ERROR: Size too large. (MAX:256x256)\n");
		goto ENDPROC;
	}

	/* 8ビットカラー以外は弾く */
	rc++;
	if(8!=dh.bits) {
		fprintf(stderr,"ERROR: This program supports only 8bit color.\n");
		goto ENDPROC;
	}

	/* 無圧縮以外は弾く */
	rc++;
	if(dh.ctype) {
		fprintf(stderr,"ERROR: This program supports only none-compress type.\n");
		goto ENDPROC;
	}

	/* 画像データを読む込む領域を確保する */
	rc++;
	if(NULL==(gbuf=(char*)malloc(dh.gsize))) {
		fprintf(stderr,"ERROR: This program supports only 8bit color.\n");
		goto ENDPROC;
	}

	/* パレット情報を読み込む */
	rc++;
	if(sizeof(pal)!=fread(pal,1,sizeof(pal),fpR)) {
		fprintf(stderr,"ERROR: Could not read pallete data.\n");
		goto ENDPROC;
	}

	/* 画像データを読み込む */
	rc++;
	if(dh.gsize!=fread(gbuf,1,dh.gsize,fpR)) {
		fprintf(stderr,"ERROR: Could not read graphic data.\n");
		goto ENDPROC;
	}

	/* 書き込みファイルをオープン */
	rc++;
	if(NULL==(fpW=fopen(argv[2],"wb"))) {
		fprintf(stderr,"ERROR: Could not open: %s\n",argv[2]);
		goto ENDPROC;
	}

	/* ヘッダを書き込む */
	rc++;
	mh[0]='S';
	mh[1]='Y';
	mh[2]=(dh.width-1) & 0xFF;
	mh[3]=(dh.height-1) & 0xFF;
	if(sizeof(mh)!=fwrite(mh,1,sizeof(mh),fpW)) {
		fprintf(stderr,"ERROR: Could not write size data.\n");
		goto ENDPROC;
	}

	/* 画像データを上下反転して書き込む */
	/* (BMPは上下反転状態で書かれているため) */
	rc++;
	for(i=dh.height-1;0<=i;i--) {
		if(dh.width!=fwrite(&gbuf[i*dh.width],1,dh.width,fpW)) {
			fprintf(stderr,"ERROR: Could not write graphic data.\n");
			goto ENDPROC;
		}
	}

	rc=0;

	/* 終了処理 */
ENDPROC:
	if(gbuf) {
		free(gbuf);
	}
	if(fpR) {
		fclose(fpR);
	}
	if(fpW) {
		fclose(fpW);
	}
	return rc;
}
