# Video Game System mk-II SR
## 概要
- `Linux` , `Mac OS X` , `Windows` , `Android` 及び `iOS` 対応のアプリを開発する `SDK` です
- 全てのプラットフォームのゲームの実装を一本化できる特徴があります
- 使用するプログラム言語は `C/C++言語` のみです
- `Objective-c` などのクソ言語は一切使いません

## アプリ開発の流れ
- `Linux` , `Mac OS X` または `Windows` でアプリを開発し、完成後 `Android` 版をビルド、その後 `iOS` へポーティングという流れで開発します
- VGSでは `Linux` , `Mac OS X` および `Windows` はサンドボックス環境という位置づけで扱います
- `vgs2mkpj` コマンドでプロジェクトを作成
- 自プロジェクトの [game.c](https://github.com/suzukiplan/vgs2/blob/master/template/game.c) にゲームの処理を記述
  - `vgs2_init` : 初期化処理を記述
  - `vgs2_loop` : メインループの処理(1/60秒間隔で実行される)を記述
  - `vgs2_pause` : ポーズループの処理(ポーズ時に1/60秒間隔で実行される)を記述
  - `vgs2_term` : 終了時処理を記述
  - [game.c](https://github.com/suzukiplan/vgs2/blob/master/template/game.c) 中では 標準関数、OSに依存しない独自関数、[VGSのAPI関数](https://github.com/suzukiplan/vgs2/blob/master/API.md) を使用できます
- グラフィックと音のリソースを準備
  - `GSLOTxxx.CHR` に画像リソースを準備
  - `ESLOTxxx.PCM` に効果音リソースを準備
  - `BSLOTxxx.BGM` に音楽リソースを準備
- `vgs2rom` コマンドで ROMDATA.BIN を生成
- ビルド（後述）

## ライセンス
- 2-Clause BSDライセンスで提供しています
- [LICENSE.txt](https://github.com/suzukiplan/vgs2/blob/master/LICENSE.txt)を参照してください
- 本リポジトリに含まれるソースコードの 一部 若しくは 全部 を流用 又は `VGS mk-II SR`を用いて製作されたプログラムを配布又は販売する場合、プログラムの利用者が参照できるドキュメント（スマートフォンアプリの場合は公開するアプリストアに掲載される説明文 又は アプリ内で参照できるテキスト）に [LICENSE.txt](https://github.com/suzukiplan/vgs2/blob/master/LICENSE.txt) 記載の Copyright 表記を記載してください
- 本リポジトリに含まれるソースコードの 一部 若しくは 全部 を流用 又は `VGS mk-II SR`を用いて製作されたプログラムを配布又は販売の結果発生した損失や損害等につき、当方は一切の責任を負わないものとします

## 前提ハードウェア
- Linux , Macintosh or Windows PC
- Android phone
- iOS device (iPhone 3GS以降)

## 前提ソフトウェア
### Linux
- Linux版のビルドに必要なもの（必須）
  - GNU Compiler Collection - C compiler (gcc) 
  - SDL
  - ALSA
  - Git
- Android版をビルドする場合
  - Java Developer Kit
  - Apache Ant
  - Android SDK
  - Android NDK

### Mac OS X
- Mac OS X版のビルドに必要なもの（必須）
  - GNU Compiler Collection - C compiler (gcc) 
  - SDL
  - Git
- Android版のビルドに必要なもの
  - Java Developer Kit
  - Apache Ant
  - Android SDK
  - Android NDK
- iOS版のビルドに必要なもの
  - XCODE

### Windows
- Windows版のビルドに必要なもの（必須）
  - Microsoft Visual C++ (コマンドラインで使えるdesktop版)
  - Git
- Android版のビルドに必要なもの
  - Java Developer Kit
  - Android SDK + Apache Ant
  - Android NDK

## VGSのセットアップ（Linux）
### Linux版のビルドに必要なものをインストール
```bash
$ sudo yum install git
$ sudo yum install SDL
$ sudo yum install alsa-lib
$ sudo yum install SDL-devel
$ sudo yum install alsa-lib-devel
```

### vgs2のセットアップ
```bash
$ cd ~/
$ git clone https://github.com/suzukiplan/vgs2.git vgs2
$ cd vgs2/bin_src
$ make
$ export VGS2_HOME=~/vgs2
$ export PATH=$PATH:$VGS2_HOME/bin
```

次回ログイン時から自動的に環境変数の設定（export）を省略したい場合は `~/.bash_profile` に以下の定義を追加してください。
```text
export VGS2_HOME=/Users/suzukiplan/vgs2
export PATH=$PATH:$VGS2_HOME/bin
```

## VGSのセットアップ（Mac OS X）
### SDLのセットアップ
[https://www.libsdl.org/](https://www.libsdl.org/) の `Download` - `SDL2.0` から最新のソースコードをダウンロードしてビルドしてください。以下、ダウンロード後にターミナルでビルド＆インストールする手順を示します。
```bash
$ cd ~/Downloads/
$ tar xvf SDL2-2.0.3.tar.gz 
$ cd SDL2-2.0.3
$ ./configure
$ make
$ sudo make install
```

(TIPS)
- ターミナルは、Finderで左タブから「アプリケーション」を選び、右タブから「ユーティリティ」を開けば見つかります
- ターミナルはVGSを使う上で必須なのでDockに追加しておきましょう
- install後はDownloads以下のファイルは不要なので消しても問題ありません

### vgs2のセットアップ
_省略（Linux版の「vgs2のセットアップ」と同じ手順を実施）_

## VGSのセットアップ（Windows）
### (1) VGS mk-II SRのセットアップ
```cmd
> c:
> cd \home
> git clone https://github.com/suzukiplan/vgs2.git vgs2
> cd vgs2\bin_src
> make
```

### (2) 環境変数の設定
- `PATH` : `c:\home\vgs2\bin`
- `VGS2_HOME` : `c:\home\vgs2`

## プロジェクト作成
### (1) Linux / Mac OS X
```bash
$ vgs2mkpj company Test ~/test
```

_※Linux版の場合、プロジェクトのディレクトリは相対パスでも指定できます_

### (2) Windows
```cmd
> vgs2mkpj company Test c:\home\Test
```

_※Windowsのファイルシステムは残念なのでプロジェクトのディレクトリは絶対パスしか指定できません_

## ビルド(Linux)
### (1) Linux版のビルド
```bash
$ cd ~/test/linux
$ make
```

### (2) Android版のビルド
```bash
$ cd ~/test/android
$ # デバッグビルドの場合
$ make debug
$ # リリースビルドの場合
$ make release
```

（補足）
- AndroidのSDK, NDKのパス、zipalignコマンドのパス、キーストアのパスを予め `android/makefile` に定義する必要があります。
- キーストアはreleaseビルド時にのみ必要です
```text
NDK=~/android/ndk
SDK=~/android/sdk
ZIPALIGN=$(SDK)/build-tools/22.0.1/zipalign
KEYSTORE=./keystore
```

## ビルド(Mac OS X)
### (1) Mac OS X版のビルド
```bash
$ cd ~/test/linux
$ make
```

### (2) Android版のビルド
_省略（Linux版の「(2)Android版のビルド」と同じ手順を実施）_

### (3) iOS版のビルド
_todo: そのうち書く_

## ビルド(Windows)
### (1) Windows版のビルド
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
$ cd /cygdrive/c/home/Test/android/jni
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

## API仕様
[API.md](https://github.com/suzukiplan/vgs2/blob/master/API.md) を参照してください。

## ハードウェア仕様
- 最大同時発色数: 16777216色中256色
- 解像度: 160 x 200
- VRAM: スプライト1面 + BG1面
- PPU
  - スプライト機能
    - GSLOTからの矩形転送
      - 回転
      - 1/2縮小（高速）
      - 任意サイズの拡大・縮小
      - マスク
    - 図形描画
  - BG機能
    - GSLOTからの矩形転送
    - 図形描画
    - ラスタスクロール
- SPU(BGM音源): _初代SUZUKI PLAN - Video Game Systemと同じ_
  - 全ストア型の波形メモリ音源
  - 音色数: 4 (三角波、ノコギリ波、矩形波、ノイズ）
  - チャネル数: 6
  - 周波数: 22050Hz
  - ビットレート: 16bit
  - モノラル
- スロット仕様
  - グラフィックス(GSLOT): 最大256x256ピクセル x 256枚
  - 効果音（ESLOT）: 22050Hz, 16bit, 1ch(モノラル) x 256個
  - BGM（BSLOT）: 最大65,536ノート × 256個
