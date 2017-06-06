#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "ft_cloudfn.h"
#include "necbaas/nb_service.h"
#include "necbaas/nb_api_gateway.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;

class NbLoggerManual : public ::testing::Test {
  protected:
    static void SetUpTestCase() {
        service_ = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
        shared_ptr<NbService> master_service =
            NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
        FTCloudfn::RegisterCloudfnDefault(master_service);
    }
    static void TearDownTestCase() {
        shared_ptr<NbService> master_service =
            NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
        FTCloudfn::DeleteCloudfnDefault(master_service);
    }

    virtual void SetUp() {}
    virtual void TearDown() {}

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
};

shared_ptr<NbService> NbLoggerManual::service_;

static void CallLoggerFunctions() {
    NBLOG(INFO) << "Info log";
    NBLOG(WARNING) << "Wwarning log";
    NBLOG(ERROR) << "Error log";
    NBLOG(FATAL) << "Fatal log";
    NBLOG(DEBUG) << "Debug log";
    NBLOG(TRACE) << "Trace log";
    CVLOG(0, "necbaas") << "Verbose log";
}

static string Bool2String(bool flag) {
    return flag ? string("true") : string("false");
}

// ログ出力
TEST_F(NbLoggerManual, OutputLog) {
    std::cout << "Debug log : " << Bool2String(NbLogger::IsDebugLogEnabled()) << std::endl;
    std::cout << "Error log : " << Bool2String(NbLogger::IsErrorLogEnabled()) << std::endl;
    std::cout << "Rest log  : " << Bool2String(NbLogger::IsRestLogEnabled()) << std::endl;

    CallLoggerFunctions();
    
    for (int i = 0; i < 2; ++i) {
        std::cout << "Debug log change: " << Bool2String(NbLogger::IsDebugLogEnabled()) <<
             "--->" << Bool2String(!NbLogger::IsDebugLogEnabled()) << std::endl;
        NbService::SetDebugLogEnabled(!NbLogger::IsDebugLogEnabled());
        CallLoggerFunctions();

        std::cout << "Error log change: " << Bool2String(NbLogger::IsErrorLogEnabled()) <<
             "--->" << Bool2String(!NbLogger::IsErrorLogEnabled()) << std::endl;
        NbService::SetErrorLogEnabled(!NbLogger::IsErrorLogEnabled());
        CallLoggerFunctions();

        std::cout << "Rest log change: " << Bool2String(NbLogger::IsRestLogEnabled()) <<
             "--->" << Bool2String(!NbLogger::IsRestLogEnabled()) << std::endl;
        NbService::SetRestLogEnabled(!NbLogger::IsRestLogEnabled());
        CallLoggerFunctions();
    }
}

// RESTログ確認(application/json)
TEST_F(NbLoggerManual, ApplicationJson) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);

    string body{R"({"key":"val"})"};
    apigw.AddHeader("test-header", "test-value");
    apigw.SetContentType("application/json");
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(body);
}

// RESTログ確認(text/plain)
TEST_F(NbLoggerManual, TextPlain) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);

    string body{"abcde"};
    apigw.AddHeader("test-header", "test-value");
    apigw.SetContentType("text/plain");
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(body);
}

// RESTログ確認(text/html)
TEST_F(NbLoggerManual, TextHtml) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);

    string body{"<html><body>Hello!</body></html>"};
    apigw.AddHeader("test-header", "test-value");
    apigw.SetContentType("text/html");
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(body);
}

// RESTログ確認(text/xml)
TEST_F(NbLoggerManual, TextXml) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);

    string body{R"(<?xml version="1.0" encoding="Shift-Jis" ?><test>Hello</test>)"};
    apigw.AddHeader("test-header", "test-value");
    apigw.SetContentType("text/xml");
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(body);
}

// RESTログ確認(image/jpeg)
TEST_F(NbLoggerManual, ImageJpeg) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);

    string body{"jpeg data"};
    apigw.AddHeader("test-header", "test-value");
    apigw.SetContentType("image/jpeg");
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(body);
}

// マルチテナント
TEST_F(NbLoggerManual, MultiTenant) {
    shared_ptr<NbService> service_b = NbService::CreateService(kEndPointUrl, kTenantIdB, kAppIdB, kAppKeyB, kProxyB);

    NbUser::LoginWithUsername(service_, kUserName, kPassword);
    NbUser::LoginWithUsername(service_b, kUserName, kPassword);

    NbService::SetDebugLogEnabled(!NbLogger::IsDebugLogEnabled());

    NbUser::LoginWithUsername(service_, kUserName, kPassword);
    NbUser::LoginWithUsername(service_b, kUserName, kPassword);

    NbService::SetDebugLogEnabled(!NbLogger::IsDebugLogEnabled());
    NbService::SetRestLogEnabled(!NbLogger::IsRestLogEnabled());

    NbUser::LoginWithUsername(service_, kUserName, kPassword);
    NbUser::LoginWithUsername(service_b, kUserName, kPassword);
}

} //namespace necbaas
