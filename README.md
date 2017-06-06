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
* `-DARM_TEST`：ARMプロセッサ向けでのUT/FTビルドON/OFF。ARMビルドでもUTまたはFTのビルドを行わない場合は不要 (デフォルト OFF)

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
事前準備としてテスト用テナントを２つ作成し、functional_tests/ft_data.cc にテナント情報を記載する。

ビルド実施後、他の機器に実行ファイルを転送して実施する場合は、kFileDir に試験コマンドを実行するディレクトを記載する。相対パスでも可。（例）試験コマンドに移動して実行する場合：kFileDir = "./"  
また、試験コマンドを実行するディレクトリに試験用ファイルをディレクトリごと転送する（functional_tests/files）

試験コマンド実行方法

    $ ./functional_tests/functional_test [オプション]

オプションなしでの実行では、手動試験用のテストケースも動作してしまうため、
テストフィルターを使用する。

* `*Manual*` : 手動試験用テストケース
* `*SlowTest` : 時間を要するテスト

例）自動試験で時間がかかるテストをスキップ

    $ ./functional_tests/functional_test --gtest_filter=-*Manual*:*SlowTest

例）自動試験で時間がかかるテストも実行

    $ ./functional_tests/functional_test --gtest_filter=-*Manual*

例）オブジェクトストレージのテストで時間がかかるテストはスキップ

    $ ./functional_tests/functional_test --gtest_filter=NbObjectFT.*:-*SlowTest

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

