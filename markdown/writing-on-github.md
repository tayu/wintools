GitHub help
https://help.github.com/categories/88/articles

Writing on GitHub
https://help.github.com/articles/writing-on-github



GitHub Help

    Contact Support
    Return to GitHub

Writing on GitHub / Writing on GitHub
Article last updated on 07-Feb-14



Writing on GitHub
GitHub で書く事

Issues, comments, and pull request descriptions are written using GitHub Flavored Markdown along with some additional features to make writing content on GitHub easy.
課題、コメント、そして pull リクエストの説明文。これらは GitHub 風味のマークダウンで書かれる。そこにはいくつかの機能が加えられており、ユーザが GitHub で書く事を簡単に、お手軽にしてくれる。



Markup
マークアップ

Newlines
改行

The biggest difference with writing on GitHub is the way we handle linebreaks. With Markdown, you can hard wrap paragraphs of text to have them combine into a single paragraph. We find this causes a huge number of unintentional formatting errors. GitHub treats newlines in paragraph-like content as real line breaks, which is usually what you intended.
GitHub 上で書く事の最大の違いは改行の取り扱いだ。マークダウンではテキストをきっちりと囲む事で 1 つの段落にする事が出来る。私達は、これが、数多くの意図しない整形のエラーを引き起こす事を見つけた。GitHub は段落に見えるひと塊のテキストの中の改行を本当の改行の様に扱う。それ（改行）は通常、意図されたものだから。

The next paragraph contains two phrases separated by a single newline character:
次の段落は 2 つの文からなり、 1 つの改行で分けられている。

Roses are red
Violets are blue

becomes
次の様になる

Roses are red
Violets are blue


Task Lists
タスクリスト


Lists can be turned into Task Lists by prefacing list items with [ ] or [x] (incomplete or complete, respectively).
リストはリストの各項に [ ] や [x] （未完または完了、個々に記述）を置くことでタスクリストにできる。


- [x] @mentions, #refs, [links](), **formatting**, and <del>tags</del> are supported
- [x] list syntax is required (any unordered or ordered list supported)
- [x] this is a complete item
- [ ] this is an incomplete item


Task lists render with checkboxes that you can check on and off in Issues, Comments, Pull Requests, Gist markdown files, and Gist comments.
タスクリストはチェックボックスに変換される。また、on または off のマークが入る。イシュー、コメント、プルリクエスト、概要のマークダウンファイル、概要のコメントで使える。

See the Task Lists blog post for more details.
詳細は Task Lists blog post を参照のこと



References
参照

Certain references are auto-linked:
確実な参照は自動でリンクになる

* SHA: 17bb13a686306dd0e95428d89311e45342532db0
* User@SHA: mojombo@17bb13a686306dd0e95428d89311e45342532db0
* User/Repository@SHA: mojombo/jekyll@17bb13a686306dd0e95428d89311e45342532db0
* #Num: #1
* User#Num: mojombo#1
* User/Repository#Num: mojombo/jekyll#1

becomes
以下の様になる

    SHA: 17bb13a
    User@SHA: mojombo@17bb13a
    User/Project@SHA: mojombo/jekyll@17bb13a
    #Num: #1
    User#Num: mojombo#1
    User/Project#Num: mojombo/jekyll#1

Features
その他の機能


Quick quoting
簡単な引用

Typing r on your keyboard lets you reply to the current issue or pull request with a comment. Any text you select within the discussion thread before pressing r will be added to your comment automatically and formatted as a blockquote.
キーボードで 'r' を打つと現在のイシューかプルリクエストをコメント付きで受け取る。'r' を打つ前に、議論スレッドに選択済みテキストがあれば、コメントに自動的に付加され、また、blockquote として整形される。


Name and Team @mentions autocomplete
名前とチーム。

Typing an @ symbol will bring up a list of people or teams on a project. The list filters as you type, so once you find the name of the person or team you are looking for, you can use the arrow keys to select it and hit either tab or enter to complete the name. For teams, just enter the @organization/team-name and all members of that team will get subscribed to the issue.
'@' を入力するとプロジェクトのメンバやチームの名前のリストが表示される。リストの絞り込みには、更に入力する。探していた人やチームの名前が見つかったら、矢印キーで選択できる。また、タブやリターンキーで補完できる。チーム名の場合、'@' ＋組織名／チーム名でそのイシューに署名している全メンバ名が取得できる。




The autocomplete results are restricted to repository collaborators and any other participants on the thread, so it's not a full global search. It uses the same fuzzy filter as the file finder and works for both usernames and full names.
自動補完の結果はリポジトリの関係者、スレッドの他の参加者に限定される。全体を検索対象していない。これは file finder の様な大まかなフィルタである。ユーザ名とフルネームに対して使用できる。

Check out the blog posts for more information about @mention autocompletes for users and teams.
ユーザ、チームの @mention 自動補完についてねより多くの情報はブログをチェックのこと。




Emoji autocomplete
絵文字自動補完


Typing : will bring up a list of suggested emoji. The list will filter as you type, so once you find the emoji you're looking for, either hit tab or enter to complete the highlighted result.
':' を入力すると絵文字の提案リストが表示される。リストは入力によって絞り込みされる。探していた絵文字が見つかったら、タブかリターンの入力でハイライトされた結果として確定される。


Issue autocompletion
課題の自動補完


Typing # will bring up a list of suggested Issues and Pull Requests. Type a number or some text to filter the list, then hit either tab or enter to complete the highlighted result.
'#' を入力すると、課題とプルリクエストの提案リストが表示される。幾つかの数字かテキストの入力によって絞り込みされる。タブかリターンの入力でハイライトされた結果として確定される。




Zen Mode (fullscreen) writing
禅モード（フルスクリーン）で書く

Zen Mode allows you to write markdown in fullscreen mode. You'll find the Zen Mode button on comment, issue, and pull request forms across the site.
禅モードではフルスクリーンでマークダウンの記述ができる。禅モードボタンはコメント、課題、他サイトからのプルリクエスト時に使える。

Zen Mode button
禅モードボタン

You can also use it when creating and editing files by using the Zen Mode button above the file box.
禅モードボタンは、ファイルの作成／編集時にの file box でも使える。



Zen Mode comes with both a light and a dark theme. You can change your theme using the switcher at the top right of the window.
禅モードは白黒のテーマである。テーマの切り替えはウインドウの右上のスイッチャーからもできる。


Check out the blog post for more information about Zen writing mode.
禅モードでについての情報は、ブログをチェックのこと。


See also
Markdown Basics GitHub Flavored Markdown


(c) year GitHub Inc. All rights reserved.

    Terms of Service
    Privacy
    Security
