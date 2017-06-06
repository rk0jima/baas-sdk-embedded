#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "ft_cloudfn.h"
#include "necbaas/nb_user.h"
#include "necbaas/nb_file_bucket.h"
#include "necbaas/nb_object_bucket.h"
#include "necbaas/nb_api_gateway.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;

class NbCommunicationFT : public ::testing::Test {
  protected:
    static void SetUpTestCase() {
        service_ = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
        FTCloudfn::RegisterCloudfnDefault(NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy));
    }
    static void TearDownTestCase() {
        FTUtil::DeleteAllObject();
        FTUtil::DeleteAllFile();
        FTCloudfn::DeleteCloudfnDefault(NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy));
    }

    virtual void SetUp() {}
    virtual void TearDown() {
        if (NbUser::IsLoggedIn(service_)) {
            NbResult<NbUser> result = NbUser::Logout(service_);
        }
    }

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
};

shared_ptr<NbService> NbCommunicationFT::service_;

// URLエンコード（パス）
TEST_F(NbCommunicationFT, UrlEncodingPath) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    const string test_file_name{R"(a0-._~#[]@!$&()+,;= %^{}あア漢.txt)"};
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(test_file_name, FTUtil::MakeFilePath(kUploadFile), kContentType);

    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(test_file_name, response.GetFileName());
}

// URLエンコード（クエリパラメータ）
TEST_F(NbCommunicationFT, UrlEncodingQurey) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    const string test_string{R"(a0-._~:/?#[]@!$&'()*+,;= "%^|{}<>\あア漢)"};
    object["key"] = test_string;

    ASSERT_TRUE(object.Save().IsSuccess());

    NbQuery qurey = NbQuery().EqualTo("key", test_string);
    NbResult<std::vector<NbObject>> result = object_bucket.Query(qurey);
    ASSERT_TRUE(result.IsSuccess());
    vector<NbObject> object_list = result.GetSuccessData();
    EXPECT_EQ(1, object_list.size());
    EXPECT_EQ(test_string, object_list[0].GetString("key"));
}

// テナントIDが空文字
TEST_F(NbCommunicationFT, TenantIdEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kEmpty, kAppId, kAppKey, kProxy);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);

    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_TENANT_ID, result.GetResultCode());
}

// アプリケーションIDが空文字
TEST_F(NbCommunicationFT, ApplicationIdEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kEmpty, kAppKey, kProxy);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);

    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_APP_ID, result.GetResultCode());
}

// アプリケーションキーが空文字
TEST_F(NbCommunicationFT, ApplicationKeyEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kEmpty, kProxy);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);

    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_APP_KEY, result.GetResultCode());
}

// エンドポイントURLが空文字
TEST_F(NbCommunicationFT, EndPointErlEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);

    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
}

// テナントIDがサーバに存在しない
TEST_F(NbCommunicationFT, TenantIdInvalid) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, "InvalidTenantId", kAppId, kAppKey, kProxy);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);

    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(404, result.GetRestError().status_code);
}

// アプリケーションIDがサーバに存在しない
TEST_F(NbCommunicationFT, ApplicationIdInvalid) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, "InvalidAppId", kAppKey, kProxy);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);

    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(401, result.GetRestError().status_code);
}

// アプリケーションキーがサーバに存在しない
TEST_F(NbCommunicationFT, ApplicationKeyInvalid) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, "InvalidAppKey", kProxy);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);

    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(401, result.GetRestError().status_code);
}

// 通信成功
TEST_F(NbCommunicationFT, CreateObject) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    const string test_string{"testString"};
    object["key"] = test_string;

    NbResult<NbObject> result = object.Save();

    ASSERT_TRUE(result.IsSuccess());
    EXPECT_EQ(test_string, result.GetSuccessData().GetString("key"));
}

// サーバからエラー返却
TEST_F(NbCommunicationFT, CreateObjectError) {
    NbObjectBucket object_bucket(service_, "invalidBucket");
    NbObject object = object_bucket.NewObject();
    const string test_string{"testString"};
    object["key"] = test_string;

    NbResult<NbObject> result = object.Save();

    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));
}

