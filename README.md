Embedded SDK
===================================

# ビルド&テスト手順
ここではIntel社製プロセッサ向けのビルド手順を示す。  
RaspberryPi向けのクロスコンパイル手順は、cross/raspberrypi/README.md を参照。

事前準備
--------
ビルドターゲットに対して必要なパッケージをインストールする。
* libcurl-devel(Intel 64bit)
* glibc-devel.i686(Intel 32bit)
* libcurl-devel.i686(Intel 32bit)

    $ sudo yum install libcurl-devel

ビルド手順
----------
ビルド環境をGitリポジトリから取得した場合は、以下手順を実施して外部ライブラリの取得を実施してください。

    $ git submodule init
    $ git submodule update
任意のビルド用ディレクトリを作成し、cmakeを実行する。  
ここでは、トップディレクトリにbuildディレクトリを作成する。

[ビルドオプション]
* `-DUNIT_TESTS`：UTコードのコンパイルON/OFF (デフォルト OFF)
* `-DFUNCTIONAL_TESTS`：FTコードのコンパイルON/OFF (デフォルト OFF)
* `-DCMAKE_BUILD_TYPE`：Debug版/Release版 (デフォルト Release版)
* `-DCMAKE_INSTALL_PREFIX`：インストール先ディレクトリ（デフォルト /usr/local/）
* `-DIA32`：Intel 32ビットプロセッサ向けビルドON/OFF (デフォルト OFF)

例としてUTコンパイルあり、Debug版、"out"ディレクトリ、Intel 32bitビルドを設定する。

    $ mkdir build
    $ cd build
    $ cmake3 -DUNIT_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../out/ -DIA32=ON ../
ビルド実行

    $ make
    $ make install

Unitテスト
----------
    $ make test

テスト結果の詳細は、`Testing/Temporary/LastTest.log` で確認可能。

テストコマンドを直接実行してもよい。

    $ ctest
もしくは

    $ unit_tests/unit_test

Functionテスト
--------------
T.B.D.

Coverage
--------
ルートのCMakeLists.txtを編集してCMAKE_CXX_FLAGSに`--coverage`を付与する。  
ビルド＆Unitテスト実行後、gcovコマンドを使用してカバレッジデータを集計する。

    $ cd [ビルドディレクトリ]
    $ [SDKディレクトリ]/unit_tests/tool/coverage.sh

`gcov/coverage.cc`,`gcov/coverage.h` が作成されるので、集計結果を確認する。  
カバレッジが100%に到達していないものは、\*.gcov ファイルを確認する。  
ヘッダファイルは、インライン展開された.ccファイルで計測されるため、
個別でgcovコマンドで確認する。

Doxygen
-------
* `doc/Doxyfile` : ユーザ向けSDK APIリファレンス作成用
* `doc/Doxyfile.inter` : 内部APIを含む全クラスのAPIリファレンス作成用

HTML作成(doxygen 1.8.12で動作確認済)

    $ doxygen [doxyfile]

