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

#ifndef NECBAAS_NBHTTPRESPONSE_H
#define NECBAAS_NBHTTPRESPONSE_H

#include <string>
#include <vector>
#include <map>

namespace necbaas {

/**
 * @class NbHttpResponse nb_http_response.h "necbaas/nb_http_response.h"
 * HTTPレスポンス.
 * 受信したHTTPレスポンスの情報の格納用。
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbHttpResponse {
   public:
    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>コンストラクタ.</p>
     */
    NbHttpResponse();

    /**
     * コンストラクタ.
     * @param[in]   status_code     status-code
     * @param[in]   reason_phrase   reason-phrase
     * @param[in]   headers         HTTPヘッダリスト
     * @param[in]   body            HTTPボディ
     */
    NbHttpResponse(int status_code, const std::string &reason_phrase,
                   const std::multimap<std::string, std::string> &headers, const std::vector<char> &body);

    /**
     * デストラクタ.
     */
    ~NbHttpResponse();

    /**
     * status-code取得.
     * @return      status-code
     */
    int GetStatusCode() const;

    /**
     * reason-phrase取得.
     * @return      reason-phrase
     */
    const std::string &GetReasonPhrase() const;

    /**
     * HTTPヘッダリスト取得.
     * @return      HTTPヘッダリスト
     */
    const std::multimap<std::string, std::string> &GetHeaders() const;

    /**
     * HTTPボディ取得.
     * @return      HTTPボディ
     */
    const std::vector<char> &GetBody() const;

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>ダンプ.</p>
     * ログにHTTPリクエスト情報を出力する
     */
    void Dump() const;

   private:
    int status_code_{0};                              /*!< status-code   */
    std::string reason_phrase_;                       /*!< reason-phrase */
    std::multimap<std::string, std::string> headers_; /*!< header-field  */
    std::vector<char> body_;                          /*!< message-body  */
};
}  // namespace necbaas

#endif  // NECBAAS_NBHTTPRESPONSE_H
