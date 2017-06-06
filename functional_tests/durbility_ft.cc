#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "ft_cloudfn.h"
#include "necbaas/nb_object_bucket.h"
#include "necbaas/nb_user.h"
#include "necbaas/nb_file_bucket.h"
#include "necbaas/nb_api_gateway.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;
using std::chrono::system_clock;

// フィクスチャ定義
class DurbilityTestManual : public ::testing::Test {
  protected:
    static void SetUpTestCase() {
        service_ = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
        master_service_ = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
        FTCloudfn::RegisterCloudfnDefault(master_service_);
    }
    static void TearDownTestCase() {
        FTCloudfn::DeleteCloudfnDefault(master_service_);
    }

    virtual void SetUp() {}
    virtual void TearDown() {
        if (NbUser::IsLoggedIn(service_)) {
            NbResult<NbUser> result = NbUser::Logout(service_);
        }
        FTUtil::DeleteAllObject();
    }

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
    static shared_ptr<NbService> master_service_;

    // テスト用オブジェクト
    NbObject object_;
    NbFileMetadata file_metadata_;

    void Login();
    void Logout();
    void UpdateObject(int value);
    void QueryObject(int value);
    void UplodatFile();
    void DownloadFile();
    void ExecuteCustomApi();
};

shared_ptr<NbService> DurbilityTestManual::service_;
shared_ptr<NbService> DurbilityTestManual::master_service_;

// 戻り値確認(デフォルトユーザ)
static void CheckDefaultUser(const NbUser &user) {
    EXPECT_EQ(kUserName, user.GetUserName());
    EXPECT_FALSE(user.GetUserId().empty());
    EXPECT_EQ(kEmail, user.GetEmail());
    EXPECT_EQ(kOptions, user.GetOptions());
    FTUtil::CompareList(kGroups, user.GetGroups());
    EXPECT_NE(0, user.GetCreatedTime().tm_year);
    EXPECT_NE(0, user.GetUpdatedTime().tm_year);
}

// 初期値確認（Login）
static void CheckInitialUser(const NbUser &initial_user) {
    EXPECT_TRUE(initial_user.GetUserName().empty());
    EXPECT_TRUE(initial_user.GetUserId().empty());
    EXPECT_TRUE(initial_user.GetEmail().empty());
    EXPECT_TRUE(initial_user.GetOptions().IsEmpty());
    EXPECT_TRUE(initial_user.GetGroups().empty());
    EXPECT_EQ(0, initial_user.GetCreatedTime().tm_year);
    EXPECT_EQ(0, initial_user.GetUpdatedTime().tm_year);
}

void DurbilityTestManual::Login() {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    EXPECT_FALSE(NbUser::GetSessionToken(service_).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_));
    NbUser current_user = NbUser::GetCurrentUser(service_);

    // NbUserの比較
    FTUtil::CompareUser(user, current_user);
}

void DurbilityTestManual::Logout() {
    //ログアウト
    NbResult<NbUser> result = NbUser::Logout(service_);
    EXPECT_TRUE(result.IsSuccess());
    NbUser user_logout = result.GetSuccessData();
    EXPECT_FALSE(user_logout.GetUserId().empty());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));

    // ログイン情報破棄確認
    NbUser current_user = NbUser::GetCurrentUser(service_);
    CheckInitialUser(current_user);
    EXPECT_TRUE(current_user.GetSessionToken(service_).empty());
    EXPECT_EQ(0, current_user.GetSessionTokenExpiration(service_));
}

void DurbilityTestManual::UpdateObject(int value) {
    // オブジェクト更新
    object_["key"] = value;
    NbResult<NbObject> result_update = object_.Save();

    // 戻り値確認
    ASSERT_TRUE(result_update.IsSuccess());
    NbObject response_update = result_update.GetSuccessData();
    EXPECT_EQ(value, response_update.GetInt("key"));
}

void DurbilityTestManual::QueryObject(int value) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    // オブジェクトクエリ
    NbQuery query;
    NbResult<std::vector<NbObject>> result_query = object_bucket.Query(query);

    // 戻り値確認
    ASSERT_TRUE(result_query.IsSuccess());
    std::vector<NbObject> response_query = result_query.GetSuccessData();
    EXPECT_EQ(1, response_query.size());
    EXPECT_EQ(value, response_query[0].GetInt("key"));
}

void DurbilityTestManual::UplodatFile() {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(FTUtil::MakeFilePath(kUploadFile), file_metadata_);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    file_metadata_ = result.GetSuccessData();

    EXPECT_EQ(kFileName, file_metadata_.GetFileName());
}

void DurbilityTestManual::DownloadFile() {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<int> result = file_bucket.DownloadFile(kFileName, kDownloadFile);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    int response = result.GetSuccessData();

    FTUtil::CompareFiledata(FTUtil::MakeFilePath(kUploadFile), kDownloadFile, response);
}

void DurbilityTestManual::ExecuteCustomApi() {
    std::string body_str{"abcde"};
    std::vector<char> body_bin{'v','w','x','y','z'};

    // GET
    NbApiGateway get(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kSubpath);
    get.AddHeader("test-header", "get-value");
    get.AddParameter("param1", "abc");
    get.AddParameter("param2", "123");
    NbResult<NbHttpResponse> result = get.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    std::multimap<std::string, std::string> headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "test-header", "get-value"));
    NbJsonObject json(response.GetBody());
    NbJsonObject param = json.GetJsonObject("param");
    EXPECT_EQ(string("abc"), param.GetJsonArray("param1").GetString(0));
    EXPECT_EQ(string("123"), param.GetJsonArray("param2").GetString(0));
    EXPECT_TRUE(json.GetString("body").empty());

    // PUT
    NbApiGateway put(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);
    put.SetContentType("text/plain");
    result = put.ExecuteCustomApi(body_str);
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    json = NbJsonObject(response.GetBody());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "Content-Type", "text/plain"));
    EXPECT_EQ(body_str, json.GetString("body"));

    // POST
    NbApiGateway post(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kSubpath);
    post.SetContentType("text/xml");
    result = post.ExecuteCustomApi(body_bin);
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    json = NbJsonObject(response.GetBody());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "Content-Type", "text/xml"));
    EXPECT_EQ(string(body_bin.begin(), body_bin.end()), json.GetString("body"));

    // DELETE
    NbApiGateway del(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, kSubpath);
    del.AddHeader("test-header", "delete-value");
    del.AddParameter("param3", "xyz");
    del.AddParameter("param4", "789");
    result = del.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "test-header", "delete-value"));
    json = NbJsonObject(response.GetBody());
    param = json.GetJsonObject("param");
    EXPECT_EQ(string("xyz"), param.GetJsonArray("param3").GetString(0));
    EXPECT_EQ(string("789"), param.GetJsonArray("param4").GetString(0));
    EXPECT_TRUE(json.GetString("body").empty());
}

// 性能.耐久
TEST_F(DurbilityTestManual, DurbilityAll) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    object_ = object_bucket.NewObject();
    object_["key"] = 0;
    object_.Save();

    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> upload_result =
         file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile),
                                   kContentType, true);
    ASSERT_TRUE(upload_result.IsSuccess());
    file_metadata_ = upload_result.GetSuccessData();

    system_clock::time_point start_time = system_clock::now();

    for (int i = 1; ; i++) {
        Login();
        UpdateObject(i);
        QueryObject(i);
        UplodatFile();
        DownloadFile();
        ExecuteCustomApi();
        Logout();

        // 120secスリープ//
        sleep(120);

        if (system_clock::now() > start_time + std::chrono::hours(48)) {
            break;
        }
    }
}
} //namespace necbaas
