Raspberry Pi クロスコンパイル実行手順
=====================================

ツールチェーンの取得
--------------------
以下のgitリポジトリより取得
https://github.com/raspberrypi/tools

$HOME/raspberrypi にcloneするものとする。

    $ mkdir $HOME/raspberrypi
    $ cd $HOME/raspberrypi
    $ git clone https://github.com/raspberrypi/tools.git

raspberrypi の ルートファイルシステム のコピー
----------------------------------------------
依存ライブラリやヘッダファイルを参照するために、raspberrypi本体から /lib /usr をコピーする。  
ツールチェーンの中にも sysroot が用意されているが、libcurl等がないためこの手順で構築する。  

    $ rsync -rl --delete-after --safe-links pi@[ip_addr]:/{lib,usr} $HOME/raspberrypi/rootfs

※ip_addr: pi@[ip_addr] の部分は、raspberrypi側の設定に合わせる

クロスコンパイル用cmakeファイルの編集
-------------------------------------
ツールチェーンのインストールディレクトリ、rootfsの展開先に合わせてSDKビルド環境の `cross/raspberrypi/pi.cmake` を編集する。
(上記手順の通りであれば修正不要)

ビルド実行
----------
cmake実行時に上記編集したクロスコンパイル用cmakeファイルを指定する。  
具体的には、`-DCMAKE_TOOLCHAIN_FILE=[SDKビルド環境]/cross/raspberrypi/pi.cmake` を付与する。  
cmake実行例

    $ cmake -DCMAKE_TOOLCHAIN_FILE=../cross/raspberrypi/pi.cmake -DCMAKE_INSTALL_PREFIX=out ../

以降は通常のビルド手順と同様である。