// タイムアウト（デフォルト60秒）
TEST_F(NbCommunicationFT, TimuoutDefaultSlowTest) {
    NbApiGateway get(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, "/wait70");

    auto start = std::chrono::system_clock::now();
    NbResult<NbHttpResponse> result = get.ExecuteCustomApi();
    auto end = std::chrono::system_clock::now();
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, result.GetResultCode());
    auto exec_time = end - start;
    EXPECT_LE(60, std::chrono::duration_cast<std::chrono::seconds>(exec_time).count());
    EXPECT_GE(61, std::chrono::duration_cast<std::chrono::seconds>(exec_time).count());
}

// タイムアウト（値変更、他のインスタンスには影響ないこと）
TEST_F(NbCommunicationFT, ChangeTimeoutSlowTest) {
    NbApiGateway get(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, "/wait70");
    NbApiGateway get2(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, "/wait70");
    get.SetTimeout(30);

    auto start = std::chrono::system_clock::now();
    NbResult<NbHttpResponse> result = get.ExecuteCustomApi();
    auto end = std::chrono::system_clock::now();
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, result.GetResultCode());
    auto exec_time = end - start;
    EXPECT_LE(30, std::chrono::duration_cast<std::chrono::seconds>(exec_time).count());
    EXPECT_GE(31, std::chrono::duration_cast<std::chrono::seconds>(exec_time).count());

    start = std::chrono::system_clock::now();
    result = get2.ExecuteCustomApi();
    end = std::chrono::system_clock::now();
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, result.GetResultCode());
    exec_time = end - start;
    EXPECT_LE(60, std::chrono::duration_cast<std::chrono::seconds>(exec_time).count());
    EXPECT_GE(61, std::chrono::duration_cast<std::chrono::seconds>(exec_time).count());
}

// タイムアウト（ファイルアップロード）
TEST_F(NbCommunicationFT, TimuoutFileUploadSlowTest) {
    int file_size = 1024*1024*100;
    FTUtil::CreateTestFile("testfile.txt", file_size);
    NbFileBucket file_bucket(service_, kFileBucketName);
    file_bucket.SetTimeout(1);

    NbResult<NbFileMetadata> upload = file_bucket.UploadNewFile(kFileName, "testfile.txt", kContentType);
    ASSERT_TRUE(upload.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, upload.GetResultCode());

    std::remove("testfile.txt");
}

// タイムアウト（ファイルダウンロード）
TEST_F(NbCommunicationFT, TimuoutFileDownloadSlowTest) {
    int file_size = 1024*1024*100;
    FTUtil::CreateTestFile("testfile.txt", file_size);
    NbFileBucket file_bucket(service_, kFileBucketName);
    file_bucket.SetTimeout(3600);

    NbResult<NbFileMetadata> upload = file_bucket.UploadNewFile(kFileName, "testfile.txt", kContentType);
    ASSERT_TRUE(upload.IsSuccess());
    EXPECT_EQ(file_size, upload.GetSuccessData().GetLength());

    file_bucket.SetTimeout(1);
    NbResult<int> download = file_bucket.DownloadFile(kFileName, "testfile2.txt");
    ASSERT_TRUE(download.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, download.GetResultCode());

    std::remove("testfile.txt");
}

// 最大接続数
TEST_F(NbCommunicationFT, ConnectionMaxSlowTest) {
    std::thread threads[20];
    for (int i = 0; i < 20; ++i) {
        std::thread rest_thread( []() {
            NbApiGateway get(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, "/wait40");
            get.SetTimeout(80);
            NbResult<NbHttpResponse> result = get.ExecuteCustomApi();
            ASSERT_TRUE(result.IsSuccess());
        });
        threads[i] = std::move(rest_thread);
        sleep(1);
    }

    NbApiGateway get(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, "/wait40");
    NbResult<NbHttpResponse> result = get.ExecuteCustomApi();
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());

    // スレッド終了待ち
    for (int i = 0; i < 20; ++i) {
        threads[i].join();
    }
}

} //namespace necbaas
