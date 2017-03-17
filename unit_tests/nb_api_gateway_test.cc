#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "necbaas/nb_api_gateway.h"
#include "necbaas/internal/nb_utility.h"
#include "rest_api_mock.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;

static const string kEmpty{""};

static const string kApiname{"apiName"};
static const string kSubpath{"/subPath"};

static const std::vector<char> kRequestBody{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
static const std::vector<char> kResponseBody{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};

// テスト名を変更するため、フィクスチャを継承
class NbApiGatewayTest : public RestApiTest {};

static NbResult<NbHttpResponse> GetNorm1(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/api/" + kApiname + kSubpath), request.GetUrl().substr(request.GetUrl().find("/api/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);
    
    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), kResponseBody);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

//NbApiGateway::ExecuteCustomApi(GET, サブパスあり、ヘッダなし、ボディなし)
TEST_F(NbApiGatewayTest, Get1) {
    SetExpect(&executor_, &GetNorm1);

    shared_ptr<NbService> service(mock_service_);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kSubpath);
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string("OK"), response.GetReasonPhrase());
    EXPECT_TRUE(response.GetHeaders().empty());
    EXPECT_EQ(kResponseBody, response.GetBody());
}

static NbResult<NbHttpResponse> GetNorm2(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/api/" + kApiname + "?where=param1"), request.GetUrl().substr(request.GetUrl().find("/api/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    bool check_header = false;
    for (auto header : request.GetHeaders()) {
        if (header.find("X-HEADER-KEY1:") != std::string::npos) {
            check_header = true;
            break;
        }
    }
    EXPECT_TRUE(check_header);
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(10, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), kResponseBody);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

//NbApiGateway::ExecuteCustomApi(GET, サブパスなし、その他全て設定)
TEST_F(NbApiGatewayTest, Get2) {
    SetExpect(&executor_, &GetNorm2);

    shared_ptr<NbService> service(mock_service_);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kEmpty);
    apigw.AddHeader("X-HEADER-KEY1", "abcdef");
    apigw.AddParameter("where", "param1");
    apigw.SetContentType("text/plain");
    EXPECT_EQ(string("text/plain"), apigw.GetContentType());
    apigw.SetTimeout(10);
    EXPECT_EQ(10, apigw.GetTimeout());

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(kRequestBody);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string("OK"), response.GetReasonPhrase());
    EXPECT_TRUE(response.GetHeaders().empty());
    EXPECT_EQ(kResponseBody, response.GetBody());
}

static NbResult<NbHttpResponse> PostNorm1(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/api/" + kApiname + kSubpath), request.GetUrl().substr(request.GetUrl().find("/api/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), kResponseBody);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

//NbApiGateway::ExecuteCustomApi(POST, サブパスあり、ヘッダなし、ボディなし)
TEST_F(NbApiGatewayTest, Post1) {
    SetExpect(&executor_, &PostNorm1);

    shared_ptr<NbService> service(mock_service_);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kSubpath);
    apigw.SetContentType("text/plain");
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string("OK"), response.GetReasonPhrase());
    EXPECT_TRUE(response.GetHeaders().empty());
    EXPECT_EQ(kResponseBody, response.GetBody());
}

static NbResult<NbHttpResponse> PostNorm2(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/api/" + kApiname + kSubpath), request.GetUrl().substr(request.GetUrl().find("/api/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(5, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("X-HEADER-KEY1:") != std::string::npos) {
            ++check_header;
            continue;
        }
        if (header.find("Content-Type:") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(2, check_header);
    EXPECT_EQ(string(kRequestBody.begin(), kRequestBody.end()), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), kResponseBody);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

//NbApiGateway::ExecuteCustomApi(POST, 全て設定)
TEST_F(NbApiGatewayTest, Post2) {
    SetExpect(&executor_, &PostNorm2);

    shared_ptr<NbService> service(mock_service_);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kSubpath);
    apigw.AddHeader("X-HEADER-KEY1", "abcdef");
    apigw.AddParameter("where", "param1");
    apigw.SetContentType("text/plain");
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(kRequestBody);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string("OK"), response.GetReasonPhrase());
    EXPECT_TRUE(response.GetHeaders().empty());
    EXPECT_EQ(kResponseBody, response.GetBody());
}

