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

#ifndef NECBAAS_NBHTTPREQUESTFACTORY_H
#define NECBAAS_NBHTTPREQUESTFACTORY_H

#include <string>
#include <map>
#include "necbaas/nb_http_request_method.h"
#include "necbaas/internal/nb_http_request.h"

namespace necbaas {

class TestUtil;

/**
 * @class NbHttpRequestFactory nb_http_request_factory.h "necbaas/internal/nb_http_request_factory.h"
 * HTTPリクエストファクトリ.
 * NbHttpRequestインスタンスを生成するBuilder機能を有する。
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */

class NbHttpRequestFactory {
    friend class TestUtil; //UT用

  public:
    /**
     * コンストラクタ.
     * @param[in]   end_point_url       End Point URL
     * @param[in]   tenant_id           テナントID
     * @param[in]   app_id              アプリケーションID
     * @param[in]   app_key             アプリケーションキー
     * @param[in]   sessnon_token       セッショントークン
     */
    NbHttpRequestFactory(const std::string &end_point_url, const std::string &tenant_id, const std::string &app_id,
                         const std::string &app_key, const std::string &sessnon_token, const std::string &proxy);

    /**
     * デストラクタ.
     */
    ~NbHttpRequestFactory();

    /**
     * GETリクエスト生成.
     * @param[in]   api_uri    APIパス
     * @return      this
     */
    NbHttpRequestFactory &Get(const std::string &api_uri);

    /**
     * POSTリクエスト生成.
     * @param[in]   api_uri    APIパス
     * @return      this
     */
    NbHttpRequestFactory &Post(const std::string &api_uri);

    /**
     * PUTリクエスト生成.
     * @param[in]   api_uri    APIパス
     * @return      this
     */
    NbHttpRequestFactory &Put(const std::string &api_uri);

    /**
     * DELETEリクエスト生成.
     * @param[in]   api_uri    APIパス
     * @return      this
     */
    NbHttpRequestFactory &Delete(const std::string &api_uri);

    /**
     * パス追加設定.
     * URL パス要素を末尾に連結する。パス区切り(/)は必要に応じて自動挿入される。
     * @param[in]   path    パス
     * @return this
     */
    NbHttpRequestFactory &AppendPath(const std::string &path);

    /**
     * クエリパラメータ設定(上書き).
     * @param[in]   params  クエリパラメータ 
     * @return this
     */
    NbHttpRequestFactory &Params(const std::multimap<std::string, std::string> &params);

    /**
     * クエリパラメータ設定(末尾に追加).
     * @param[in]   params  クエリパラメータ
     * @return this
     */
    NbHttpRequestFactory &AppendParam(const std::string &key, const std::string &value);

    /**
     * HTTPヘッダ設定(上書き).
     * @param[in]   headers  ヘッダリスト
     * @return this
     */
    NbHttpRequestFactory &Headers(const std::multimap<std::string, std::string> &headers);

    /**
     * HTTPヘッダ設定(末尾に追加).
     * @param[in]   headers  ヘッダリスト
     * @return this
     */
    NbHttpRequestFactory &AppendHeader(const std::string &key, const std::string &value);

    /**
     * HTTPボディ設定(上書き).
     * @param[in]   body  ボディ
     * @return this
     */
    NbHttpRequestFactory &Body(const std::string &body);

    /**
     * セッショントークン未使用設定.
     * HTTPリクエストにセッショントークンを付与しない。
     * デフォルトは、セッショントークンが存在すればセッショントークンを付与する。
     * @return this
     */
    NbHttpRequestFactory &SessionNone();

    /**
     * Builder実行.
     * @return  NbHttpRequestインスタンス
     */
    NbHttpRequest Build();

    /**
     * エラーコード取得.
     * インスタンス構築時のエラーコードを取得する。
     * @return      エラーコード
     */
    NbResultCode GetError() const;

    /**
     * エラー発生確認.
     * インスタンス構築時にエラーを検出している場合にtrueが返る。
     * @return      Proxy URL
     */
    bool IsError() const;
  private:
    std::string end_point_url_;                                 /*!< End Point URL */
    const std::string tenant_id_;                               /*!< テナントID */
    const std::string app_id_;                                  /*!< アプリケーションID */
    const std::string app_key_;                                 /*!< アプリケーションキー */
    const std::string session_token_;                           /*!< セッショントークン */
    const std::string proxy_;                                   /*!< Proxy */

    NbHttpRequestMethod request_method_{NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET};
                                                                /*!< HTTPメソッド */
    std::string path_{};                                        /*!< URLのサービス種別 */
    std::multimap<std::string, std::string> request_params_{};  /*!< リクエストパラメータ */
    std::multimap<std::string, std::string> headers_{};         /*!< HTTPヘッダリスト */
    std::string body_{};                                        /*!< HTTPボディ */
    bool session_none_{false};                                  /*!< セッショントークンを付与しない */

    NbResultCode error_{NbResultCode::NB_OK};                   /*!< error発生フラグ  */

    /**
     * リクエストパラメータ生成.
     * メンバ変数に設定されたリストからリクエストパラメータ文字列を生成する
     * @return  リクエストパラメータ
     */
    std::string CreateRequestParams();

    /**
     * パラメータチェック.
     * コンストラクタで設定されたパラメータをチェックする。<br>
     * チェック結果は、error_に設定される。
     */
    void CheckParameter();
};
} //namespace necbaas
#endif //NECBAAS_NBHTTPREQUESTFACTORY_H
