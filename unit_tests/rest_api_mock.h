#ifndef NECBAAS_TESTUTIL_H
#define NECBAAS_TESTUTIL_H

#include "necbaas/nb_service.h"

namespace necbaas {
// REST APIを実行するメソッドをテストするためのMock, Fixture定義

using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

static const std::string kEndPointUrl{"endPointUrl"};
static const std::string kTenantId{"tenantID"};
static const std::string kAppId{"applicationId"};
static const std::string kAppKey{"applicationKey"};
static const std::string kProxy{"proxyUrl"};

class MockService : public NbService {
  public:
    MockService(const std::string &endpoint_url, const std::string &tenant_id,
                const std::string &app_id, const std::string &app_key, const std::string &proxy)
        : NbService(endpoint_url, tenant_id, app_id, app_key, proxy) {}
    MOCK_METHOD0(PopRestExecutor, NbRestExecutor *());
    MOCK_METHOD1(PushRestExecutor, void(NbRestExecutor *executor));
};

class MockRestExecutor : public NbRestExecutor {
  public:
    MOCK_METHOD2(ExecuteRequest, NbResult<NbHttpResponse>(const NbHttpRequest &request, int timeout));
    MOCK_METHOD3(ExecuteFileDownload, NbResult<NbHttpResponse>(const NbHttpRequest &request, const std::string &file_path, int timeout));
    MOCK_METHOD3(ExecuteFileUpload, NbResult<NbHttpResponse>(const NbHttpRequest &request, const std::string &file_path, int timeout));
};

class RestApiTest : public ::testing::Test {
  protected:
    virtual void SetUp() {
        mock_service_ = new MockService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    }

    virtual void TearDown() {}

    void SetExpect(MockRestExecutor *executor,
                   std::function<NbResult<NbHttpResponse>(const NbHttpRequest &, int)> callback_func) {
        EXPECT_CALL(*mock_service_, PopRestExecutor())
        .WillOnce(Return(executor));

        if (executor) {
            EXPECT_CALL(*mock_service_, PushRestExecutor(executor))
            .WillOnce(Return());
        }

        if (callback_func) {
            EXPECT_CALL(executor_, ExecuteRequest(_, _))
            .WillOnce(Invoke(callback_func));
        }
    }

    void SetExpectDownload(MockRestExecutor *executor, const std::string &file_path,
                           std::function<NbResult<NbHttpResponse>(const NbHttpRequest &, const std::string &, int)> callback_func) {
        EXPECT_CALL(*mock_service_, PopRestExecutor())
        .WillOnce(Return(executor));

        if (executor) {
            EXPECT_CALL(*mock_service_, PushRestExecutor(executor))
            .WillOnce(Return());
        }

        if (callback_func) {
            EXPECT_CALL(executor_, ExecuteFileDownload(_, file_path, _))
            .WillOnce(Invoke(callback_func));
        }
    }

    void SetExpectUpload(MockRestExecutor *executor, const std::string &file_path,
                         std::function<NbResult<NbHttpResponse>(const NbHttpRequest &, const std::string &, int)> callback_func) {
        EXPECT_CALL(*mock_service_, PopRestExecutor())
        .WillOnce(Return(executor));

        if (executor) {
            EXPECT_CALL(*mock_service_, PushRestExecutor(executor))
            .WillOnce(Return());
        }

        if (callback_func) {
            EXPECT_CALL(executor_, ExecuteFileUpload(_, file_path, _))
            .WillOnce(Invoke(callback_func));
        }
    }

    MockService *mock_service_;
    MockRestExecutor executor_;
};    
}//namespace necbaas

#endif //NECBAAS_TESTUTIL_H
