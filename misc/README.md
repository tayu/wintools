# misc tools for windows

その他。まぁ、個人的なバックアップとして


#### backup.scm
make backup like pdumpfs, written by [Gauche](http://practical-scheme.net/gauche/index.html).

pdumpfs 的バックアップ。[Gauche](http://practical-scheme.net/gauche/index-j.html) で記述


日本語のファイル名でエラーになる。

[C での実装](../bup/) と比較してみても良いかも



#### change-ext.vbs
change file ext. make shortcut and D&D to this

ファイルの拡張子の変更。複数をまとめてショートカットへドラッグ＆ドロップ
する。



#### move-to-picture.vbs
move file to folder, if same name is exist, make new name with random char.

特定のフォルダに移動する。ファイル名が重複する場合、ランダムに
ファイル名に文字を付加する。名前は画像を貯めておくのに使ってるから



#### m-mode.el
emacs 用、キーアサインの為のマイナーモード （Vz ライク）

一応、Linux 上と Windows 上の emacs で動いている。

~/.emacs で

```
(load "m-mode")
```

とする。

