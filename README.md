# Video Game System mk-II SR
## 概要
- `Linux` , `Mac OS X` , `Windows` , `Android` 及び `iOS` 対応のアプリを開発する `SDK` です
- 全てのプラットフォームのゲームの実装を一本化できる特徴があります
- 使用するプログラム言語は `C/C++言語` のみです

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
  - `DSLOTxxx.DAT` に任意データリソースを準備
- `vgs2rom` コマンドで ROMDATA.BIN を生成
- ビルド（後述）

## BGM機能だけ使いたい場合
- vgs-bgm-decoder を使用してください。
- 詳しい使い方は vgs2play コマンドのソースコードを参照してください。

## ライセンス
- 2-Clause BSDライセンスで提供しています
- [LICENSE.txt](https://github.com/suzukiplan/vgs2/blob/master/LICENSE.txt)を参照してください
- 本リポジトリに含まれるソースコードの 一部 若しくは 全部 を流用 又は `VGS mk-II SR`を用いて製作されたプログラムを配布又は販売する場合、プログラムの利用者が参照できるドキュメント（スマートフォンアプリの場合は公開するアプリストアに掲載される説明文 又は アプリ内で参照できるテキスト）に [LICENSE.txt](https://github.com/suzukiplan/vgs2/blob/master/LICENSE.txt) 記載の Copyright 表記を記載してください
- 本リポジトリに含まれるソースコードの 一部 若しくは 全部 を流用 又は `VGS mk-II SR`を用いて製作されたプログラムを配布又は販売の結果発生した損失や損害等につき、当方は一切の責任を負わないものとします

## OS別セットアップ・ビルド手順
以下のドキュメントを参照してください。
- `Linux` : [README-Linux.md](https://github.com/suzukiplan/vgs2/blob/master/README-Linux.md)
- `Mac OS X` : [README-MacOSX.md](https://github.com/suzukiplan/vgs2/blob/master/README-MacOSX.md)
- `Windows` : [README-Windows.md](https://github.com/suzukiplan/vgs2/blob/master/README-Windows.md)

## ROMDATA.BIN
VGSでは、グラフィックス、効果音、音楽、任意データ（ゲームのステージ情報など）を `vgs2rom` コマンドでロムデータ形式に結合したもの ( `ROMDATA.BIN` ) がゲームのアセット情報となります。

#### GSLOT (Graphics)
- `GSLOT000.bmp` 〜 `GSLOT255.bmp` というファイル名で `romdata` ディレクトリに配置します
- ファイル名の `000` 〜 `255` は `vgs2_putBG関数` や `vgs2_putSP関数` で指定する `slot番号(n)` に対応します
- `GSLOTxxx.bmp` は 8bitカラー(256色) かつ 256 x 256 ピクセル以下のbitmap形式にする必要があります
- 新しい `GSLOTxxx.bmp` を追加する場合, `romdata/makefile` ファイルを編集してください

#### ESLOT (Sound Effect)
- `ESLOT000.wav` 〜 `ESLOT255.wav` というファイル名で `romdata` ディレクトリに配置します
- ファイル名の `000` 〜 `255` は `vgs2_eff関数` や `vgs2_effstop関数` で指定する `slot番号(n)` に対応します
- `ESLOTxxx.wav` は 22050Hz, 16bit, 1ch(mono) の wav形式でなければなりません
- 新しい `ESLOTxxx.wav` を追加する場合, `romdata/makefile` ファイルを編集してください

#### BSLOT (BGM)
- `BSLOT000.mml` 〜 `BSLOT255.mml` というファイル名で `romdata` ディレクトリに配置します
- ファイル名の `000` 〜 `255` は `vgs2_bplay関数` で指定する `slot番号(n)` に対応します
- `BSLOT000.mml` は VGS独自形式の Music Macro Language です
- MMLの書き方は[MML.md](https://github.com/suzukiplan/vgs2/blob/master/MML.md)を参照してください
- 新しい `BSLOTxxx.mml` を追加する場合, `romdata/makefile` ファイルを編集してください

#### DSLOT (Data)
- `DSLOT000.DAT` 〜 `DSLOT255.dat` というファイル名で `romdata` ディレクトリに配置します
- ファイル名の `000` 〜 `255` は `vgs2_getdata関数` で指定する `slot番号(n)` に対応します
- DSLOTのファイル内容は不問です
- マップデータ や キャラクタデータ など、任意形式のデータを VGSアプリ で用いる時に使います

#### romdata/makefile
`romdata/makefile` では以下の手続きを実行しています。
- `GSLOTxxx.bmp` を `vgs2bmp` コマンドで `CHR` 形式というVGS独自形式に変換
- `ESLOTxxx.wav` を `vgs2wav` コマンドで `PCM` 形式というVGS独自形式に変換
- `BSLOTxxx.mml` を `vgs2mml` コマンドで `BGM` 形式というVGS独自形式に変換
- `GSLOTxxx.CHR` , `ESLOTxxx.PCM` , `BSLOTxxx.BGM` , `DSLOTxxx.DAT` を `vgs2rom` コマンドで `ROMDATA.BIN` に結合

## API仕様
[API.md](https://github.com/suzukiplan/vgs2/blob/master/API.md) を参照してください。

## Command仕様
[Command.md](https://github.com/suzukiplan/vgs2/blob/master/Command.md) を参照してください。

## ハードウェア仕様
- 最大同時発色数: 16777216色中256色
- 解像度: 160 x 200
- VRAM: スプライト1面 + BG1面
- CPU
  - 現状 none-emulation (各対応OSのnative codeにcompile & 実行)
  - 将来的には独自の仮想CPUにしたい
  - 発想的には WebAssembly に近いので、WebAssembly の仕様がいい感じだったら乗っかるかもしれない（いい感じではなかったら、やはり独自開発する）
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
  - グラフィックス(GSLOT): 最大256x256ピクセル x 最大256枚
  - 効果音（ESLOT）: 22050Hz, 16bit, 1ch(モノラル) x 最大256個
  - BGM（BSLOT）: 最大65,536ノート × 最大256個
  - データ（DSLOT）: 任意データ × 最大256個
