[Invader Block 3]
Invader Block 3は、ボールでインベーダーを倒すアクションゲームです。
SUZUKI PLANが、GameDaddy対応ゲーム第一弾として開発しました。
GooglePlay、AppStoreで公開中です。

(Android版)
https://play.google.com/store/apps/details?id=com.suzukiplan.IBLOCK3

(iPhone版)
https://itunes.apple.com/us/app/invader-block-3/id838888746?mt=8

[本サンプル概要]
本サンプルは、Invader Block 3のソースコードとROMデータです。
これらを用いて、WindowsとAndroidのイメージをビルドする手順を記します。
この手順を実施する前に、GameDaddy＋前提ソフトウェア（WindowsとAndroid）をインストールしておく必要があります。
https://github.com/suzukiplan/vgs2/wiki/2.-%E3%82%A4%E3%83%B3%E3%82%B9%E3%83%88%E3%83%BC%E3%83%AB%E6%96%B9%E6%B3%95

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
※正規版のInvader Block 3がインストールされている端末だと、adb installに失敗します。
