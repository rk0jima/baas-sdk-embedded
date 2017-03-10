/*
 * COPYRIGHT (C) 2017 NEC CORPORATION
 *
 * ALL RIGHTS RESERVED BY NEC CORPORATION,  THIS PROGRAM
 * MUST BE USED SOLELY FOR THE PURPOSE FOR WHICH IT WAS
 * FURNISHED BY NEC CORPORATION,  NO PART OF THIS PROGRAM
 * MAY BE REPRODUCED OR DISCLOSED TO OTHERS,  IN ANY FORM
 * WITHOUT THE PRIOR WRITTEN PERMISSION OF NEC CORPORATION.
 *
 * NEC CONFIDENTIAL AND PROPRIETARY
 */

#ifndef NECBAAS_NBHTTPEFILEDOWNLOADHANDLER_H
#define NECBAAS_NBHTTPEFILEDOWNLOADHANDLER_H

#include <string>
#include <fstream>
#include "necbaas/internal/nb_http_handler.h"

namespace necbaas {

/**
 * @class NbHttpFileDownloadHandler nb_http_file_download_handler.h "necbaas/internal/nb_http_file_download_handler.h"
 * HTTPハンドラ(ファイルダウンロード用).
 * CURLのデータ読み書き用コールバック関数を具備する。
 * 受信したデータを指定ファイルに保存し、HTTPレスポンスデータに変換する。
 * 既にファイルが存在する場合は、上書きで書込みする。
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbHttpFileDownloadHandler : public NbHttpHandler {
  public:
    /**
     * コンストラクタ.
     * コンストラクタ実行後、IsError()を使用してエラー発生有無を確認すること。
     * @param[in]   file_name          アップロード対象ファイル名
     */
    explicit NbHttpFileDownloadHandler(const std::string &file_name);

    /**
     * デストラクタ.
     */
    ~NbHttpFileDownloadHandler();

    /**
     * エラー発生確認.
     * コンストラクタでエラーが発生した場合にtrueとなる。
     * 対象エラーは、ファイルオープンエラーである。
     * @return  確認結果
     * @retval  true    エラー発生
     * @retval  false   エラー未発生
     */
    bool IsError() const;

    /**
     * 受信データ書込み関数.
     * 受信データを指定ファイルに書込む。
     * ステータスコードが200台以外の場合は、基底クラスのメソッドで処理する。
     * @param[in]   buffer          受信データバッファ
     * @param[in]   size            データサイズ
     * @param[in]   nmemb           データ個数
     * @return      処理データサイズ
     */
    size_t WriteCallback(char *buffer, size_t size, size_t nmemb) override;
  private:
    std::ofstream file_stream_;   /*!< ファイルストリーム */
    int tmp_status_code_{0};      /*!< status-code        */

    /**
     * ステータスコード取得.
     * @return  status-code
     */
    int GetStatusCode();
};
} //namespace necbaas
#endif //NECBAAS_NBHTTPEFILEDOWNLOADHANDLER_H
