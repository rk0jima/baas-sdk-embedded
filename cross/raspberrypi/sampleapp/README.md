Raspberry Pi サンプルアプリビルド手順
=====================================

※Debian 8 でのビルド検証済  
※CentOS 7 ではリンクエラーとなりビルドできません

Enbedded SDKのビルド生成物の配置
--------------------------------
Enbedded SDKの `make install` の生成物を embeddedsdk ディレクトリに配置する。

クロスコンパイル用cmakeファイルの編集
-------------------------------------
Embedded SDKと同様にpi.cmakeをツールチェーンのパスに合わせて編集する。
また、CMakeLists.txt も同様に編集する。

ビルド実行
----------
pi.cmakeを指定してcmake, make を実行する。  
実行例

    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_TOOLCHAIN_FILE=../pi.cmake ../
    $ make

ライブラリ・実行ファイルのインストール
--------------------------------------
生成された実行ファイル、SDKライブラリをraspberrypiにインストールする。  
/usr/local/lib等のシステムディレクトリやLD_LIBRARY_PATHが通っているディレクトリに対象ファイルをコピーする。  
* sampleMain ： サンプルアプリ実行ファイル
* embeddedsdk/lib ： Enbedded SDK ライブラリ群






