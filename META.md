# VGS BGM Meta Data Specification

## 解説
- vgs2mmlコマンドで生成したBGMファイルには, 音楽の演奏情報のみが格納されています
- vgs2packコマンドを用いることで, BGMファイルにメタ情報を付与した `.vgsファイル` を生成できます
- VGS BGM Player では, メタ情報が付与された `.vgsファイル` のみインポートできます

## メタ情報
### 仕様
- メタ情報には __1つのGeneralレコード__ と __1つ以上のDataレコード__ が含まれます
- 1つ目のDataレコードは、作品の原作情報を定義するためのものです
- 2つ目のDataレコードは、作品の二次創作情報を定義するためのものです
- 最大、256個のDataレコードを定義できます（最大二百五十六次創作まで）
- 文字コードは必ず UTF-8 で記述することとします
- 数値データは必ず Big Endian とします

### Generalレコード
|パラメタ|設定例|最大サイズ|解説|
|---|---|---|---|
|format|VGS1.0|7byte|データ形式を示します（現状 `VGS1.0` のみ）|
|genre|Classic|31byte|曲（原曲）のジャンルを示します|

### Dataレコード
|パラメタ|設定例|最大サイズ|解説|
|---|---|---|---|
|year|2004|16bit uint|曲のCopyright年|
|aid|128|16bit uint|アルバム種別識別子（東方BGM on VGS専用パラメタ）|
|track|4|8bit uint|曲のトラック番号|
|album|東方永夜抄　〜 Imperishable Night.|55byte|曲が格納されているアルバム｜
|song|夜雀の歌声　〜 Night Bird|63byte|曲の名称|
|team|上海アリス幻樂団|31byte|曲作成者の所属団体など|
|creator|ZUN|31byte|曲作成者|
|right|東方Project二次創作|31byte|曲の利用権利情報|
|code|12345678|31byte|曲の許諾番号などの識別子|

項目は省略することができます

## 具体例
```
format = VGS1.0
genre = Game Music

Data:
year = 2004
aid = 128
track = 4
album = 東方永夜抄　〜 Imperishable Night.
song = 夜雀の歌声　〜 Night Bird
team = 上海アリス幻樂団
creator = ZUN

Data:
year = 2013
track = 41
album = 東方BGM on VGS
team = SUZUKI PLAN
creator = Yoji Suzuki
right = 東方Project二次創作
```

