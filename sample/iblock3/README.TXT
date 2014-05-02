このサンプルは、Invader Block 3のソースコードとROMデータです。
これらを用いて、WindowsとAndroidのイメージをビルドする手順を記します。

[ビルド手順]
Visual C++のコマンドプロンプトを起動して次のコマンドを実行
> c:
> cd c:\VGS2\sample
上記の状態から、WindowsとAndroidのInvader Block 3をビルドするまでの手順を記します。

(1)vgs2mkpjで適当なプロジェクトを作る
> vgs2mkpj suzukiplan IBLOCK3 c:\myprj\IBLOCK3

(2)game.cを配置
> copy game.c c:\myprj\IBLOCK3\src\game.c

(3)ROMDATA.BINを配置
> copy ROMDATA.BIN c:\myprj\IBLOCK3\windows\ROMDATA.BIN
> copy ROMDATA.BIN c:\myprj\IBLOCK3\android\res\raw\romdata.bin

(4)c:\myprj\IBLOCK3\windows\makefile からromdataのmakefileをキックしている箇所を削る
before) all: MKROM $(RELDIR) $(PROJECT).EXE
 after) all:       $(RELDIR) $(PROJECT).EXE

上記手順を実施後に、Windows、Androidでビルドを実行すればOKです。

[Windows版のビルド＆実行]
> cd c:\myprj\IBLOCK3\windows\
> nmake
> IBLOCK3.EXE

[Android版のビルド＆実行]
cygwin:
$ cd /cygdrive/c/myprj/IBLOCK3/android/jni
$ ndk-build

Visual C++のコマンドプロンプト:
> c:
> cd c:\myprj\IBLOCK3\android
> ant debug
> adb install -r bin\IBLOCK3-debug.apk