//NbApiGateway::ExecuteCustomApi(POST, Content-Type未設定)
TEST_F(NbApiGatewayTest, PostContentTypeNone) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kSubpath);
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(string(kRequestBody.begin(), kRequestBody.end()));

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONTENT_TYPE, result.GetResultCode());
}

static NbResult<NbHttpResponse> PutNorm1(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/api/" + kApiname + kSubpath), request.GetUrl().substr(request.GetUrl().find("/api/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), kResponseBody);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

//NbApiGateway::ExecuteCustomApi(PUT, サブパスあり、ヘッダなし、ボディなし)
TEST_F(NbApiGatewayTest, Put1) {
    SetExpect(&executor_, &PutNorm1);

    shared_ptr<NbService> service(mock_service_);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);
    apigw.SetContentType("text/plain");
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string("OK"), response.GetReasonPhrase());
    EXPECT_TRUE(response.GetHeaders().empty());
    EXPECT_EQ(kResponseBody, response.GetBody());
}

static NbResult<NbHttpResponse> PutNorm2(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/api/" + kApiname + kSubpath), request.GetUrl().substr(request.GetUrl().find("/api/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    EXPECT_EQ(5, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("X-HEADER-KEY1:") != std::string::npos) {
            ++check_header;
            continue;
        }
        if (header.find("Content-Type:") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(2, check_header);
    EXPECT_EQ(string(kRequestBody.begin(), kRequestBody.end()), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), kResponseBody);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

//NbApiGateway::ExecuteCustomApi(PUT, 全て設定)
TEST_F(NbApiGatewayTest, Put2) {
    SetExpect(&executor_, &PutNorm2);

    shared_ptr<NbService> service(mock_service_);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);
    apigw.AddHeader("X-HEADER-KEY1", "abcdef");
    apigw.AddParameter("where", "param1");
    apigw.SetContentType("text/plain");
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(string(kRequestBody.begin(), kRequestBody.end()));

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string("OK"), response.GetReasonPhrase());
    EXPECT_TRUE(response.GetHeaders().empty());
    EXPECT_EQ(kResponseBody, response.GetBody());
}

//NbApiGateway::ExecuteCustomApi(PUT, Content-Type未設定)
TEST_F(NbApiGatewayTest, PutContentTypeNone) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(kRequestBody);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONTENT_TYPE, result.GetResultCode());
}

static NbResult<NbHttpResponse> DeleteNorm1(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/api/" + kApiname + kSubpath), request.GetUrl().substr(request.GetUrl().find("/api/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);
    
    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), kResponseBody);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

//NbApiGateway::ExecuteCustomApi(DELETE, サブパスあり、ヘッダなし、ボディなし)
TEST_F(NbApiGatewayTest, Delete1) {
    SetExpect(&executor_, &DeleteNorm1);

    shared_ptr<NbService> service(mock_service_);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, kSubpath);
    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string("OK"), response.GetReasonPhrase());
    EXPECT_TRUE(response.GetHeaders().empty());
    EXPECT_EQ(kResponseBody, response.GetBody());
}

static NbResult<NbHttpResponse> DeleteNorm2(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/api/" + kApiname + "?where=param1"), request.GetUrl().substr(request.GetUrl().find("/api/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    bool check_header = false;
    for (auto header : request.GetHeaders()) {
        if (header.find("X-HEADER-KEY1:") != std::string::npos) {
            check_header = true;
            break;
        }
    }
    EXPECT_TRUE(check_header);
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(10, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), kResponseBody);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

//NbApiGateway::ExecuteCustomApi(DELETE, サブパスなし、その他全て設定)
TEST_F(NbApiGatewayTest, Delete2) {
    SetExpect(&executor_, &DeleteNorm2);

    shared_ptr<NbService> service(mock_service_);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, kEmpty);
    apigw.AddHeader("X-HEADER-KEY1", "abcdef");
    apigw.AddParameter("where", "param1");
    apigw.SetContentType("text/plain");
    apigw.SetTimeout(10);

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(kRequestBody);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string("OK"), response.GetReasonPhrase());
    EXPECT_TRUE(response.GetHeaders().empty());
    EXPECT_EQ(kResponseBody, response.GetBody());
}

