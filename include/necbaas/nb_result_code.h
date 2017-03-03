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

#ifndef NECBAAS_NBRESULTCODE_H
#define NECBAAS_NBRESULTCODE_H

namespace necbaas {

/**
 * 処理結果コード.
 */
enum class NbResultCode {
    NB_OK = 0,                   /*!< 成功                           */
    NB_ERROR_RESPONSE,           /*!< HTTPレスポンスエラー           */
    NB_ERROR_INVALID_ARGUMENT,   /*!< 引数不正                       */
    NB_ERROR_ENDPOINT_URL,       /*!< Endpoint URIエラー             */
    NB_ERROR_TENANT_ID,          /*!< テナントIDエラー               */
    NB_ERROR_APP_ID,             /*!< アプリケーションIDエラー       */
    NB_ERROR_APP_KEY,            /*!< アプリケーションキーエラー     */
    NB_ERROR_OPEN_FILE,          /*!< ファイルオープンエラー         */
    NB_ERROR_FILE_DOWNLOAD,      /*!< ファイルダウンロードエラー     */
    NB_ERROR_INCORRECT_RESPONSE, /*!< 不正なHTTPレスポンス           */
    NB_ERROR_CONNECTION_OVER,    /*!< 同時接続数オーバー             */
    NB_ERROR_NOT_LOGGED_IN,      /*!< 未ログイン                     */
    NB_ERROR_SESSION_EXPIRED,    /*!< セッショントークン有効期限切れ */
    NB_ERROR_BUCKET_NAME,        /*!< バケット名エラー               */
    NB_ERROR_OBJECT_ID,          /*!< オブジェクトIDエラー           */
    NB_ERROR_API_NAME,           /*!< API Nameエラー                 */
    NB_ERROR_CONTENT_TYPE,       /*!< Content-Typeエラー             */
    NB_ERROR_CURL_RUNTIME,       /*!< CURL Runtimeエラー             */
    NB_ERROR_CURL_LOGIC,         /*!< CURL Loginエラー               */
    NB_ERROR_CURL_FATAL,         /*!< CURL その他エラー              */
    NB_FATAL,                    /*!< 処理異常検出                   */
};
}  // namespace necbaas
#endif  // NECBAAS_NBRESULTCODE_H
