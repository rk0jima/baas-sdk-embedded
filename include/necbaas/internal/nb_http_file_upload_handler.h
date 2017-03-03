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

#ifndef NECBAAS_NBHTTPFILEUPLOADHANDLER_H
#define NECBAAS_NBHTTPFILEUPLOADHANDLER_H

#include <string>
#include <fstream>
#include "necbaas/internal/nb_http_handler.h"

namespace necbaas {

/**
 * @class NbHttpFileUploadHandler nb_http_file_upload_handler.h "necbaas/internal/nb_http_file_upload_handler.h"
 * HTTPハンドラ(ファイルアップロード用).
 * CURLのデータ読み書き用コールバック関数を具備する。
 * 受信したデータをメンバ変数に保存し、HTTPレスポンスデータに変換する。
 * 送信データ読出しコールバックを実装し、対象ファイルデータを送信する。
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbHttpFileUploadHandler : public NbHttpHandler {
  public:
    /**
     * コンストラクタ.
     * コンストラクタ実行後、IsError()を使用してエラー発生有無を確認すること。
     * @param[in]   file_name          アップロード対象ファイル名
     */
    explicit NbHttpFileUploadHandler(const std::string &file_name);

    /**
     * デストラクタ.
     */
    ~NbHttpFileUploadHandler();

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
     * 送信データ読出し関数.
     * 指定ファイルを読み出し、送信バッファに格納する。
     * @param[out]  buffer          送信データバッファ
     * @param[in]   size            データサイズ
     * @param[in]   nmemb           データ個数
     * @return      処理データサイズ
     */
    size_t ReadCallback(void *buffer, size_t size, size_t nmemb) override;
  private:
    std::ifstream file_stream_;   /*!< ファイルストリーム */
};
}

#endif //NECBAAS_NBHTTPFILEUPLOADHANDLER_H
