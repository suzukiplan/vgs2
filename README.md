# Video Game System mk-II SR

## 概要
- `Windows` , `Android` 及び `iOS` 対応のアプリケーションを開発するための `SDK` です
- `Windows` , `Android` 及び `iOS` 全てのプラットフォームのゲームの実装を一本化できる特徴があります
- 使用するプログラム言語は `C/C++言語` のみです

## アプリ開発の流れ
- 最初に `Windows` 上でアプリを開発し、完成したら `Android` 版をビルド、その後 `iOS` へポーティングという流れで開発します
- [プロジェクト作成](https://github.com/suzukiplan/vgs2#%E3%83%97%E3%83%AD%E3%82%B8%E3%82%A7%E3%82%AF%E3%83%88%E4%BD%9C%E6%88%90) の手順でプロジェクトを作成
- 自プロジェクトの [game.c](https://github.com/suzukiplan/vgs2/blob/master/template/game.c) にゲームの処理を記述
  - `vgs2_init` に初期化処理を記述
  - `vgs2_loop` にメインループの処理(1/60秒間隔で実行される)を記述
  - `vgs2_pause` にポーズループの処理(ポーズ時に1/60秒間隔で実行される)を記述
  - `vgs2_term` に終了時処理を記述
  - [game.c](https://github.com/suzukiplan/vgs2/blob/master/template/game.c) 中では 標準関数、OSに依存しない独自実装関数、`VGS mk-II SR` の [API関数](https://github.com/suzukiplan/vgs2/blob/master/API.md) を利用できます
- `GSLOTxxx.CHR` に画像リソースを準備
- `ESLOTxxx.PCM` に効果音リソースを準備
- `BSLOTxxx.BGM` に音楽リソースを準備
- `vgs2rom` コマンドで ROMDATA.BIN を生成
- [ビルド](https://github.com/suzukiplan/vgs2#%E3%83%93%E3%83%AB%E3%83%89)

## ライセンス
- 2-Clause BSDライセンスで提供しています
- [LICENSE.txt](https://github.com/suzukiplan/vgs2/blob/master/LICENSE.txt)を参照してください

## 前提ハードウェア
- Windows PC
- Android phone
- Macintosh
- iOS device

## 前提ソフトウェア
### Windows
- Microsoft Visual C++ (コマンドラインで使えるdesktop版)
- Java Developer Kit
- Android SDK + Apache Ant
- Android NDK
- GitHub

### Macintosh
- XCODE

## セットアップ（Windows + Androidアプリ開発環境）

### (1) VGS mk-II SRのセットアップ
```cmd
> c:
> cd \home
> git clone https://github.com/suzukiplan/vgs2.git vgs2
> cd vgs2\bin_src
> nmake
```

### (2) 環境変数の設定
- `PATH` : `c:\home\vgs2\bin`
- `VGS2_HOME` : `c:\home\vgs2`

## プロジェクト作成
```cmd
> vgs2mkpj company Test c:\home\Test
```

## ビルド
#### (1) Windows版のビルド
```cmd
> c:
> cd \home\Test\windows
> nmake
```

- `Windows版` のビルドでは `Android版` の `ROMDATA.BIN` の作成も行います
- `ROMDATA.BIN` のビルド手続きは、`C:\home\Test\romdata` 以下にある [makefile](https://github.com/suzukiplan/vgs2/blob/master/template/makerom) に記述されています

### (2) Android版のビルド
#### cygwin
```cygwin
$ cd /cyghome/c/home/Test/android/jni
$ ndk_build
```

#### MS-DOS (デバッグビルド時)
```cmd
> c:
> cd \home\Test\android
> ant debug
```

#### MS-DOS (リリースビルド時)
```cmd
> c:
> cd \home\Test\android
> ant release
> jarsigner -sigalg MD5withRSA -digestalg SHA1 -verbose -keystore my_keystore bin/Test-release-unsigned.apk techkey
> c:\android\sdk\build-tools\21.1.2\zipalign -v 4 bin\Test-release-unsigned.apk bin\Test-release.apk 
```

### (3) iPhone版のビルド
todo: 後で書く

## API仕様
[API.md](https://github.com/suzukiplan/vgs2/blob/master/API.md) を参照してください。

## ハードウェア仕様
todo: 後で書く
