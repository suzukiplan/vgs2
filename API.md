# API関数リファレンス
`SUZUKI PLAN - Video Game System mk-II SR` が提供するAPI関数の仕様を記します。

## (1)デバイス入力

### vgs2_touch
> 画面のタッチ（マウスのクリック）状態を取得します。
>
> `プロトタイプ`
>
> void vgs2_touch(int* s,int* cx,int* cy,int* dx,int* dy);
>
> `引数`
> - s: 非タッチ中は0, タッチ中は非0
> - cx: タッチしているX座標（非タッチ中は不定値）
> - cy: タッチしているY座標（非タッチ中は不定値）
> - dx: スライドしたX座標差分（非タッチ中は不定値）
> - dy: スライドしたY座標差分（非タッチ中は不定値）

### vgs2_setPause
> ポーズ状態の設定、解除を行います。
>
> `プロトタイプ`
>
> void vgs2_setPause(unsigned char c);
>
> `引数`
> - c: ポーズ状態にする場合は1、解除する場合は0を指定する

## (2)ファイル入出力

### vgs2_fopen
> - ファイルを開きます。
> - ファイルを開く時は、標準関数fopenの代わりにこの関数を用いてください。
> - ファイルの読み書きや閉じる操作には、標準関数（fread, fwrite, fclose等）を用いてください。
> - ファイル名には、ディレクトリを含めないでください。
>
> `プロトタイプ`
>
> FILE* vgs2_fopen(const char* n,const char* p);
>
> `引数・戻り値`
> - fopen関数と同じです。

## (3)数学
GameDaddyの数学は、通常の数学関数と違い、実数を全て固定小数点数方式で扱います。

### vgs2_abs
> - 絶対値を求めるマクロです。
>
> `マクロ宣言`
>
> ＃define vgs2_abs(x) (x>=0?(x):-(x))

### vgs2_sgn
> - 指定値が正数orゼロの場合は1、負数の場合は-1を返すマクロです。
> - 一般的なsgn関数と違いゼロを返しません。
>
> `マクロ宣言`
>
> ＃define vgs2_sgn(x) (x>=0?(1):(-1))

### vgs2_rad
> - 2点間の角度をラジアン値×100（0～627※小数点以下切り捨て）で求めます。
>
> `プロトタイプ`
>
> int vgs2_rad(int x1,int y1,int x2,int y2);
>
> `引数`
> - x1, y1: 点1の座標
> - x2, y2: 点2の座標
>
> `戻り値`
> - 0～627（ラジアン値×100）

### vgs2_deg
> - 2点間の角度（0～359）を求めます。
>
> `プロトタイプ`
>
> int vgs2_deg(int x1,int y1,int x2,int y2);
>
> `引数`
> - x1, y1: 点1の座標
> - x2, y2: 点2の座標
>
> `戻り値`
> - 0～359

### vgs2_deg2rad
> - 角度をラジアン値×100（0～627※小数点以下切り捨て）に変換します。
>
> `プロトタイプ`
>
> int vgs2_deg2rad(int deg);
>
> `引数`
> - deg: 角度
>
> `戻り値`
> - 0～627（ラジアン値×100）

### vgs2_rad2deg
> - ラジアン値×100を角度（0～359）に変換します。
>
> `プロトタイプ`
>
> int vgs2_rad2deg(int rad);
>
> `引数`
> - rad: ラジアン値×100
>
> `戻り値`
> - 0～359

### vgs2_rands
> - 疑似乱数を初期化します。
>
> `プロトタイプ`
>
> void vgs2_rands();

### vgs2_rand
> - 疑似乱数値を求めます。
>
> `プロトタイプ`
>
> int vgs2_rand();
>
> `戻り値`
> - 0～2147483647

### vgs2_sin
> - ラジアン値×100からサイン値×256を求めます。
>
> `プロトタイプ`
>
> int vgs2_sin(int r);
>
> `引数`
> - r: ラジアン値×100
>
> `戻り値`
> - sin(r÷100)×256

### vgs2_cos
> - ラジアン値×100からコサイン値×256を求めます。
>
> `プロトタイプ`
>
> int vgs2_cos(int r);
>
> `引数`
> - r: ラジアン値×100
>
> `戻り値`
> - cos(r÷100)×256

## (4)効果音

### vgs2_eff
> - 効果音を再生します。
>
> `プロトタイプ`
>
> void vgs2_eff(unsigned char n);
>
> `引数`
> - n: ESLOTのスロット番号

### vgs2_effstop
> - 特定の効果音の再生を停止します。
>
> `プロトタイプ`
>
> void vgs2_effstop(unsigned char n);
>
> `引数`
> - n: ESLOTのスロット番号

