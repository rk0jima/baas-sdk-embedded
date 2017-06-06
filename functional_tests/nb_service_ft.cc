#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_user.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;

// サービス生成から各種データ取得
TEST(NbServiceFT, CreateService) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, "http://proxy.example.com:8080");

    EXPECT_EQ(kEndPointUrl, service->GetEndpointUrl());
    EXPECT_EQ(kTenantId, service->GetTenantId());
    EXPECT_EQ(kAppId, service->GetAppId());
    EXPECT_EQ(kAppKey, service->GetAppKey());
    EXPECT_EQ(string("http://proxy.example.com:8080"), service->GetProxy());
}

// サービス生成から各種データ取得(空文字)
TEST(NbServiceFT, CreateServiceEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kEmpty, kEmpty, kEmpty);

    EXPECT_TRUE(service->GetEndpointUrl().empty());
    EXPECT_TRUE(service->GetTenantId().empty());
    EXPECT_TRUE(service->GetAppId().empty());
    EXPECT_TRUE(service->GetAppKey().empty());
    EXPECT_TRUE(service->GetProxy().empty());
}

// EndPointUrlの最後にスラッシュあり
TEST(NbServiceFT, UrlSlashExist) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl + "/", kTenantId, kAppId, kAppKey, kProxy);

    EXPECT_EQ('/', *service->GetEndpointUrl().rbegin());

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);
    // 通信成功でＯＫとする
    ASSERT_TRUE(result.IsSuccess());
    NbUser::Logout(service);
}

// EndPointUrlの最後にスラッシュなし
TEST(NbServiceFT, UrlSlashNone) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    EXPECT_NE('/', *service->GetEndpointUrl().rbegin());

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);
    // 通信成功でＯＫとする
    ASSERT_TRUE(result.IsSuccess());
    NbUser::Logout(service);
}

// マルチテナント
TEST(NbServiceFT, MultiTenant) {
    shared_ptr<NbService> service_a = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, "http://proxy.example.com:8080");
    shared_ptr<NbService> service_b = NbService::CreateService(kEndPointUrl, kTenantIdB, kAppIdB, kAppKeyB, "http://proxy.exampleB.com:8080");

    EXPECT_EQ(kEndPointUrl, service_a->GetEndpointUrl());
    EXPECT_EQ(kTenantId, service_a->GetTenantId());
    EXPECT_EQ(kAppId, service_a->GetAppId());
    EXPECT_EQ(kAppKey, service_a->GetAppKey());
    EXPECT_EQ("http://proxy.example.com:8080", service_a->GetProxy());

    EXPECT_EQ(kEndPointUrl, service_b->GetEndpointUrl());
    EXPECT_EQ(kTenantIdB, service_b->GetTenantId());
    EXPECT_EQ(kAppIdB, service_b->GetAppId());
    EXPECT_EQ(kAppKeyB, service_b->GetAppKey());
    EXPECT_EQ("http://proxy.exampleB.com:8080", service_b->GetProxy());
}
} //namespace necbaas
