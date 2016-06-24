# tools for windows, windows 上で作業するためのちょっとしたツール

#### bup
make backup like pdumpfs

pdumpfs 的バックアップ。

日付＋時刻ディレクトリにバックアップする。変更が無いファイルは
リンクで済ますため、領域の節約になる。

また、特定の日時の全体が、断面として存在するので、
そのままコピーとかできる。


#### compression
set compressed attribute to file

ファイルへの圧縮属性のセット


#### hardlink
make link on NTFS

ファイルをリンクする。NTFS オンリー


#### diskrw
in tasktray, display disk access (read, write).

タスクトレイに常駐してディスクアクセスを表示。

PC がフリーズした時、CPU の負荷とディスクアクセスを見ていると、
本当のフリーズなのか、単に忙しいだけなのかの判断が付きやすい。
ただ、PC 本体を机の下なんかに置いていると、アクセスランプを
見るのが大変。なので、アクセスランプをモニタの中にって事で。


#### dmenu
in tasktray, make menu from desktop icon when right click

タスクトレイに常駐して、デスクトップにあるアイコンを右クリックでメニュー化する

Microsoft のツール集に DeskMenu ってのが有って、デスクトップのアイコンを
メニューにできて、すごく便利だった。ただ、不満があった。
それはサブディレクトリを見てくれなかった事。サブディレクトリを見てくれれば
デスクトップに溢れ返るショートカットをグループ化できたのに。

で、作ってみた。一覧の取得をファイル一覧の取得でやっているので
「全ユーザ」で有効になってるものは取れていない。
某所で IE の呼び出しを使うサンプルコードが出てて、こちらは
見事に動くのだが、サンプルなのでサブディレクトリに対応していない。
で、その I/F を使おうとしたのだが、ヘルプ他の説明が見つからない。



####  trashbox
move to trashbox. usefull, make shorcut in SendTo folder.

ゴミ箱に捨てる。ショートカットを作って「送る」フォルダに置いておくと便利



####  misc
misc tools. see 'misc' directory.

その他のツール。説明は misc ディレクトリの中
