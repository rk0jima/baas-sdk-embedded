#include "gtest/gtest.h"
#include "necbaas/nb_service.h"
#include "test_util.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;

static const string kEndPointUrl{"endPointUrl"};
static const string kTenantId{"tenantID"};
static const string kAppId{"applicationId"};
static const string kAppKey{"applicationKey"};
static const string kProxy{"proxyUrl"};

static const string kSessionToken{"sessionToken"};
static const time_t kExpireAt{0x12345678};

//NbService::CreateService(文字列あり)
TEST(NbService, CreateService) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    EXPECT_EQ(kEndPointUrl, service->GetEndpointUrl());
    EXPECT_EQ(kTenantId, service->GetTenantId());
    EXPECT_EQ(kAppId, service->GetAppId());
    EXPECT_EQ(kAppKey, service->GetAppKey());
    EXPECT_EQ(kProxy, service->GetProxy());
}

//NbService::CreateService(文字列空)
TEST(NbService, CreateServiceEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(string(""), string(""), string(""), string(""), string(""));
    EXPECT_EQ(string(""), service->GetEndpointUrl());
    EXPECT_EQ(string(""), service->GetTenantId());
    EXPECT_EQ(string(""), service->GetAppId());
    EXPECT_EQ(string(""), service->GetAppKey());
    EXPECT_EQ(string(""), service->GetProxy());
}

//NbService(SessionToken)
TEST(NbService, SessionToken) {
    NbSessionToken session_token(kSessionToken, kExpireAt);
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    //Set & Get
    service->SetSessionToken(session_token);
    EXPECT_EQ(kSessionToken, service->GetSessionToken().GetSessionToken());
    EXPECT_EQ(kExpireAt, service->GetSessionToken().GetExpireAt());

    //Clear & Get
    service->ClearSessionToken();
    EXPECT_EQ(string(""), service->GetSessionToken().GetSessionToken());
    EXPECT_EQ(0, service->GetSessionToken().GetExpireAt());
}

//NbService(Executor)
TEST(NbService, Executor) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    // Pop → Push → Pop を実行
    NbRestExecutor *executor = service->PopRestExecutor();
    EXPECT_NE(nullptr, executor);

    service->PushRestExecutor(executor);

    NbRestExecutor *executor2 = service->PopRestExecutor();

    //１回目と２回目で取得したExecutorのポインタが同じであればOKとする
    EXPECT_EQ(executor, executor2);
}

//NbService(HttpRequestFactor)
TEST(NbService, HttpRequestFactor) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    NbSessionToken session_token(kSessionToken, kExpireAt);
    service->SetSessionToken(session_token);

    NbHttpRequestFactory request_factory = service->GetHttpRequestFactory();

    EXPECT_EQ(kEndPointUrl, TestUtil::NbHttpRequestFactory_GetEndPointUrl(request_factory));
    EXPECT_EQ(kTenantId, TestUtil::NbHttpRequestFactory_GetTenantId(request_factory));
    EXPECT_EQ(kAppId, TestUtil::NbHttpRequestFactory_GetAppId(request_factory));
    EXPECT_EQ(kAppKey, TestUtil::NbHttpRequestFactory_GetAppKey(request_factory));
    EXPECT_EQ(kSessionToken, TestUtil::NbHttpRequestFactory_GetSessionToken(request_factory));
    EXPECT_EQ(kProxy, TestUtil::NbHttpRequestFactory_GetProxy(request_factory));
}

//NbService ログ設定・読出
TEST(NbService, Settings) {
    vector<bool> flags{true, false, true};
    for (auto flag : flags) {
        NbService::SetDebugLogEnabled(flag);
        EXPECT_EQ(flag, NbLogger::IsDebugLogEnabled());

        NbService::SetErrorLogEnabled(flag);
        EXPECT_EQ(flag, NbLogger::IsErrorLogEnabled());

        NbService::SetRestLogEnabled(flag);
        EXPECT_EQ(flag, NbLogger::IsRestLogEnabled());
    }
}
} //namespace necbaas