//NbApiGateway::ExecuteCustomApi(api_name空)
TEST(NbApiGateway, ExecuteCustomApiNameEmpty) { 
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbApiGateway apigw(service, kEmpty, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kEmpty);

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_API_NAME, result.GetResultCode());    
}

//NbApiGateway::Logout(request_factory構築失敗)
TEST(NbApiGateway, ExecuteCustomApiRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kEmpty);

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
}

//NbApiGateway::Logout(接続数オーバー)
TEST_F(NbApiGatewayTest, ExecuteCustomApiConnectionOver) {
    SetExpect(nullptr, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kEmpty);

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
}

//NbApiGateway::Headers操作
TEST(NbApiGateway, Headers) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kEmpty);
    std::multimap<string, string> exp_header;

    EXPECT_TRUE(apigw.GetHeaders().empty());

    apigw.AddHeader("Haeder-A", "Value-A");
    exp_header.insert(std::make_pair("Haeder-A", "Value-A"));
    EXPECT_EQ(exp_header, apigw.GetHeaders());

    apigw.AddHeader("Haeder-B", "Value-B");
    exp_header.insert(std::make_pair("Haeder-B", "Value-B"));
    EXPECT_EQ(exp_header, apigw.GetHeaders());

    apigw.AddHeader("Haeder-A", "Value-C");
    exp_header.insert(std::make_pair("Haeder-A", "Value-C"));
    EXPECT_EQ(exp_header, apigw.GetHeaders());

    EXPECT_EQ(2, apigw.RemoveHeader("Haeder-A"));
    exp_header.erase("Haeder-A");
    EXPECT_EQ(exp_header, apigw.GetHeaders());

    apigw.AddHeader(kEmpty, "Value-D");
    EXPECT_EQ(exp_header, apigw.GetHeaders());

    apigw.AddHeader("Haeder-D", kEmpty);
    EXPECT_EQ(exp_header, apigw.GetHeaders());

    apigw.AddHeader("Content-Type", "application/json");
    EXPECT_EQ(exp_header, apigw.GetHeaders());

    apigw.AddHeader("Haeder-A", "Value-A");
    apigw.AddHeader("Haeder-C", "Value-C");
    apigw.ClearHeaders();
    EXPECT_TRUE(apigw.GetHeaders().empty());
    apigw.ClearHeaders();
    EXPECT_TRUE(apigw.GetHeaders().empty());
}

//NbApiGateway::Parameters操作
TEST(NbApiGateway, Parameters) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbApiGateway apigw(service, kApiname, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kEmpty);
    std::multimap<string, string> exp_parameter;

    EXPECT_TRUE(apigw.GetParameters().empty());

    apigw.AddParameter("Param-A", "Value-A");
    exp_parameter.insert(std::make_pair("Param-A", "Value-A"));
    EXPECT_EQ(exp_parameter, apigw.GetParameters());

    apigw.AddParameter("Param-B", "Value-B");
    exp_parameter.insert(std::make_pair("Param-B", "Value-B"));
    EXPECT_EQ(exp_parameter, apigw.GetParameters());

    apigw.AddParameter("Param-A", "Value-C");
    exp_parameter.insert(std::make_pair("Param-A", "Value-C"));
    EXPECT_EQ(exp_parameter, apigw.GetParameters());

    EXPECT_EQ(2, apigw.RemoveParameter("Param-A"));
    exp_parameter.erase("Param-A");
    EXPECT_EQ(exp_parameter, apigw.GetParameters());

    apigw.AddParameter(kEmpty, "Value-D");
    EXPECT_EQ(exp_parameter, apigw.GetParameters());

    apigw.AddParameter("Param-D", kEmpty);
    EXPECT_EQ(exp_parameter, apigw.GetParameters());

    apigw.AddParameter("Param-A", "Value-A");
    apigw.AddParameter("Param-C", "Value-C");
    apigw.ClearParameters();
    EXPECT_TRUE(apigw.GetParameters().empty());
    apigw.ClearParameters();
    EXPECT_TRUE(apigw.GetParameters().empty());
}
} //namespace necbaas
