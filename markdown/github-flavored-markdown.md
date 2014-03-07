GitHub Help
https://help.github.com/categories/88/articles

GitHub Flavored Markdown
https://help.github.com/articles/github-flavored-markdown




GitHub Help

    Contact Support
    Return to GitHub

Writing on GitHub / GitHub Flavored Markdown
Article last updated on 07-Feb-14


GitHub Flavored Markdown
GitHub 風味マークダウン

GitHub uses what we call "GitHub Flavored Markdown" (GFM) everywhere you can use Markdown -- in files, issues, and comments. It differs from standard Markdown (SM) in a few significant ways and adds some additional functionality.
GitHub は「GitHub 風味のマークダウン」(GFM) と呼ばれるものを、マークダウンが使えるところではどこでも使っています -- ファイル、課題、コメントなどで。これは標準のマークダウン (SM) とは幾つかの（でも重要な）点で異なっています。また、機能の拡張もされています。、



If you're not already familiar with Markdown, take a look at Markdown Basics. If you'd like to know more about features that are available in issues, comments, and pull request descriptions, read Writing on GitHub.
もし、マークダウンに慣れていないなら、Markdown Basics を見てください。機能や、それによって可能になる事柄（個別の項目、コメント、pull リクエスト時の説明文などの場面で）について、もっと知りたい場合は Writing on GitHub を読んでください。


Differences from traditional Markdown
伝統的な マークダウン との違い


Multiple underscores in words
複数の下線（'_'）

It is not reasonable to italicize just part of a word, especially when you're dealing with code and names often appear with multiple underscores. Therefore, GFM ignores multiple underscores in words.

イタリック体にする為に、単語を個々に囲む方法はリーズナブルではありまぜん。特に、良くあるような、コード中で名前を多数の下線（'_'）で繋いでいる場合には。なので、GFM では単語（複合語）の中の下線（'_'）を無視します。



perform_complicated_task
do_this_and_do_that_and_another_thing

becomes
以下の様になります

perform_complicated_task
do_this_and_do_that_and_another_thing



URL autolinking
URL の自動リンク化


GFM will autolink standard URLs, so if you want to link to a URL (instead of setting link text), you can simply enter the URL and it will be turned into a link to that URL.
GFM では標準的な URL 表記を自動でリンク化します。なので、URL にリンクする（かつ、リンクテキストを設定しない）場合は、シンプルに URL を記述するだけで、そこへのリンクとなります。

http://example.com

becomes
以下の様になります

http://example.com


Strikethrough
打消し線


GFM adds syntax to create strikethrough text, which is missing from standard Markdown.
GFM は打消し線の構文を追加しました。これは標準のマークダウンに無いものです。



~~Mistaken text.~~

becomes
以下の様になります

Mistaken text.




Fenced code blocks
囲みコードブロック

Standard Markdown converts text with four spaces at the beginning of each line into a code block; GFM also supports fenced blocks. Just wrap your code in ``` (as shown below) and you won't need to indent it by four spaces. Note that although fenced code blocks don't have to be preceded by a blank line―unlike indented code blocks―we recommend placing a blank line before them to make the raw Markdown easier to read.
標準のマークダウンは 4 個のスペースを各行の先頭に書く事でコードブロックに変換します。GFM では、囲まれたブロックをサポートします。コード部を（'```'）で囲むだけです（下記にある様に）。4 個のスペースによるインデントは要りません。
注意。囲みコードブロックは空行の後に続ける必要はありません。インデントされたコードブロックと違うもの。私達はコードブロックの前に空行を置くことを推奨します。そうする事でマークダウンのソースが読みやすくなります。


Here's an example:

```
function test() {
  console.log("notice the blank line before this function?");
}
```

Keep in mind that, within lists, you must indent non-fenced code blocks eight spaces to render them properly.
参考まで。リストでは囲まれないコードブロックを正しくインデントさせるために 8 個のスペースを使います。



Syntax highlighting
シンタックスハイライト

Code blocks can be taken a step further by adding syntax highlighting. In your fenced block, add an optional language identifier and we'll run it through syntax highlighting. For example, to syntax highlight Ruby code:
コードブロックでは更なる機能、シンタックスハイライトできます。囲みコードブロックにオプションの言語指定することでシンタックスハイライトになります。例として Ruby の場合：

```ruby
require 'redcarpet'
markdown = Redcarpet.new("Hello World!")
puts markdown.to_html
```

We use Linguist to perform language detection and syntax highlighting. You can find out which keywords are valid by perusing the languages YAML file.
言語の選択とシンタックスハイライトには Linguist を使います。使えるキーワードの詳細は the languages YAML file にあります。




Tables
テーブル


You can create tables by assembling a list of words and dividing them with hyphens - (for the first row), and then separating each column with a pipe |:
テーブルを組み立てる事ができます。ハイフン（'-'）で区切り（最初の行のみ）、パイプ（'|'）で各列を区切ります。


First Header  | Second Header
------------- | -------------
Content Cell  | Content Cell
Content Cell  | Content Cell

For aesthetic purposes, you can also add extra pipes on the ends:
見やすくする為に、端にパイプ（'|'）を置くこともできます。

| First Header  | Second Header |
| ------------- | ------------- |
| Content Cell  | Content Cell  |
| Content Cell  | Content Cell  |


Note that the dashes at the top don't need to match the length of the header text exactly:
参考。ハイフン（'-'）行の長さはヘッダ行と正確に一致する必要はありません。


| Name | Description          |
| ------------- | ----------- |
| Help      | Display the help window.|
| Close     | Closes a window     |


You can also include inline Markdown such as links, bold, italics, or strikethrough:
行中にも Markdown で書く事ができます。リンク、太字、イタリック体、打消し線などなど：

| Name | Description          |
| ------------- | ----------- |
| Help      | ~~Display the~~ help window.|
| Close     | _Closes_ a window     |


Finally, by including colons : within the header row, you can define text to be left-aligned, right-aligned, or center-aligned:
最後に。コロン（':'）。ヘッダ行でテキストの左寄せ、中央寄せ、右寄せを指定できます。


| Left-Aligned  | Center Aligned  | Right Aligned |
| :------------ |:---------------:| -----:|
| col 3 is      | some wordy text | $1600 |
| col 2 is      | centered        |   $12 |
| zebra stripes | are neat        |    $1 |

A colon on the left-most side indicates a left-aligned column; a colon on the right-most side indicates a right-aligned column; a colon on both sides indicates a center-aligned column.

左端のコロン（':'）は左寄せ、右端のコロン（':'）は右寄せ、両側のコロン（':'）は中央寄せを表します。



(c) year GitHub Inc. All rights reserved.

    Terms of Service
    Privacy
    Security

