/*
 * Copyright (C) 2017-2019 NEC Corporation
 */

#ifndef NECBAAS_NBSERVICE_H
#define NECBAAS_NBSERVICE_H

#include <string>
#include <memory>
#include "necbaas/internal/nb_session_token.h"
#include "necbaas/internal/nb_rest_executor.h"
#include "necbaas/internal/nb_rest_executor_pool.h"
#include "necbaas/internal/nb_http_request_factory.h"

namespace necbaas {

/**
 * @class NbService nb_service.h "necbaas/nb_service.h"
 * MBaaS サービスクラス. MBaaS 機能を提供するメインクラス.
 */
class NbService {
   public:
    /**
     * サービスインスタンス生成.
     * サービスインスタンスは、本メソッドを使用して取得する.
     * proxy以外のパラメータは必須である。必須パラメータが空文字の場合はREST実行時にエラーが発生する。
     * proxyに空文字を設定した場合はProxyを使用しない。Proxyを使用する場合は必ず設定すること。
     * @param[in]   endpoint_url    Endpoint URI
     * @param[in]   tenant_id       テナントID
     * @param[in]   app_id          アプリケーションID
     * @param[in]   app_key         アプリケーションキー
     * @param[in]   proxy           Proxy URL "[scheme]://[hostname]:[port]"<br>
     *                              デフォルト：Proxy無効(空文字)<br>
     *                              例）"http://proxyhost:8080"
     * @return      サービスインスタンス
     */
    static std::shared_ptr<NbService> CreateService(const std::string &endpoint_url, const std::string &tenant_id,
                                                    const std::string &app_id, const std::string &app_key,
                                                    const std::string &proxy = "");

    /**
     * ロギング設定（デバッグログ）.
     * プロセス内の全サービスに対して設定される。<br>
     * default設定: Debug版: 有効、Release版: 無効
     * @param[in]   flag    true:有効／false:無効
     */
    static void SetDebugLogEnabled(bool flag);

    /**
     * ロギング設定（エラーログ）.
     * プロセス内の全サービスに対して設定される。<br>
     * default設定: Debug版: 有効、Release版: 無効
     * @param[in]   flag    true:有効／false:無効
     */
    static void SetErrorLogEnabled(bool flag);

    /**
     * ロギング設定（RESTログ）.
     * プロセス内の全サービスに対して設定される。<br>
     * default設定: Debug版: 有効、Release版: 無効
     * @param[in]   flag    true:有効／false:無効
     */
    static void SetRestLogEnabled(bool flag);

    /**
     * デストラクタ.
     */
    virtual ~NbService();

    /**
     * アプリケーションID取得.
     * @return  アプリケーションID
     */
    const std::string &GetAppId() const;

    /**
     * アプリケーションキー取得.
     * @return  アプリケーションキー
     */
    const std::string &GetAppKey() const;

    /**
     * Endpoint URI 取得.
     * @return  Endpoint URI
     */
    const std::string &GetEndpointUrl() const;

    /**
     * テナントID取得.
     *  @return  テナントID
     */
    const std::string &GetTenantId() const;

    /**
     * Proxy URL取得.
     * @return  Proxy URL
     */
    const std::string &GetProxy() const;

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>セッショントークン取得.</p>
     * @return  セッショントークン
     */
    NbSessionToken GetSessionToken();

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>セッショントークン設定.</p>
     * @param[in]   token    セッショントークン
     */
    virtual void SetSessionToken(const NbSessionToken &token);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>セッショントークン削除.</p>
     */
    void ClearSessionToken();

    /**
     * HTTPアクセスのオプション設定.
     * @param[in]   options    HTTPオプション
     */
    void SetHttpOptions(const NbHttpOptions &options);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>REST実行(データの送受信).</p>
     * @param[in]   create_request  HTTPリクエスト作成関数ポインタ
     * @param[in]   timeout         タイムアウト値(秒)
     * @return      処理結果
     */
    NbResult<NbHttpResponse> ExecuteRequest(std::function<NbHttpRequest(NbHttpRequestFactory &)> create_request, int timeout);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>ファイルダウンロード実行.</p>
     * @param[in]   create_request  HTTPリクエスト作成関数ポインタ
     * @param[in]   file_path       ファイルパス
     * @param[in]   timeout         タイムアウト値(秒)
     * @return      処理結果
     */
    NbResult<NbHttpResponse> ExecuteFileDownload(std::function<NbHttpRequest(NbHttpRequestFactory &)> create_request,
                                                 const std::string &file_path, int timeout);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>ファイルアップロード実行.</p>
     * @param[in]   create_request  HTTPリクエスト作成関数ポインタ
     * @param[in]   file_path       ファイルパス
     * @param[in]   timeout         タイムアウト値(秒)
     * @return      処理結果
     */
    NbResult<NbHttpResponse> ExecuteFileUpload(std::function<NbHttpRequest(NbHttpRequestFactory &)> create_request,
                                               const std::string &file_path, int timeout);
   private:
    std::string app_id_;                    /*!< アプリケーションID */
    std::string app_key_;                   /*!< アプリケーションキー */
    std::string endpoint_url_;              /*!< Endpoint URI */
    std::string tenant_id_;                 /*!< テナントID */
    std::string proxy_;                     /*!< Proxy URL */
    NbHttpOptions http_options_;            /*!< HTTPオプション */
    NbSessionToken session_token_;          /*!< セッショントークン */
    NbRestExecutorPool rest_executor_pool_; /*!< REST Executorプール */
    std::mutex session_token_mutex_;        /*!< セッショントークン更新用Mutex */

   protected:
    /**
     * コンストラクタ.
     */
    NbService(const std::string &endpoint_url, const std::string &tenant_id, const std::string &app_id,
              const std::string &app_key, const std::string &proxy);

    /**
     * REST Executor 取り出し.
     * @return  REST Executor
     * @retval  nullptr以外  取得成功
     * @retval  nullptr      同時接続数オーバー
     */
    virtual NbRestExecutor *PopRestExecutor();

    /**
     * REST Executor 返却.
     * @param[in]   executor    REST Executor
     */
    virtual void PushRestExecutor(NbRestExecutor *executor);

    /**
     * HTTPリクエストファクトリ取得.
     * @param[in]   executor    RESTタイムアウト(秒)
     * @return  リクエストファクトリ
     */
    NbHttpRequestFactory GetHttpRequestFactory();

    /**
     * REST実行(共通処理).
     * @param[in]   create_request         HTTPリクエスト作成関数ポインタ
     * @param[in]   executor_method        Executor関数ポインタ
     * @return      処理結果
     */
    NbResult<NbHttpResponse> ExecuteCommon(
        std::function<NbHttpRequest(NbHttpRequestFactory &)> create_request,
        std::function<NbResult<NbHttpResponse>(NbRestExecutor *, const NbHttpRequest &)> executor_method);
};
}  // namespace necbaas
#endif  // NECBAAS_NBSERVICE_H
