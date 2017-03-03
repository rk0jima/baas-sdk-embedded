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

#ifndef NECBAAS_NBRESTEXECUTOR_H
#define NECBAAS_NBRESTEXECUTOR_H

//curlpp関連ヘッダのwarning抑止
#define CURLPP_GLOBAL_H

#include <string>
#include <curlpp/Easy.hpp>
#include "necbaas/nb_result.h"
#include "necbaas/nb_result_code.h"
#include "necbaas/nb_http_response.h"
#include "necbaas/internal/nb_http_request.h"
#include "necbaas/internal/nb_http_handler.h"
#include "necbaas/internal/nb_constants.h"

namespace necbaas {

/**
 * @class NbRestExecutor nb_rest_executor.h "necbaas/internal/nb_rest_executor.h"
 * REST APIを実行するクラス.
 * CURLライブラリを使用してREST APIを実行する。
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbRestExecutor {
public:
    /**
     * コンストラクタ.
     */
    NbRestExecutor();

    /**
     * デストラクタ.
     */
    ~NbRestExecutor();

    /**
     * ファイルアップロード実行.
     * @param[in]   request         HTTPリクエスト
     * @param[in]   file_name       ファイル名
     * @param[in]   timeout         RESTタイムアウト(秒)
     * @return      処理結果
     */
    NbResult<NbHttpResponse> ExecuteFileUpload(const NbHttpRequest &request, const std::string &file_name,
                                               int timeout = kRestTimeoutDefault);

    /**
     * ファイルダウンロード実行.
     * @param[in]   request         HTTPリクエスト
     * @param[in]   file_name       ファイル名
     * @param[in]   timeout         RESTタイムアウト(秒)
     * @return      処理結果
     */
    NbResult<NbHttpResponse> ExecuteFileDownload(const NbHttpRequest &request, const std::string &file_name,
                                                 int timeout = kRestTimeoutDefault);

    /**
     * REST実行(Jsonデータの送受信).
     * @param[in]   request         HTTPリクエスト
     * @param[in]   timeout         RESTタイムアウト(秒)
     * @return      処理結果
     */
    NbResult<NbHttpResponse> ExecuteJsonRequest(const NbHttpRequest &request, int timeout = kRestTimeoutDefault);
private:
    curlpp::Easy curlpp_easy_;    /*!< cURLppインスタンス */

    /**
     * CURLオプション 共通設定.
     * @param[in]   request         HTTPリクエスト
     * @param[in]   http_handler    HTTPハンドラ
     * @param[in]   timeout         RESTタイムアウト(秒)
     */
    void SetOptCommon(const NbHttpRequest &request, NbHttpHandler &http_handler, int timeout);

    /**
     * 処理結果生成.
     * @param[in]   http_handler    HTTPハンドラ
     * @param[in]   result_code     処理結果コード
     * @return      処理結果
     */
    NbResult<NbHttpResponse> MakeResult(NbHttpHandler &http_handler, NbResultCode result_code);

    /**
     * ダウンロードファイルサイズ検証.
     * X-Content-Length ヘッダに設定された値とダウンロードしたファイルサイズを比較する
     * @param[in]   response    HTTPレスポンス 
     * @param[in]   file_name   ファイルパス
     * @return      検証結果
     * @retval      true    ファイルサイズ一致
     * @retval      false   ファイルサイズ不一致 or X-Content-Lengthヘッダなし
     */
    bool ValidateFileSize(const NbHttpResponse &response, const std::string &file_name);
};
} //namespace necbaas
#endif //NECBAAS_NBRESTEXECUTOR_H
