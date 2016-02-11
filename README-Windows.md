# Video Game System mk-II SR (Windows)

## 前提ソフトウェア
- Windows版のビルドに必要なもの（必須）
  - Microsoft Visual C++ (コマンドラインで使えるdesktop版)
  - Git
- Android版のビルドに必要なもの
  - Java Developer Kit
  - Android SDK + Apache Ant
  - Android NDK

## VGSのセットアップ
### vgs2のセットアップ
```cmd
> c:
> cd \home
> git clone https://github.com/suzukiplan/vgs2.git vgs2
> cd vgs2
> git submodule init
> git submodule update
> cd bin_src
> make
```

### システム環境変数の設定
- `PATH` : `c:\home\vgs2\bin`
- `VGS2_HOME` : `c:\home\vgs2`

## プロジェクト作成
```cmd
> vgs2mkpj company Test c:\home\Test
```

_※Windowsのファイルシステムは残念なのでプロジェクトのディレクトリは絶対パスしか指定できません_

## ビルド
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
