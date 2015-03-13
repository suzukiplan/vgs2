# Video Game System mk-II SR

## 概要
`SUZUKI PLAN - Video Game System mk-II SR`は、`C/C++` 言語で `Windows` , `Android` 及び `iOS` 対応のアプリケーションを開発するための `SDK` です

## ライセンス
- 2-Clause BSDライセンスで提供しています
- LICENSE.txtを参照してください

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

_※Windows版のビルドではAndroid版のROMDATA.BINの作成も行います_

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

## アプリの開発方法
todo: 後で書く

## API仕様
API.md を参照してください。

## ハードウェア仕様
todo: 後で書く
