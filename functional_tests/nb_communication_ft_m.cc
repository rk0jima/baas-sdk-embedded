#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_object_bucket.h"
#include "necbaas/nb_object.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;

class NbCommunicationManual : public ::testing::Test {
  protected:
    static void SetUpTestCase() {
        service_ = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    }
    static void TearDownTestCase() {}

    virtual void SetUp() {}
    virtual void TearDown() {
        if (NbUser::IsLoggedIn(service_)) {
            NbResult<NbUser> result = NbUser::Logout(service_);
        }
    }

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
};

shared_ptr<NbService> NbCommunicationManual::service_;

// 通信異常（圏外）
TEST_F(NbCommunicationManual, CouldNotConnect) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, result.GetResultCode());
}

// 通信異常（接続先URLが不正）
TEST_F(NbCommunicationManual, InvalidEndPointUrl) {
    shared_ptr<NbService> service = NbService::CreateService("http://xx.xx.xx.xx:8081", kTenantId, kAppId, kAppKey, kProxy);
    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);

    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, result.GetResultCode());
}

// 各種ヘッダ確認
TEST_F(NbCommunicationManual, CheckHeaders) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(login.IsSuccess());
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();

    NbResult<NbObject> save = object.Save();
    ASSERT_TRUE(save.IsSuccess());

    // セッションをタイムアウトさせる
    sleep(60);

    // ログイン状態でなくなるので、権限エラー
    save = object.Save();
    ASSERT_TRUE(save.IsRestError());
    EXPECT_EQ(403, save.GetRestError().status_code);
}

// Proxy設定
TEST_F(NbCommunicationManual, SetProxy) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, "http://proxygate.nic.nec.co.jp:8080");

    NbResult<NbUser> login = NbUser::LoginWithUsername(service, kUserName, kPassword);
    // 結果はログで確認
}

// Proxy未設定
TEST_F(NbCommunicationManual, NoProxy) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey);

    NbResult<NbUser> login = NbUser::LoginWithUsername(service, kUserName, kPassword);
    // 結果はログで確認
}

// 10回連続送信
TEST_F(NbCommunicationManual, Rest10Times) {
    auto start = std::chrono::system_clock::now();
    ASSERT_TRUE(NbUser::LoginWithUsername(service_, kUserName, kPassword).IsSuccess());

    for (int i; i < 8; ++i) {
        NbObjectBucket object_bucket(service_, kObjectBucketName);
        NbObject object = object_bucket.NewObject();
        ASSERT_TRUE(object.Save().IsSuccess());
    }

    ASSERT_TRUE(NbUser::Logout(service_).IsSuccess());
    auto end = std::chrono::system_clock::now();
    EXPECT_GE(1, std::chrono::duration_cast<std::chrono::seconds>(end - start).count());
}

} //namespace necbaas