### vgs2_effstopA
> - 全ての効果音の再生を停止します。
>
> `プロトタイプ`
>
> void vgs2_effstopA();

### vgs2_setmute
> - ミュート状態を設定or解除します。
>
> `プロトタイプ`
>
> void vgs2_setmute(unsigned char n);
>
> `引数`
> - n: 0=解除、1=設定

### vgs2_getmute
> - ミュート状態を取得します。
>
> `プロトタイプ`
>
> unsigned char vgs2_getmute();
>
> `戻り値`
> - ミュート状態（0=解除、1=設定）

## (5)グラフィックス

### vgs2_pallet
> - ベースカラーを設定します。
> - ベースカラーは、モノクロ表示の基礎となる色のことです。
> - 画面は、本関数で設定したベースカラーを基にした8段調のモノクロで表示されます。
>
> `プロトタイプ`
>
> void vgs2_pallet(int c);
>
> `引数`
> - c: ベースカラー（1～7）
>   - 1:blue (青)
>   - 2:red (赤)
>   - 3:magenta (紫)
>   - 4:green (緑)
>   - 5:cyan (水色)
>   - 6:yellow (黄色)
>   - 7:white (白)

### vgs2_pallet256
> - 8bitカラー（256色）モードにします。
> - パレット情報はデータスロットに持つ必要があります。
> - データスロットには、1色あたり32bit（ARGB）の色情報 × 256色（合計 `1024byte` ）のバイナリデータが格納されている必要があります。
>
> `プロトタイプ`
>
> void vgs2_pallet256(unsigned char n);
>
> `引数`
> - n: パレット情報が格納されているデータスロット番号

### vgs2_interlace
> - インタレース・スキャンのON/OFFを設定します。
> - アプリ起動時のデフォルトはONです。
>
> `プロトタイプ`
>
> void vgs2_interlace(int i);
>
> `引数`
> - i: インタレースのON(1)またはOFF(0)

### vgs2_clear
> - BG面をクリアします。
>
> `プロトタイプ`
>
> void vgs2_clear(unsigned char c);
>
> `引数`
> 
> - c: パレット番号（0～7）

### vgs2_scroll
> - BG面をスクロール（ラスタスクロール）します。
>
> `プロトタイプ`
>
> void vgs2_scroll(int x,int y);
>
> `引数`
> 
> - x,y: スクロール値（ピクセル単位）

### vgs2_pixel
> - 点を描画します。
>
> `プロトタイプ`
> - void vgs2_pixel(unsigned char* p,int x,int y,unsigned char c);
> - ＃define vgs2_pixelBG(X,Y,C) vgs2_pixel(_vram.bg,X,Y,C)
> - ＃define vgs2_pixelSP(X,Y,C) vgs2_pixel(_vram.sp,X,Y,C)
>
> `引数`
> - p: _vram.bg(BG面) or _vram.sp(スプライト面)
> - x,y: 座標
> - c: パレット番号

### vgs2_line
> - 線を描画します。
>
> `プロトタイプ`
> - void vgs2_line(unsigned char* p,int fx,int fy,int tx,int ty,unsigned char c);
> - ＃define vgs2_lineBG(FX,FY,TX,TY,C) vgs2_line(_vram.bg,FX,FY,TX,TY,C)
> - ＃define vgs2_lineSP(FX,FY,TX,TY,C) vgs2_line(_vram.sp,FX,FY,TX,TY,C)
>
> `引数`
> - p: _vram.bg(BG面) or _vram.sp(スプライト面)
> - fx,fy: 座標(from)
> - tx,ty: 座標(to)
> - c: パレット番号

### vgs2_circle
> - 円を描画します。
>
> `プロトタイプ`
> - void vgs2_circle(char*p, int x,int y,int r, unsigned char c);
> - ＃define vgs2_circleBG(X,Y,R,C) vgs2_circle(_vram.bg,X,Y,R,C)
> - ＃define vgs2_circleSP(X,Y,R,C) vgs2_circle(_vram.sp,X,Y,R,C)
>
> `引数`
> - p: _vram.bg(BG面) or _vram.sp(スプライト面)
> - x,y: 座標
> - r: 半径
> - c: パレット番号

