/*
 * Copyright (C) 2017 NEC Corporation
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
    virtual ~NbRestExecutor();

    /**
     * ファイルアップロード実行.
     * @param[in]   request         HTTPリクエスト
     * @param[in]   file_path       ファイルパス
     * @param[in]   timeout         RESTタイムアウト(秒)
     * @return      処理結果
     */
    virtual NbResult<NbHttpResponse> ExecuteFileUpload(const NbHttpRequest &request, const std::string &file_path,
                                                       int timeout = kRestTimeoutDefault);

    /**
     * ファイルダウンロード実行.
     * @param[in]   request         HTTPリクエスト
     * @param[in]   file_path       ファイルパス
     * @param[in]   timeout         RESTタイムアウト(秒)
     * @return      処理結果
     */
    virtual NbResult<NbHttpResponse> ExecuteFileDownload(const NbHttpRequest &request, const std::string &file_path,
                                                         int timeout = kRestTimeoutDefault);

    /**
     * REST実行(データの送受信).
     * @param[in]   request         HTTPリクエスト
     * @param[in]   timeout         RESTタイムアウト(秒)
     * @return      処理結果
     */
    virtual NbResult<NbHttpResponse> ExecuteRequest(const NbHttpRequest &request, int timeout = kRestTimeoutDefault);

protected:
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
    bool ValidateFileSize(const NbHttpResponse &response, const std::string &file_name) const;

    /**
     * REST実行開始.
     */   
    virtual void Execute();
};
} //namespace necbaas
#endif //NECBAAS_NBRESTEXECUTOR_H
