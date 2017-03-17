Embedded SDK
===================================

# ビルド&テスト手順

ビルド手順
----------

任意のビルド用ディレクトリを作成し、cmakeを実行する。  
ここでは、トップディレクトリにbuildディレクトリを作成する。

[ビルドオプション]
* `-DCMAKE_BUILD_TYPE`：Debug版/Release版(デフォルト)
* `-DCMAKE_INSTALL_PREFIX`：インストール先ディレクトリ（デフォルト /usr/local/）

例としてDebug版、"out"ディレクトリを設定する。

    $ mkdir build
    $ cd build
    $ cmake3 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../out/ ../
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
