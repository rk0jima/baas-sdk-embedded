/*
 * Copyright (C) 2017-2019 NEC Corporation
 */

#ifndef NECBAAS_NBHTTPREQUEST_H
#define NECBAAS_NBHTTPREQUEST_H

#include <string>
#include <list>
#include "necbaas/nb_http_request_method.h"
#include "necbaas/nb_result_code.h"
#include "necbaas/nb_http_options.h"

namespace necbaas {

/**
 * @class NbHttpRequest nb_http_request.h "necbaas/internal/nb_http_request.h"
 * HTTPリクエスト.
 * NbHttpRequestFactoryクラスのBuilder機能を使用してインスタンスを生成すること。
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbHttpRequest {
  public:
    /**
     * コンストラクタ.
     * @param[in]   url        リクエストURL
     * @param[in]   method     HTTPメソッド
     * @param[in]   headers    HTTPヘッダリスト
     * @param[in]   body       HTTPボディ
     */
    NbHttpRequest(const std::string &url, const NbHttpRequestMethod &method,
                  const std::list<std::string> &headers, const std::string &body,
                  const std::string &proxy, const NbHttpOptions &http_options);

    /**
     * デストラクタ.
     */
    ~NbHttpRequest();

    /**
     * リクエストURL取得.
     * @return      リクエストURL
     */
    const std::string &GetUrl() const;

    /**
     * HTTPメソッド取得.
     * @return      HTTPメソッド
     */
    const NbHttpRequestMethod &GetMethod() const;

    /**
     * HTTPヘッダリスト取得.
     * @return      HTTPヘッダリスト
     */
    const std::list<std::string> &GetHeaders() const;

    /**
     * HTTPボディ取得.
     * @return      HTTPボディ
     */
    const std::string &GetBody() const;

    /**
     * Proxy取得.
     * @return      Proxy URL
     */
    const std::string &GetProxy() const;

    /**
     * CURLオプション取得.
     * @return      Curlオプション
     */
    std::list<std::shared_ptr<curlpp::OptionBase>> GetHttpOptions() const;

    /**
     * ダンプ.
     * ログにHTTPリクエスト情報を出力する
     */
    void Dump() const;
  private:
    const std::string url_{};                   /*!< リクエストURL    */
    const NbHttpRequestMethod method_{};        /*!< HTTPメソッド     */
    const std::list<std::string> headers_{};    /*!< HTTPヘッダリスト */
    const std::string body_{};                  /*!< HTTPボディ       */
    const std::string proxy_{};                 /*!< Proxy URL        */
    const NbHttpOptions http_options_{};        /*!< HTTPオプション   */
};
} //namespace necbaas

#endif //NECBAAS_NBHTTPREQUEST_H
