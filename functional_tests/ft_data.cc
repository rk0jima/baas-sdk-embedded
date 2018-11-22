#include "ft_data.h"
#include "necbaas/nb_json_object.h"

namespace necbaas {

// エンドポイントURL
const std::string kEndPointUrl = "https://xx.xx.xx.xx/api";

// デフォルトテナント（テナントA）
const std::string kTenantId = "";
const std::string kAppId = "";
const std::string kAppKey = "";
const std::string kMasterKey = "";
const std::string kProxy = "";

// デフォルトテナント(テナントB)
const std::string kTenantIdB = "";
const std::string kAppIdB = "";
const std::string kAppKeyB = "";
const std::string kMasterKeyB = "";
const std::string kProxyB = "";

// デフォルトユーザ
const std::string kUserName = "userTest";
const std::string kEmail = "userTest@example.com";
const std::string kPassword = "Passw0rD";
NbJsonObject kOptions(R"({"option1":"testOption1","option2":"testOption2"})");
const std::vector<std::string> kGroups = {"group1", "group2"};

// ユーザB
const std::string kUserNameB = "userB";
const std::string kEmailB = "userB@example.com";
const std::string kPasswordB = "Passw0rDB";

// ファイルバケット
const std::string kFileBucketName = "fileBucket";
const std::string kFileName = "file_name.txt";
const std::string kContentType = "text/plain";

// 試験用ファイルパス
// ビルド環境のfunctinal_test/filesをデフォルトとする
// kFileDir が設定されていれば kFileDir + "files/" を使用
const std::string kFileDir = "";
const std::string kUploadFile = "upload.txt";
const std::string kUploadFile2 = "upload2.txt";
const std::string kDownloadFile = "download.txt";

// API-GW
const std::string kApiName = "embedded";
const std::string kSubpath = "/echoBack";

// オブジェクトバケット
const std::string kObjectBucketName = "objectBucket";
const std::string kNoAclObjectBucketName = "noAclObjectBucket";

// 空文字
const std::string kEmpty = "";
} //namespace necbaas
