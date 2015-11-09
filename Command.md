# Command manual

## vgs2bmp
#### 概要
bitmapファイル を `ROMDATA.BIN` の イメージ形式(.CHR) に変換します

#### usage
```bash
$ vgs2bmp bitmap-file CHR-file
```
- bitmap-file: _input file_
  - 8bit color (256 color)
  - 256 x 256 pixels or less
- CHR-file: _output file_
  - ROMDATA image

## vgs2wav
#### 概要
wavファイル を `ROMDATA.BIN` の イメージ形式(.PCM) に変換します

#### usage
```bash
$ vgs2wav wav-file PCM-file
```
- wav-file: _input file_
  - sampling rate: 22050Hz
  - bit rate: 16bit
  - channels: 1 (mono)
- PCM-file: _output file_
  - ROMDATA image

## vgs2mml
#### 概要
[VGSのMML](https://github.com/suzukiplan/vgs2/blob/master/MML.md)が記述されたテキストファイルを 演奏可能な形式(.BGM) に変換します。

#### usage
```bash
$ vgs2mml mml-file BGM-file
```
- mml-file: _input file_
- BGM-file: _output file_

## vgs2rom
#### 概要
`ROMDATA.BIN` を生成する。

#### usage
```bash
$ vgs2rom directory ROMDATA.BIN
```
- directory: _input directory_
  - イメージファイルが配置されているディレクトリを指定すること
  - このコマンドは指定ディレクトリ直下の以下のファイルをイメージファイルとして認識する
    - `GSLOT000.CHR` 〜 `GSLOT255.CHR` : Graphics
    - `ESLOT000.PCM` 〜 `ESLOT255.PCM` : Sound effects
    - `BSLOT000.BGM` 〜 `BSLOT255.BGM` : BGMs
    - `DSLOT000.DAT` 〜 `DSLOT255.DAT` : immediate binary files
- ROMDATA.BIN: _output file_

## vgs2mkpj
#### 概要
VGSのプロジェクトを生成します。

#### usage
```
$ vgs2mkpj company Project directory
```
- company: _your company name_
  - 31byte 以下の ASCIIコード(8859_1) で指定
  - 先頭1バイト: 英字
  - 先頭2バイト目以降: 英字 or 数字
  - `指名名称.com` のドメインを取得していること（推奨）
- Project: _app project name_
  - 8byte 以下の ASCIIコード(8859_1) で指定
  - 先頭1バイト: 英大文字
  - 先頭2バイト目以降: 英字 or 数字
- directory: _output directory_
  - プロジェクトの配置先ディレクトリ
  - Windowsの場合、絶対パスしか指定できない
  - Linux or Mac OS Xの場合、絶対パス or 相対パスが指定できる

#### Java package name
Android版のJavaパッケージ名は、`com.company.Project` になる。

## vgs2play
#### 概要
BGMファイルをコマンドライン上で再生します。

#### usage
```bash
$ vgs2play BGM-file [mm:ss]
```
- BGM-file: _input file_
- [mm:ss]: _start playing time (optional)_

#### internal commands
`vgs2play` で演奏を開始すると 内部コマンド で __演奏の制御__ ができます。

##### 時間指定
```cmd
Command : j { sec | min:sec }
```
で指定した再生時間にジャンプ。

##### キーチェンジ（移調）
```cmd
Command : k+ 数字
Command : k- 数字
```

##### 特定チャネルのミュート・ミュート解除
```cmd
Command : mチャネル ch...
```

##### ポーズ/再開
```cmd
Command : p
```

##### リロード
```cmd
Command : r
```
BGMファイル自体を読み直し、ミュートや移調の設定をクリア

## vgs2pack
#### 概要
BGMファイルとmetaファイルを結合します。

#### usage
```
$ vgs2pack base-filename
```
- input(1): `base-filename.BGM`
- input(2): `base-filename.meta`
- output: `base-filename.vgs`

（補足）
- .BGM: vgs2mml コマンドで生成したBGMデータ
- .meta: [VGS Meta Data](https://github.com/suzukiplan/vgs2/blob/master/META.md)
- .vgs: VGS BGM Playerにインポートできる形式のファイル
- 