### vgs2_box
> - vgs2_lineを用いて矩形を描画するマクロです。
>
> `マクロ宣言`
> ＃define vgs2_boxBG(FX,FY,TX,TY,C) \
>
> vgs2_line(_vram.bg,FX,FY,TX,FY,C);\
>
> vgs2_line(_vram.bg,FX,TY,TX,TY,C);\
>
> vgs2_line(_vram.bg,FX,FY,FX,TY,C);\
>
> vgs2_line(_vram.bg,TX,FY,TX,TY,C)
>
> ＃define vgs2_boxSP(FX,FY,TX,TY,C) \
>
> vgs2_line(_vram.sp,FX,FY,TX,FY,C);\
>
> vgs2_line(_vram.sp,FX,TY,TX,TY,C);\
>
> vgs2_line(_vram.sp,FX,FY,FX,TY,C);\
>
> vgs2_line(_vram.sp,TX,FY,TX,TY,C)

### vgs2_boxf
> - 塗りつぶし矩形を描画します。
>
> `プロトタイプ`
> - void vgs2_boxf(unsigned char* p,int fx,int fy,int tx,int ty,unsigned char c);
> - ＃define vgs2_boxfBG(FX,FY,TX,TY,C) vgs2_boxf(_vram.bg,FX,FY,TX,TY,C)
> - ＃define vgs2_boxfSP(FX,FY,TX,TY,C) vgs2_boxf(_vram.sp,FX,FY,TX,TY,C)
>
> `引数`
> - p: _vram.bg(BG面) or _vram.sp(スプライト面)
> - fx,fy: 座標(from)
> - tx,ty: 座標(to)
> - c: パレット番号

### vgs2_putBG
> - GSLOTの内容をBG面へ矩形転送します。
>
> `プロトタイプ`
> - void vgs2_putBG(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy);
> - void vgs2_putBG2(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy);
>
> `備考`
> - vgs2_putBG: パレット0を含めて描画する
> - vgs2_putBG2: パレット0を透明色として描画する
>
> `引数`
> - n: GSLOTのスロット番号
> - sx,sy: 転送元座標（GSLOT）
> - xs,ys: サイズ
> - dx,dy: 転送先座標（BG面）

### vgs2_putSP
> - GSLOTの内容をスプライト面へ矩形転送します。
>
> `プロトタイプ`
> - void vgs2_putSP(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy);
> - void vgs2_putSPM(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy,unsigned char c);
> - void vgs2_putSPH(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy);
> - void vgs2_putSPMH(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy,unsigned char c);
> - void vgs2_putSPR(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy,int r);
> - void vgs2_putSPE(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy,int dxs,int dys);
> - void vgs2_putSPRE(unsigned char n,int sx,int sy,int xs,int ys,int dx,int dy,int r,int dxs,int dys);
>
> `備考`
> - vgs2_putSPM: 非透明色をマスク（単色描画）とする
> - vgs2_putSPH: 1/2サイズで転送する
> - vgs2_putSPMH: vgs2_putSPM + vgs2_putSPH（組み合わせ）
> - vgs2_putSPR: 回転させる
> - vgs2_putSPE: 拡大（縮小）する
> - vgs2_putSPRE: vgs2_putSPR +　vgs2_putSPE（組み合わせ） 
>
> `引数`
> - n: GSLOTのスロット番号
> - sx,sy: 転送元座標（GSLOT）
> - xs,ys: サイズ
> - dx,dy: 転送先座標（スプライト面）
> - c: マスク時のパレット番号
> - r: 回転角（ラジアン×100）
> - dxs,dys: 転送先サイズ

## (6)ユーティリティ

### vgs2_getdata
> - DSLOTの内容を取得します。
>
> `プロトタイプ`
>
> const char* vgs2_getdata(unsigned char n,unsigned int* size);
>
> `引数`
> - n: DSLOTのスロット番号
> - size: サイズ（output only）
>
> `戻り値`
> - NULL: 指定したDSLOTが存在しない
> - NULL以外: DSLOTの先頭ポインタ
>
> `備考`
> - 戻り値で返されるポインタは、アライメント調整していません。

## (7)BGM

### vgs2_bplay
> - BGMを再生します
>
> `プロトタイプ`
>
> void vgs2_bplay(unsigned char n);
>
> - n: BSLOTのスロット番号

### vgs2_bchk
> - BGMデータ（BSLOT）の有無をチェックします
>
> `プロトタイプ`
>
> void vgs2_bchk(unsigned char n);
>
> - n: BSLOTのスロット番号

### vgs2_bstop
> - BGMを停止します
>
> `プロトタイプ`
>
> void vgs2_bstop();

### vgs2_bresume
> - BGMを停止したところから再開します
>
> `プロトタイプ`
>
> void vgs2_bresume();

### vgs2_bfade
> - BGMをフェードアウトします
>
> `プロトタイプ`
>
> void vgs2_bfade(unsigned int hz);
>
> - hz: マスターボリュームをデクリメントする間隔（Hz単位: 1秒=22050Hz）
