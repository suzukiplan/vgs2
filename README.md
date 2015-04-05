# Video Game System mk-II SR
## 概要
- `Windows` , `Android` 及び `iOS` 対応のアプリケーションを開発するための `SDK` です
- `Windows` , `Android` 及び `iOS` 全てのプラットフォームのゲームの実装を一本化できる特徴があります
- 使用するプログラム言語は `C/C++言語` のみです

## アプリ開発の流れ
- `Windows版` のアプリを開発し、完成後 `Android` 版をビルド、その後 `iOS` へポーティングという流れで開発します
- [プロジェクト作成](https://github.com/suzukiplan/vgs2#%E3%83%97%E3%83%AD%E3%82%B8%E3%82%A7%E3%82%AF%E3%83%88%E4%BD%9C%E6%88%90) の手順でプロジェクトを作成
- 自プロジェクトの [game.c](https://github.com/suzukiplan/vgs2/blob/master/template/game.c) にゲームの処理を記述
  - `vgs2_init` : 初期化処理を記述
  - `vgs2_loop` : メインループの処理(1/60秒間隔で実行される)を記述
  - `vgs2_pause` : ポーズループの処理(ポーズ時に1/60秒間隔で実行される)を記述
  - `vgs2_term` : 終了時処理を記述
  - [game.c](https://github.com/suzukiplan/vgs2/blob/master/template/game.c) 中では 標準関数、OSに依存しない独自実装関数、`VGS mk-II SR` の [API関数](https://github.com/suzukiplan/vgs2/blob/master/API.md) を利用できます
- グラフィックと音のリソースを準備
  - `GSLOTxxx.CHR` に画像リソースを準備
  - `ESLOTxxx.PCM` に効果音リソースを準備
  - `BSLOTxxx.BGM` に音楽リソースを準備
- `vgs2rom` コマンドで ROMDATA.BIN を生成
- [ビルド](https://github.com/suzukiplan/vgs2#%E3%83%93%E3%83%AB%E3%83%89)

## ライセンス
- 2-Clause BSDライセンスで提供しています
- [LICENSE.txt](https://github.com/suzukiplan/vgs2/blob/master/LICENSE.txt)を参照してください
- 本リポジトリに含まれるソースコードの 一部 若しくは 全部 を流用 又は `VGS mk-II SR`を用いて製作されたプログラムを配布又は販売する場合、プログラムの利用者が参照できるドキュメント（スマートフォンアプリの場合は公開するアプリストアに掲載される説明文 又は アプリ内で参照できるテキスト）に [LICENSE.txt](https://github.com/suzukiplan/vgs2/blob/master/LICENSE.txt) 記載の Copyright 表記を記載してください
- 本リポジトリに含まれるソースコードの 一部 若しくは 全部 を流用 又は `VGS mk-II SR`を用いて製作されたプログラムを配布又は販売の結果発生した損失や損害等につき、当方は一切の責任を負わないものとします

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
- Git

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

### (3) iPhone版のビルド
todo: 後で書く

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
