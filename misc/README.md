# misc tools for windows

その他。まぁ、個人的なツール類


#### backup.scm
make backup like pdumpfs, written by [Gauche](http://practical-scheme.net/gauche/index.html).

pdumpfs 的バックアップ。[Gauche](http://practical-scheme.net/gauche/index-j.html) で記述


日本語のファイル名でエラーになる。

[C での実装](../bup/) と比較してみても良いかも



#### m-mode.el
emacs 用、キーアサインの為のマイナーモード （Vz ライク）

一応、Linux 上と Windows 上の emacs で動いている。

~/.emacs で

```
(load "m-mode")
```

とする。

