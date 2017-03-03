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

#ifndef NECBAAS_NBCONSTANTS_H
#define NECBAAS_NBCONSTANTS_H

#include <string>

namespace necbaas {

//
// HTTP関連
//
extern const int kHttpConnectionMax;                /*!< HTTP同時接続数最大値 */
extern const int kRestTimeoutDefault;               /*!< RESTタイムアウトデフォルト(秒) */

//
// URI パス定義
//
extern const std::string kPathApiVersion;           /*!< URLパス: APIバージョン */
extern const std::string kLoginUrl;                 /*!< URLパス: Login */
extern const std::string kObjectBucketPath;         /*!< URLパス: Object Bucket */
extern const std::string kObjectsPath;              /*!< URLパス: Objects */
extern const std::string kFileBucketPath;           /*!< URLパス: File Bucket */
extern const std::string kFilesPath;                /*!< URLパス: Files */
extern const std::string kMetaPath;                 /*!< URLパス: meta data */
extern const std::string kApigwUrl;                 /*!< URLパス: API Gateway */

//
// HTTPヘッダ
//
extern const std::string kHeaderAppId;              /*!< HTTPヘッダ: アプリケーションID */
extern const std::string kHeaderAppKey;             /*!< HTTPヘッダ: アプリケーションKey */
extern const std::string kHeaderContentType;        /*!< HTTPヘッダ: コンテントタイプ */
extern const std::string kHeaderContentTypeJson;    /*!< HTTPヘッダ: コンテントタイプ値(Json) */
extern const std::string kHeaderSessionToken;       /*!< HTTPヘッダ: セッショントークン */
extern const std::string kHeaderUserAgent;          /*!< HTTPヘッダ: User Agent */
extern const std::string kHeaderUserAgentDefault;   /*!< HTTPヘッダ: User Agent値 */
extern const std::string kHeaderContentLength;      /*!< HTTPヘッダ: Content-Length */
extern const std::string kHeaderXContentLength;     /*!< HTTPヘッダ: X-Content-Length */
extern const std::string kHeaderXAcl;               /*!< HTTPヘッダ: X-ACL */

//
// Key
//
// common
extern const std::string kKeyId;                    /*!< Key: Id */
extern const std::string kKeyCreatedAt;             /*!< Key: CreatedAt */
extern const std::string kKeyUpdatedAt;             /*!< Key: UpdatedAt */
extern const std::string kKeyAcl;                   /*!< Key: Acl */
extern const std::string kKeyContentAcl;            /*!< Key: ContentAcl */
extern const std::string kKeyETag;                  /*!< Key: ETag */
extern const std::string kKeyResults;               /*!< Key: Results */
extern const std::string kKeyReasonCode;            /*!< Key: ReasonCode */
extern const std::string kKeyDetail;                /*!< Key: Detail */

// user / group
extern const std::string kKeyUsername;              /*!< Key: ユーザ名 */
extern const std::string kKeyPassword;              /*!< Key: パスワード */
extern const std::string kKeyEmail;                 /*!< Key: E-mail */
extern const std::string kKeySessionToken;          /*!< Key: セッショントークン */
extern const std::string kKeyExpire;                /*!< Key: 有効期限 */
extern const std::string kKeyGroups;                /*!< Key: グループ一覧 */
extern const std::string kKeyUsers;                 /*!< Key: ユーザ一覧 */
extern const std::string kKeyOptions;               /*!< Key: オプション */

// bucket
extern const std::string kKeyName;                  /*!< Key: バケット名 */
extern const std::string kKeyDescription;           /*!< Key: 説明文 */

// file
extern const std::string kKeyFilename;              /*!< Key: ファイル名 */
extern const std::string kKeyContentType;           /*!< Key: Content-Type */
extern const std::string kKeyLength;                /*!< Key: ファイルサイズ */
extern const std::string kKeyPublished;             /*!< Key: 公開済フラグ */
extern const std::string kKeyDeleted;               /*!< Key: 削除済フラグ */
extern const std::string kKeyMetaETag;              /*!< Key: ETag値 */
extern const std::string kKeyFileETag;              /*!< Key: ETag値 */
extern const std::string kKeyCacheDisabled;         /*!< Key: キャッシュ禁止フラグ */
extern const std::string kKeyDeleteMark;            /*!< Key: 削除マーク */

// object
extern const std::string kKeyWhere;                 /*!< Key: 検索条件 */
extern const std::string kKeyOrder;                 /*!< Key: ソート順序 */
extern const std::string kKeySkip;                  /*!< Key: スキップカウント*/
extern const std::string kKeyLimit;                 /*!< Key: 検索数上限 */
extern const std::string kKeyCount;                 /*!< Key: 検索条件に合致する全件数取得 */
extern const std::string kKeyProjection;            /*!< Key: プロジェクション */
extern const std::string kKeyReadPreference;        /*!< Key: 参照するDBの指定 */
extern const std::string kKeyTimeout;               /*!< Key: クエリタイムアウト（ミリ秒） */

// ACL
extern const std::string kKeyOwner;                 /*!< Key: owner */
extern const std::string kKeyRead;                  /*!< Key: read */
extern const std::string kKeyWrite;                 /*!< Key: write */
extern const std::string kKeyCreate;                /*!< Key: create */
extern const std::string kKeyUpdate;                /*!< Key: update */
extern const std::string kKeyDelete;                /*!< Key: delete */
extern const std::string kKeyAdmin;                 /*!< Key: admin */
extern const std::string kGroupAnonymous;           /*!< Group名: anonymous */
extern const std::string kGroupAuthenticated;       /*!< Group名: authenticated */
} //namespace necbaas
#endif //NECBAAS_NBCONSTANTS_H
