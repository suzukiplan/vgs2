# VGS MML Reference

## 概要
- MML(Music Macro Language)とは、マクロを用いて音楽（楽譜）を記述する言語です
- VGSのMMLは、一般的なMMLと概ね互換性のある標準的な書式になっています

## Macro
VGSのMMLではマクロを定義できます。
```mml
$Hoge abcdefg
$Hige (Hoge) (Hoge)
$Hage (Hige) (Hige)
```

上記のように定義した場合、以下のように解釈されます。
- Hoge =「abcdefg」
- Hige =「abcdefg abcdefg」
- Hage =「abcdefg abcdefg abcdefg abcdefg」

マクロを駆使することで、連続した記述をシンプルに記述することができます。
なお、マクロの展開指定「(マクロ)」は、上の方の行で定義済みのマクロしか指定できない点を注意してください。

## Channels
- VGSの波形メモリ音源は __6チャネル__ です
- MMLでは各チャネルの情報を「Ch0 xxxxx」～「Ch5 xxxxx」という形式のレコード（行）で定義します。
- チャネル0（Ch0）のエンベロープ、ボリューム、比率は、若干意図しない形で鳴る場合があります
  - 再生毎に変化する訳ではないので、これは仕様としています
  - MMLの最初の行で「r1024」などのものすごく短い休符を全チャネルで鳴らせばこのCh0の謎仕様を回避できる場合があります
  - まぁ、ぶっちゃけバグ扱いで直してしまっても問題ないと思いつつ、実用上の問題はないのでそのままにしてあります

## Operand
- チャネルレコードには、下表のオペランドを定義することができます
- オペランドは大文字、小文字を区別しません
- オペランド間の区切り文字は不要です
- 見難い場合は、空白を入れるなどして調整してください

## Operands specification
- `M数字` : マスターボリュームを設定します。必ず最初の方に設定してください。後の方で設定すると、適切にフェードアウトできない場合があります。
- `T数字` : テンポを設定します
  - テンポは必ず最初の方に設定してください
  - 後の方で設定すると、色々なものがズレる場合があります
  - どうしてもTでルバート表現をしたい場合、全チャネルの再生開始地点が一致するタイミングを見計らい挿入してください
- `@0～3` : 音色を選択します
  - 0: 三角波
  - 1: ノコギリ波
  - 2: 矩形波
  - 3: ノイズ
- `V数字` : ベロシティーを設定します
  - ベロシティーの初期値は0（無音）です
  - 1以上の値を設定しなければ音が鳴りません
  - 0はミュートしたい時に使えます
- `V+` : ベロシティーを+の数だけインクリメントします
- `V-` : ベロシティーを-の数だけデクリメントします（ベロシティは符号なし整数値なので0未満にならないように気をつけてください）
- `O数字` : オクターブを設定します。オクターブの上限/下限は、実際に聴いて確認してみてください
  - なお、VGSの波形メモリ音源は、高音が若干音痴かもしれません
  - 低音は割りとキレイです
- `<` : オクターブをデクリメントします。
- `>` : オクターブをインクリメントします。
- `L数字` : 分解率の省略値を定義します。
  - 分解率とは、1なら全音符、2なら半音符（２分音符）、3なら3分音符、4なら4分音符という具合です
  - なお、VGSの波形メモリ音源では、存在しない分解率も指定できます（3分音符や1000分音符とか）
  - また、分解率の後にドット(.)を付与することで、付点音符（1.5倍の長さの音符）になります
  - 人間界には0分音符が存在しないので、人間は0分音符を指定しないでください
- `%数字` : キーオフのタイミングを設定します
  - 例えば%50で全音符を鳴らした場合、キーオフは2分音符分の長さの後になります
- `\S数字` : キーオンしてから出力が最大になるまでの時間をHz（1秒は22050Hz）で指定します（開始時エンベロープ）
- `\E数字` : キーオフしてから消音状態になるまでの時間をHz（1秒は22050Hz）で指定します（終了時エンベロープ）
- `P-数字` : 自動ノートダウンする間隔をHz（1秒は22050Hz）で指定します
  - ノートダウンとは、半音単位で自動的に音程が低くなることです
  - ベースのスライド（ダウン）に相当する演出やドラム音を鳴らす時などに使います
  - p-0を設定すれば解除できます
- `R[数字]` : 休符です
  - 数字は分解率です
  - 数字を省略時はLを参照します
- `|` : ループです
  - １回だけ書けます
  - VGSは22050をテンポ基準の値で分解率除算したw値で各チャネルが進行し、唯一の同期タイミングは曲の先頭のみであることを注意してループを設定する必要があります

## Note
- チャネルレコードには、ノートを定義することができます
- ノートとは、「ドレミファソラシ」のことです
- ノートは英字表記で表記します（ドレミファソラシ=CDEFGAB）
- ノートはオペランドと同様、大文字、小文字を区別しません
- また、ノート・オペランド間の区切り文字は不要です
- 見難い場合は、空白を入れるなどして調整してください。

ノートは、A音の場合、次のように表記します。
```mml
A[+|-][[数字[^数字]...]
```
- `+|-` : シャープ(+)またはフラット(b)の指定です（省略時はナチュラルになります）
- `数字` : 分解率です（オペランドLの説明を参照）
- `^数字` : タイを意味します（何個でも書けます）