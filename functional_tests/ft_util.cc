#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_service.h"
#include "necbaas/nb_file_bucket.h"
#include "necbaas/internal/nb_utility.h"

// テストユーティリティ&メイン関数

namespace necbaas {
namespace FTUtil {

using std::string;
using std::vector;
using std::list;
using std::shared_ptr;

static shared_ptr<NbService> master_service;
static shared_ptr<NbService> master_service_b;

// バケット初期化
void InitDefaultBucket() {
    CreateBucket("file", "CUSTOM_CODE", NbAcl().ToJsonString(), NbAclBase().ToJsonString());
    CreateBucket(master_service_b, "file", "CUSTOM_CODE", NbAcl().ToJsonString(), NbAclBase().ToJsonString());
    CreateBucket("file", "fileBucket");
    CreateBucket(master_service_b, "file", "fileBucket");
    DeleteAllFile();
    DeleteAllFile(master_service_b);
    CreateBucket("object", "objectBucket");
    CreateBucket(master_service_b, "object", "objectBucket");
    DeleteAllObject();
    DeleteAllObject(master_service_b);
}

void CreateGroup(shared_ptr<NbService> service, const string &group) {
    NbResult<NbHttpResponse> rest_result = service->ExecuteRequest(
        [&](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Put("/groups")
                          .AppendPath("/" + group)
                          .AppendHeader(string("Content-Type"), string("application/json"))
                          .Body(string(R"({"users":[]})"))
                          .Build();
        }, 60);
}

string CreateUser(shared_ptr<NbService> service, const string &user_name, const string &email,
                  const string &password, const NbJsonObject &options) {
    // ユーザ生成
    string user_id;
    NbResult<NbHttpResponse> rest_result = service->ExecuteRequest(
        [&](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Get("/users")
                          .AppendParam(string("username"), user_name)
                          .Build();
        }, 60);
    if (rest_result.IsSuccess()) {
        const NbHttpResponse &http_response = rest_result.GetSuccessData();
        NbJsonObject json(http_response.GetBody());
        
        user_id = json.GetJsonArray("results").GetJsonObject(0).GetString("_id");
    } else {
        rest_result = service->ExecuteRequest(
            [&](NbHttpRequestFactory &factory) -> NbHttpRequest {
                NbJsonObject json;
                json["username"] = user_name;
                json["email"] = email;
                json["password"] = password;
                json.PutJsonObject("options", options);
                return factory.Post("/users")
                              .AppendHeader(string("Content-Type"), string("application/json"))
                              .Body(json.ToJsonString())
                              .Build();
            }, 60);
        EXPECT_TRUE(rest_result.IsSuccess());
        const NbHttpResponse &http_response = rest_result.GetSuccessData();
        NbJsonObject json(http_response.GetBody());
        user_id = json.GetString("_id");
    }

    return user_id;
}

void UpdateGroup(shared_ptr<NbService> service, const string &group, const NbJsonObject &user_list) {
    NbResult<NbHttpResponse> rest_result = service->ExecuteRequest(
        [&](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Put("/groups/" + group + "/addMembers")
                          .AppendHeader(string("Content-Type"), string("application/json"))
                          .Body(user_list.ToJsonString())
                          .Build();
        }, 60);
    ASSERT_TRUE(rest_result.IsSuccess());
}

// デフォルトユーザの作成
void CreateDefaultUser() {
    NbJsonObject users;
    NbJsonArray user_list_a;
    NbJsonArray user_list_b;

    CreateGroup(master_service, "group1");
    CreateGroup(master_service, "group2");
    CreateGroup(master_service_b, "group1");
    CreateGroup(master_service_b, "group2");
    user_list_a.Append(CreateUser(master_service, kUserName, kEmail, kPassword, kOptions));
    user_list_a.Append(CreateUser(master_service, kUserNameB, kEmailB, kPasswordB, kOptions));
    user_list_a.Append(CreateUser(master_service, "userTest@", "userTest2@example.com", kPassword, kOptions));
    user_list_b.Append(CreateUser(master_service_b, kUserName, kEmail, kPassword, kOptions));
    users.PutJsonArray("users", user_list_a);
    UpdateGroup(master_service, "group1", users);
    UpdateGroup(master_service, "group2", users);
    users.PutJsonArray("users", user_list_b);
    UpdateGroup(master_service_b, "group1", users);
    UpdateGroup(master_service_b, "group2", users);
}

// バケットを作成・更新する
void CreateBucket(const string &bucket_type, const string &bucket_name, string acl, string content_acl) {
    CreateBucket(master_service, bucket_type, bucket_name, acl, content_acl);
}

void CreateBucket(shared_ptr<NbService> service, const string &bucket_type, const string &bucket_name, string acl, string content_acl) {
    NbJsonObject body;
    body["description"] = "description";
    if (acl.empty()) {
        acl = R"({"r":["g:authenticated"],"w":[],"c":[],"u":[],"d":[],"admin":[]})";
    }
    body.PutJsonObject("ACL", NbJsonObject(acl));
    if (content_acl.empty()) {
        content_acl = R"({"r":["g:anonymous"],"w":["g:anonymous"],"c":[],"u":[],"d":[]})";
    }
    body.PutJsonObject("contentACL", NbJsonObject(content_acl));

    NbResult<NbHttpResponse> rest_result = service->ExecuteRequest(
        [&](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Put("/buckets")
                          .AppendPath("/" + bucket_type + "/" + bucket_name)
                          .AppendHeader(string("Content-Type"), string("application/json"))
                          .Body(body.ToJsonString())
                          .Build();
        }, 60);
    EXPECT_TRUE(rest_result.IsSuccess());
}

// テストバケットの全オブジェクトを削除する
void DeleteAllObject() {
    DeleteAllObject(master_service);
}

void DeleteAllObject(shared_ptr<NbService> service) {
    NbResult<NbHttpResponse> rest_result = service->ExecuteRequest(
        [](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Delete("/objects")
                          .AppendPath("/" + kObjectBucketName)
                          .Build();
        }, 60);
    EXPECT_TRUE(rest_result.IsSuccess());
}

// テストバケットの全ファイルを削除する
void DeleteAllFile() {
    DeleteAllFile(master_service);
}

void DeleteAllFile(shared_ptr<NbService> service) {
    NbFileBucket file_bucket(service, kFileBucketName);
    NbResult<vector<NbFileMetadata>> file_lists = file_bucket.GetFiles(false, true);
    ASSERT_TRUE(file_lists.IsSuccess());
    for (auto meta : file_lists.GetSuccessData()) {
        file_bucket.DeleteFile(meta.GetFileName(), kEmpty, kEmpty);
    }
}

// ファイル公開
void PublishFile(const string &bucket_name, const string &file_name) {
    NbResult<NbHttpResponse> rest_result = master_service->ExecuteRequest(
        [&](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Put("/files")
                          .AppendPath("/" + bucket_name + "/" + file_name + "/publish")
                          .Build();
        }, 60);
    EXPECT_TRUE(rest_result.IsSuccess());
}

// ファイルパスの作成
string MakeFilePath(const string &file_name) {
    // ビルド環境のfunctinal_test/filesをデフォルトとする
    // kFileDir が設定されていれば kFileDir + "files/" を使用
    string file_path;

    if (kFileDir.empty()) {
        file_path = __FILE__;
        file_path.erase(file_path.rfind("/") + 1);
        file_path += "files/" + file_name;
    } else {
        file_path = kFileDir + "files/" + file_name;
    }

    return file_path;
}

// テスト用ファイルの作成
void CreateTestFile(const string &file_path, int file_size) {
    std::ofstream of_stream(file_path, std::ios::out | std::ios::binary | std::ios::trunc);
    string data{"0123456789"};
    int data_size = data.size();
    for (int i = 0; i < file_size / data_size; ++i) {
        of_stream << data;
    }
    for (int i = 0; i < file_size % data_size; ++i) {
        of_stream << std::to_string(i);
    }
    of_stream.flush();
}

// key,valに一致するヘッダを検索する（keyの大文字小文字は問わない）
bool SearchHeader(const std::multimap<std::string, std::string> &map,
                         const string &key, const string &value) {
    for (auto search : map) {
        if (NbUtility::CompareCaseInsensitiveString(search.first, key) &&
           (value == search.second)) {
            return true;
        }
    }
    return false;
}

// ACLの比較
void CompareAcl(const NbAcl &acl1, const NbAcl &acl2) {
    EXPECT_EQ(acl1.GetCreate(), acl2.GetCreate());
    EXPECT_EQ(acl1.GetDelete(), acl2.GetDelete());
    EXPECT_EQ(acl1.GetRead(), acl2.GetRead());
    EXPECT_EQ(acl1.GetUpdate(), acl2.GetUpdate());
    EXPECT_EQ(acl1.GetWrite(), acl2.GetWrite());
    EXPECT_EQ(acl1.GetAdmin(), acl2.GetAdmin());
    EXPECT_EQ(acl1.GetOwner(), acl2.GetOwner());
}

// NbObjectの比較
void CompareObject(const NbObject &obj1, const NbObject &obj2) {
    EXPECT_EQ(obj1.GetBucketName(), obj2.GetBucketName());
    EXPECT_EQ(obj1.GetObjectId(), obj2.GetObjectId());
    CompareTime(obj1.GetCreatedTime(), obj2.GetCreatedTime());
    CompareTime(obj1.GetUpdatedTime(), obj2.GetUpdatedTime());
    CompareAcl(obj1.GetAcl(), obj2.GetAcl());
    EXPECT_EQ(obj1.IsDeleteMark(), obj2.IsDeleteMark());
    EXPECT_EQ(obj1, obj2);
}

// ファイルの中身の比較
void CompareFiledata(const string &file1, const string &file2, int size) {
    char *buffer = new char[size];
    std::ifstream file_stream;
    file_stream.open(file1, std::ios::in | std::ios::binary);
    file_stream.read(buffer, size);
    string file1_data(buffer, size);

    file_stream.open(file2, std::ios::in | std::ios::binary);
    file_stream.read(buffer, size);
    string file2_data(buffer, size);

    EXPECT_EQ(file1_data, file2_data);
    delete[] buffer;
}

// std::tmの比較
void CompareTime(const std::tm &tm1, const std::tm &tm2, bool equal) {
    if (equal) {
        EXPECT_EQ(tm1.tm_sec, tm2.tm_sec);
        EXPECT_EQ(tm1.tm_min, tm2.tm_min);
        EXPECT_EQ(tm1.tm_hour, tm2.tm_hour);
        EXPECT_EQ(tm1.tm_mday, tm2.tm_mday);
        EXPECT_EQ(tm1.tm_mon, tm2.tm_mon);
        EXPECT_EQ(tm1.tm_year, tm2.tm_year);
        EXPECT_EQ(tm1.tm_wday, tm2.tm_wday);
        EXPECT_EQ(tm1.tm_yday, tm2.tm_yday);
    } else {
        EXPECT_FALSE(tm1.tm_sec == tm2.tm_sec &&
                     tm1.tm_min == tm2.tm_min &&
                     tm1.tm_hour == tm2.tm_hour &&
                     tm1.tm_mday == tm2.tm_mday &&
                     tm1.tm_mon == tm2.tm_mon &&
                     tm1.tm_year == tm2.tm_year &&
                     tm1.tm_wday == tm2.tm_wday &&
                     tm1.tm_yday == tm2.tm_yday);
    }
}

// std::tm型データのインクリメント
std::tm IncrementTime(std::tm tm) {
    std::time_t time = std::mktime(&tm);
    ++time;
    return *std::localtime(&time);
}

// NbUserの比較
void CompareUser(const NbUser &user1, const NbUser &user2) {
    EXPECT_EQ(user1.GetUserName(), user2.GetUserName());
    EXPECT_EQ(user1.GetUserId(), user2.GetUserId());
    EXPECT_EQ(user1.GetEmail(), user2.GetEmail());
    EXPECT_EQ(user1.GetOptions(), user2.GetOptions());
    CompareList(user1.GetGroups(), user2.GetGroups());
    CompareTime(user1.GetCreatedTime(), user2.GetCreatedTime());
    CompareTime(user1.GetUpdatedTime(), user2.GetUpdatedTime());
}
} //namespace FTUtil
} //namespace necbaas

// メイン
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    using namespace necbaas;
    FTUtil::master_service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
    FTUtil::master_service_b = NbService::CreateService(kEndPointUrl, kTenantIdB, kAppIdB, kMasterKeyB, kProxyB);

    // 初期設定
    FTUtil::InitDefaultBucket();
    FTUtil::CreateDefaultUser();

    return RUN_ALL_TESTS();
}

