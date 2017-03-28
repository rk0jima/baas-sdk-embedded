#ifndef NECBAAS_FTDATA_H
#define NECBAAS_FTDATA_H

#include <string>
#include <vector>
#include "necbaas/nb_json_object.h"

// テストデータ定義ヘッダ

namespace necbaas {
// エンドポイントURL
static const std::string kEndPointUrl{"http://10.27.163.25:8080/api"};

// デフォルトテナント
static const std::string kTenantId{"58980feb2909b20f69a18463"};
static const std::string kAppId{"589810262909b20f69a18467"};
static const std::string kAppKey{"IrqJVO5bzj0Mcbx6g415MI3F5idp2CkcNqLAGOwy"};
static const std::string kMasterKey{"mfUeyIvWYRFm4DXyNwpRLd31WUnwtVOtWDc9XXvu"};
static const std::string kProxy{};

// デフォルトユーザ
static const std::string kUserName{"userTest"};
static const std::string kEmail{"userTest@example.com"};
static const std::string kPassword{"Passw0rD"};
static NbJsonObject kOptions(R"({"option1":"testOption1","option2":"testOption2"})");
static const std::vector<std::string> kGroups{"group1", "group2"};

// ファイルバケット
static const std::string kFileBucketName{"fileBucket"};
static const std::string kFileName{"file_name.txt"};
static const std::string kContentType{"text/plain"};

// 試験用ファイルパス
// ビルド環境のfunctinal_test/filesをデフォルトとする
// kFileDir が設定されていれば kFileDir + "files/" を使用
static const std::string kFileDir{};
static const std::string kUploadFile{"upload.txt"};
static const std::string kUploadFile2{"upload2.txt"};
static const std::string kDownloadFile{"download.txt"};

// API-GW
static const std::string kApiName{"embedded"};
static const std::string kSubpath{"/echoBack"};

// オブジェクトバケット
static const std::string kObjectBucketName{"objectBucket"};

// 空文字
static const std::string kEmpty{};
} //namespace necbaas
#endif //NECBAAS_FTDATA_H
