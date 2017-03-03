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
    void SetSessionToken(NbSessionToken token);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>セッショントークン削除.</p>
     */
    void ClearSessionToken();

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>REST Executor 取り出し.</p>
     * @return  REST Executor
     * @retval  nullptr以外  取得成功
     * @retval  nullptr      同時接続数オーバー
     */
    NbRestExecutor *PopRestExecutor();

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>REST Executor 返却.</p>
     * @param[in]   executor    REST Executor
     */
    void PushRestExecutor(NbRestExecutor *executor);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>HTTPリクエストファクトリ取得.</p>
     * @param[in]   executor    RESTタイムアウト(秒)
     * @return  リクエストファクトリ
     */
    NbHttpRequestFactory GetHttpRequestFactory();

   private:
    std::string app_id_;                    /*!< アプリケーションID */
    std::string app_key_;                   /*!< アプリケーションキー */
    std::string endpoint_url_;              /*!< Endpoint URI */
    std::string tenant_id_;                 /*!< テナントID */
    std::string proxy_;                     /*!< Proxy URL */
    NbSessionToken session_token_;          /*!< セッショントークン */
    NbRestExecutorPool rest_executor_pool_; /*!< REST Executorプール */
    std::mutex session_token_mutex_;        /*!< セッショントークン更新用Mutex */

    /**
     * コンストラクタ.
     */
    NbService(const std::string &endpoint_url, const std::string &tenant_id, const std::string &app_id,
              const std::string &app_key, const std::string &proxy);
};
}  // namespace necbaas
#endif  // NECBAAS_NBSERVICE_H
