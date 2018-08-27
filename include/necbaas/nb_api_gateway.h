/*
 * Copyright (C) 2017 NEC Corporation
 */

#ifndef NECBAAS_NBAPIGATEWAY_H
#define NECBAAS_NBAPIGATEWAY_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "necbaas/nb_service.h"

namespace necbaas {

/**
 * @class NbApiGateway nb_api_gateway.h "necbaas/nb_api_gateway.h"
 * API Gatewayクラス.
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbApiGateway {
   public:
    /**
     * コンストラクタ.
     * @param[in]   service        サービスインスタンス
     * @param[in]   api_name       api-name
     * @param[in]   http_method    HTTPメソッド
     * @param[in]   subpath        subpath(空文字：設定しない)
     */
    NbApiGateway(const std::shared_ptr<NbService> &service, const std::string &api_name,
                 NbHttpRequestMethod http_method, const std::string &subpath = "");

    /**
     * デストラクタ.
     */
    ~NbApiGateway();

    /**
     * カスタムAPI実行.
     * 文字列Body付きのカスタムAPIを実行する。<br>
     * 設定されたapi-nameが空文字の場合は、API名エラーを返す。<br>
     * 空文字が設定された場合はBodyなしとして実行する。<br>
     * GET,DELETEメソッドの場合、Bodyは無視される。
     * @param[in]   body        Bodyデータ(文字列)
     * @return      処理結果
     */
    NbResult<NbHttpResponse> ExecuteCustomApi(const std::string &body = "");

    /**
     * カスタムAPI実行.
     * バイナリBody付きのカスタムAPIを実行する。<br>
     * 設定されたapi-nameが空文字の場合は、API名エラーを返す。<br>
     * Bodyが空の場合は、Bodyなしとして実行する。<br>
     * GET,DELETEメソッドの場合、Bodyは無視される。
     * @param[in]   body        Bodyデータ(バイナリ)
     * @return      処理結果
     */
    NbResult<NbHttpResponse> ExecuteCustomApi(const std::vector<char> &body);

    /**
     * RESTタイムアウト取得.
     * @return      タイムアウト(秒)
     */
    int GetTimeout() const;

    /**
     * RESTタイムアウト設定.
     * RESTのタイムアウト値(秒)を設定する。<br>
     * 0以下の値が設定された場合は、デフォルト値(60秒)が設定される。
     * @param[in]   timeout        タイムアウト(秒)
     */
    void SetTimeout(int timeout);

    /**
     * HTTPヘッダリスト取得.
     * @return      HTTPヘッダリスト
     */
    const std::multimap<std::string, std::string> &GetHeaders() const;

    /**
     * HTTPヘッダ追加.
     * HTTPヘッダ情報を追加する。<br>
     * Content-Typeは、SetContentType()を使用すること。<br>
     * 以下のヘッダ名は、本メソッドで設定しても無視される。
     * - Content-Type
     * - Content-Length
     * - X-Application-Id
     * - X-Application-Key
     * - X-Session-Token
     * - Host
     *
     * User-Agentは、1つのみ設定可能。2つ目以降は上書きされる。<br>
     * ヘッダ名、ヘッダ値のいずれかが空文字の場合は追加しない。
     * @param[in]   name        ヘッダ名
     * @param[in]   value       ヘッダ値
     */
    void AddHeader(const std::string &name, const std::string &value);

    /**
     * HTTPヘッダ削除.
     * HTTPヘッダ情報を削除する。<br>
     * ヘッダ名が空文字の場合は処理しない。
     * @param[in]   name        ヘッダ名
     * @return      削除された数
     */
    int RemoveHeader(const std::string &name);

    /**
     * HTTPヘッダクリア.
     * HTTPヘッダ情報を全削除する。
     */
    void ClearHeaders();

    /**
     * リクエストパラメータリスト取得.
     * @return      リクエストパラメータリスト
     */
    const std::multimap<std::string, std::string> &GetParameters() const;

    /**
     * リクエストパラメータ追加.
     * リクエストパラメータ情報を追加する。<br>
     * パラメータ名、パラメータ値のいずれかが空文字の場合は追加しない。<br>
     * POST, PUTメソッドの場合は、無視される。
     * @param[in]   name        パラメータ名
     * @param[in]   value       パラメータ値
     */
    void AddParameter(const std::string &name, const std::string &value);

    /**
     * リクエストパラメータ削除.
     * リクエストパラメータ情報を削除する。<br>
     * リクエストパラメータ名が空文字の場合は処理しない。
     * @param[in]   name        パラメータ名
     * @return      削除された数
     */
    int RemoveParameter(const std::string &name);

    /**
     * リクエストパラメータクリア.
     * リクエストパラメータ情報を全削除する。
     */
    void ClearParameters();

    /**
     * Content-Type取得.
     * @return      Content-Type
     */
    const std::string &GetContentType() const;

    /**
     * Content-Type設定.
     * Bodyの設定がない場合は、無視される。
     * @param[in]   content_type      Content-Type
     */
    void SetContentType(const std::string &content_type);

   private:
    std::shared_ptr<NbService> service_;                 /*!< サービスインスタンス   */
    int timeout_{kRestTimeoutDefault};                   /*!< RESTタイムアウト(秒)   */
    std::string api_name_;                               /*!< api-name               */
    NbHttpRequestMethod http_method_;                    /*!< HTTPメソッド           */
    std::string subpath_;                                /*!< subpath                */
    std::multimap<std::string, std::string> parameters_; /*!< リクエストパラメータ   */
    std::multimap<std::string, std::string> headers_;    /*!< HTTPヘッダ             */
    std::string content_type_;                           /*!< Content-Type           */

    /**
     * Content-Typeヘッダ判定.
     * PSOT/PUTメソッドでボディが設定されている場合、Content-Typeが設定されているか確認する。
     * @param[in]       body                ボディ
     * @return  判定結果
     * @retval  true    OK
     * @retval  false   NG
     */
    bool CheckContentType(const std::string &body);

    /**
     * Content-Typeヘッダ追加.
     * HTTPリクエストファクトリにContent-Typeを設定する。<br>
     * ボディが空の場合は何もしない。<br>
     * @param[in]       body                ボディ
     * @param[in,out]   request_factory     HTTPリクエストファクトリ
     */
    void AppendContentType(const std::string &body, NbHttpRequestFactory *request_factory);

    /**
     * 予約ヘッダ名確認.
     * ヘッダ名が予約ヘッダ名であるかどうか確認する。
     * @param[in]       name        ヘッダ名
     * @return  確認結果
     * @retval  true    予約ヘッダ名である
     * @retval  false   予約ヘッダ名でない
     */
    bool IsReservedHeaderName(const std::string &name) const;
};
}  // namespace necbaas
#endif  // NECBAAS_NBAPIGATEWAY_H
