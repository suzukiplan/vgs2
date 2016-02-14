# Video Game System mk-II SR (Linux)

## 前提ソフトウェア
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

## VGSのセットアップ
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
$ cd vgs2
$ git submodule init
$ git submodule
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

## ビルド
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
