#ifndef NECBAAS_FTDATA_H
#define NECBAAS_FTDATA_H

#include <string>
#include <vector>
#include "necbaas/nb_json_object.h"

// テストデータ定義ヘッダ

namespace necbaas {

// エンドポイントURL
extern const std::string kEndPointUrl;

// デフォルトテナント（テナントA）
extern const std::string kTenantId;
extern const std::string kAppId;
extern const std::string kAppKey;
extern const std::string kMasterKey;
extern const std::string kProxy;

// デフォルトテナント(テナントB)
extern const std::string kTenantIdB;
extern const std::string kAppIdB;
extern const std::string kAppKeyB;
extern const std::string kMasterKeyB;
extern const std::string kProxyB;

// デフォルトユーザ
extern const std::string kUserName;
extern const std::string kEmail;
extern const std::string kPassword;
extern NbJsonObject kOptions;
extern const std::vector<std::string> kGroups;

// ユーザB
extern const std::string kUserNameB;
extern const std::string kEmailB;
extern const std::string kPasswordB;

// ファイルバケット
extern const std::string kFileBucketName;
extern const std::string kFileName;
extern const std::string kContentType;

// 試験用ファイルパス
// ビルド環境のfunctinal_test/filesをデフォルトとする
// kFileDir が設定されていれば kFileDir + "files/" を使用
extern const std::string kFileDir;
extern const std::string kUploadFile;
extern const std::string kUploadFile2;
extern const std::string kDownloadFile;

// API-GW
extern const std::string kApiName;
extern const std::string kSubpath;

// オブジェクトバケット
extern const std::string kObjectBucketName;

// 空文字
extern const std::string kEmpty;
} //namespace necbaas
#endif //NECBAAS_FTDATA_H
