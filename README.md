Embedded SDK
===================================

ビルド&テスト手順
----------

* ビルド

任意のビルド用ディレクトリを作成し、cmakeを実行する。

ここでは、トップディレクトリにbuildディレクトリを作成する。

[ビルドオプション]

-DCMAKE_BUILD_TYPE：Debug版/Release版(デフォルト)

-DCMAKE_INSTALL_PREFIX：インストール先ディレクトリ（デフォルト /usr/local/）

例としてDebug版、"out"ディレクトリを設定する。
~~~
    $ mkdir build
    $ cd build
    $ cmake3 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../out/ ../
~~~

ビルド実行
~~~
    $ make
    $ make install
~~~

* テスト

T.B.D.
