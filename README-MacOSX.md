# Video Game System mk-II SR (Mac OS X)
## 前提ソフトウェア
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

## VGSのセットアップ
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

### JDKのセットアップ
省略

### Android SDKのセットアップ
省略

### Android NDKのセットアップ
省略

### antのセットアップ
```bash
$ # homebrewのセットアップ
$ ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
$ # antのセットアップ
$ brew install ant
```

### vgs2のセットアップ
```bash
$ cd ~/
$ git clone https://github.com/suzukiplan/vgs2.git vgs2
$ cd vgs2
$ git submodule init
$ git submodule update
$ cd bin_src
$ make
$ export VGS2_HOME=~/vgs2
$ export PATH=$PATH:$VGS2_HOME/bin
```

次回ログイン時から自動的に環境変数の設定（export）を省略したい場合は `~/.bash_profile` に以下の定義を追加してください。
```text
export VGS2_HOME=~/vgs2
export PATH=$PATH:$VGS2_HOME/bin
```

## プロジェクト作成
```bash
$ vgs2mkpj company Test ~/test
```

_※Mac OS X版の場合、プロジェクトのディレクトリは相対パスでも指定できます_

## ビルド
### (1) Mac OS X版のビルド
```bash
$ cd ~/test/mac
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

### (3) iOS版のビルド
_todo: そのうち書く_
