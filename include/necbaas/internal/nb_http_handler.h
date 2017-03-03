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

#ifndef NECBAAS_NBHTTPHANDLER_H
#define NECBAAS_NBHTTPHANDLER_H

#include <string>
#include <vector>
#include "necbaas/nb_http_response.h"

namespace necbaas {

/**
 * @class NbHttpHandler nb_http_handler.h "necbaas/internal/nb_http_handler.h"
 * HTTPハンドラ.
 * CURLのデータ読み書き用コールバック関数を具備する。
 * 受信したデータをメンバ変数に保存し、HTTPレスポンスデータに変換する。
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbHttpHandler {
  public:
    /**
     * コンストラクタ.
     */
    NbHttpHandler();

    /**
     * デストラクタ.
     */
    virtual ~NbHttpHandler();

    /**
     * 送信データ読出し関数.
     * HTTPリクエスト送信データを提供するコールバック関数。
     * 本クラスでは未実装。
     */
    virtual size_t ReadCallback(void *buffer, size_t size, size_t nmemb);

    /**
     * 受信データ（ヘッダ）書込み関数.
     * HTTPレスポンスのヘッダデータをメンバ変数に書込みを行うコールバック関数。
     * @param[in]   buffer          受信データバッファ
     * @param[in]   size            データサイズ
     * @param[in]   nmemb           データ個数
     * @return      処理データサイズ
     */
    virtual size_t WriteHeaderCallback(void *buffer, size_t size, size_t nmemb);

    /**
     * 受信データ（ボディ）書込み関数.
     * HTTPレスポンスのボディデータをメンバ変数に書込みを行うコールバック関数。
     * @param[in]   buffer          受信データバッファ
     * @param[in]   size            データサイズ
     * @param[in]   nmemb           データ個数
     * @return      処理データサイズ
     */
    virtual size_t WriteCallback(char *buffer, size_t size, size_t nmemb);

    /**
     * 受信データ解析.
     * 受信データを解析し、NbHttpResponse型のデータを作成する。
     * @return      HTTPレスポンスデータ
     */
    NbHttpResponse Parse();

  protected:
    std::vector<std::string> response_headers_;    /*!< HTTPレスポンスヘッダ（１行毎の配列）*/
    std::vector<char> response_body_;              /*!< HTTPレスポンスボディ（バイナリ）    */

    /**
     * 受信データ解析(ステータスライン).
     * 受信データ(ヘッダ)の１行目を解析し、status-codeとreason-phraseを取り出す。
     * @param[out]   status_code        status-code
     * @param[out]   eson_phrase        reason-phrase
     */
    std::vector<std::string>::iterator ParseStatusLine(int *status_code, std::string *reason_phrase);

    std::vector<std::string>::iterator SearchStatusLine();
};
} //namespace necbaas

#endif //NECBAAS_NBHTTPHANDLER_H
