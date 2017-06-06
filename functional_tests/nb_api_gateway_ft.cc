#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "ft_cloudfn.h"
#include "necbaas/nb_api_gateway.h"
#include "necbaas/internal/nb_utility.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;

class NbApiGatewayFT : public ::testing::Test {
  protected:
    static void SetUpTestCase() {
        service_ = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
        shared_ptr<NbService> master_service_a =
            NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
        shared_ptr<NbService> master_service_b =
            NbService::CreateService(kEndPointUrl, kTenantIdB, kAppIdB, kMasterKeyB, kProxyB);
        FTCloudfn::RegisterCloudfnDefault(master_service_a);
        FTCloudfn::RegisterCloudfnDefault(master_service_b);
    }
    static void TearDownTestCase() {
        shared_ptr<NbService> master_service_a =
            NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
        shared_ptr<NbService> master_service_b =
            NbService::CreateService(kEndPointUrl, kTenantIdB, kAppIdB, kMasterKeyB, kProxyB);
        FTCloudfn::DeleteCloudfnDefault(master_service_a);
        FTCloudfn::DeleteCloudfnDefault(master_service_b);
    }

    virtual void SetUp() {}
    virtual void TearDown() {}

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
};

shared_ptr<NbService> NbApiGatewayFT::service_;

// カスタムAPI呼び出し(GET)
// subpathあり、ヘッダあり(User-Agent設定)、リクエストパラメータあり、Bodyあり、Content-Typeあり
TEST_F(NbApiGatewayFT, ExecuteCustomApiGet) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kSubpath);

    std::vector<char> body{'a','b','c','d','e'};

    apigw.AddHeader("user-agent", "test-agent");
    apigw.AddHeader("test-header", "test-value");
    apigw.AddParameter("param1", "abc");
    apigw.AddParameter("param2", "123");
    apigw.SetContentType("text/plain");

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(body);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();

    EXPECT_EQ(200, response.GetStatusCode());

    std::multimap<std::string, std::string> headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "User-Agent", "test-agent"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "test-header", "test-value"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "Content-Type", "application/json"));

    NbJsonObject json(response.GetBody());
    NbJsonObject param = json.GetJsonObject("param");
    EXPECT_EQ(string("abc"), param.GetJsonArray("param1").GetString(0));
    EXPECT_EQ(string("123"), param.GetJsonArray("param2").GetString(0));

    EXPECT_TRUE(json.GetString("body").empty());
}

// カスタムAPI呼び出し(GET)
// subpathなし、ヘッダ空、リクエストパラメータ空、Bodyなし、Content-Typeなし
TEST_F(NbApiGatewayFT, ExecuteCustomApiGet2) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kEmpty);

    apigw.AddHeader("user-agent", "");
    apigw.AddHeader("", "test-agent");
    EXPECT_TRUE(apigw.GetHeaders().empty());
    apigw.AddParameter("param1", "");
    apigw.AddParameter("", "123");
    EXPECT_TRUE(apigw.GetParameters().empty());
    EXPECT_TRUE(apigw.GetContentType().empty());

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();

    EXPECT_EQ(200, response.GetStatusCode());

    std::multimap<std::string, std::string> headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "User-Agent", "baas embedded sdk"));

    EXPECT_TRUE(response.GetBody().empty());
}

// カスタムAPI呼び出し(PUT)
// subpathあり、ヘッダあり(User-Agent設定)、リクエストパラメータあり、Bodyあり、Content-Typeあり
TEST_F(NbApiGatewayFT, ExecuteCustomApiPut) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);

    std::vector<char> body{'a','b','c','d','e'};

    apigw.AddHeader("User-Agent", "test-agent");
    apigw.AddHeader("test-header", "test-value");
    apigw.AddParameter("param1", "abc");
    apigw.AddParameter("param2", "123");
    apigw.SetContentType("text/plain");

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(body);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();

    EXPECT_EQ(200, response.GetStatusCode());

    std::multimap<std::string, std::string> headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "User-Agent", "test-agent"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "test-header", "test-value"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "Content-Type", "text/plain"));

    NbJsonObject json(response.GetBody());
    NbJsonObject param = json.GetJsonObject("param");
    EXPECT_TRUE(param.IsEmpty());

    EXPECT_EQ(string(body.begin(), body.end()), json.GetString("body"));
}

// カスタムAPI呼び出し(PUT)
// subpathなし、ヘッダ空、リクエストパラメータ空、Bodyなし、Content-Typeなし
TEST_F(NbApiGatewayFT, ExecuteCustomApiPut2) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kEmpty);

    apigw.AddHeader("user-agent", "");
    apigw.AddHeader("", "test-agent");
    EXPECT_TRUE(apigw.GetHeaders().empty());
    apigw.AddParameter("param1", "");
    apigw.AddParameter("", "123");
    EXPECT_TRUE(apigw.GetParameters().empty());
    EXPECT_TRUE(apigw.GetContentType().empty());

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();

    EXPECT_EQ(200, response.GetStatusCode());

    std::multimap<std::string, std::string> headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "User-Agent", "baas embedded sdk"));

    EXPECT_TRUE(response.GetBody().empty());
}

// カスタムAPI呼び出し(PUT)
// bodyあり、Content-Type空
TEST_F(NbApiGatewayFT, ExecuteCustomApiPutContentTypeNone) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);

    std::vector<char> body{'a','b','c','d','e'};
    EXPECT_TRUE(apigw.GetContentType().empty());

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(body);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONTENT_TYPE, result.GetResultCode());
}

// カスタムAPI呼び出し(POST)
// subpathあり、ヘッダあり(User-Agent設定)、リクエストパラメータあり、Bodyあり、Content-Typeあり
TEST_F(NbApiGatewayFT, ExecuteCustomApiPost) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kSubpath);

    std::vector<char> body{'a','b','c','d','e'};

    apigw.AddHeader("User-Agent", "test-agent");
    apigw.AddHeader("test-header", "test-value");
    apigw.AddParameter("param1", "abc");
    apigw.AddParameter("param2", "123");
    apigw.SetContentType("text/plain");

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(string{"abcde"});

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();

    EXPECT_EQ(200, response.GetStatusCode());

    std::multimap<std::string, std::string> headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "User-Agent", "test-agent"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "test-header", "test-value"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "Content-Type", "text/plain"));

    NbJsonObject json(response.GetBody());
    NbJsonObject param = json.GetJsonObject("param");
    EXPECT_TRUE(param.IsEmpty());

    EXPECT_EQ(string(body.begin(), body.end()), json.GetString("body"));
}

// カスタムAPI呼び出し(POST)
// subpathなし、ヘッダ空、リクエストパラメータ空、Bodyなし、Content-Typeなし
TEST_F(NbApiGatewayFT, ExecuteCustomApiPost2) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kEmpty);

    apigw.AddHeader("user-agent", "");
    apigw.AddHeader("", "test-agent");
    EXPECT_TRUE(apigw.GetHeaders().empty());
    apigw.AddParameter("param1", "");
    apigw.AddParameter("", "123");
    EXPECT_TRUE(apigw.GetParameters().empty());
    EXPECT_TRUE(apigw.GetContentType().empty());

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();

    EXPECT_EQ(200, response.GetStatusCode());

    std::multimap<std::string, std::string> headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "User-Agent", "baas embedded sdk"));

    EXPECT_TRUE(response.GetBody().empty());
}

// カスタムAPI呼び出し(POST)
// bodyあり、Content-Type空
TEST_F(NbApiGatewayFT, ExecuteCustomApiPostContentTypeNone) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kSubpath);

    std::string body{"abcde"};
    EXPECT_TRUE(apigw.GetContentType().empty());

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(body);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONTENT_TYPE, result.GetResultCode());
}

// カスタムAPI呼び出し(DELETE)
// subpathあり、ヘッダあり(User-Agent設定)、リクエストパラメータあり、Bodyあり、Content-Typeあり
TEST_F(NbApiGatewayFT, ExecuteCustomApiDelete) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, kSubpath);

    apigw.AddHeader("user-agent", "test-agent");
    apigw.AddHeader("test-header", "test-value");
    apigw.AddParameter("param1", "abc");
    apigw.AddParameter("param2", "123");
    apigw.SetContentType("text/plain");

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(string{"abcde"});

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();

    EXPECT_EQ(200, response.GetStatusCode());

    std::multimap<std::string, std::string> headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "User-Agent", "test-agent"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "test-header", "test-value"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "Content-Type", "application/json"));

    NbJsonObject json(response.GetBody());
    NbJsonObject param = json.GetJsonObject("param");
    EXPECT_EQ(string("abc"), param.GetJsonArray("param1").GetString(0));
    EXPECT_EQ(string("123"), param.GetJsonArray("param2").GetString(0));

    EXPECT_TRUE(json.GetString("body").empty());
}

// カスタムAPI呼び出し(DELETE)
// subpathなし、ヘッダ空、リクエストパラメータ空、Bodyなし、Content-Typeなし
TEST_F(NbApiGatewayFT, ExecuteCustomApiDelete2) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, kEmpty);

    apigw.AddHeader("user-agent", "");
    apigw.AddHeader("", "test-agent");
    EXPECT_TRUE(apigw.GetHeaders().empty());
    apigw.AddParameter("param1", "");
    apigw.AddParameter("", "123");
    EXPECT_TRUE(apigw.GetParameters().empty());
    EXPECT_TRUE(apigw.GetContentType().empty());

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();

    EXPECT_EQ(200, response.GetStatusCode());

    std::multimap<std::string, std::string> headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "User-Agent", "baas embedded sdk"));

    EXPECT_TRUE(response.GetBody().empty());
}

// カスタムAPI呼び出し(共通)
// api-name空文字
TEST_F(NbApiGatewayFT, ExecuteCustomApiApiNameEmpty) {
    NbApiGateway apigw(service_, kEmpty, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kSubpath);

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_API_NAME, result.GetResultCode());
}

// カスタムAPI呼び出し(共通)
// パス不正(api-nameに不正文字)
TEST_F(NbApiGatewayFT, ExecuteCustomApiInvalidApiName) {
    NbApiGateway apigw(service_, "invalidApiname", NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kSubpath);

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));
}

// カスタムAPI呼び出し(共通)
// タイムアウト
TEST_F(NbApiGatewayFT, ExecuteCustomApiTimeout) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, "/timeout");

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(504, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));
}

// カスタムAPI呼び出し(共通)
// 299成功
TEST_F(NbApiGatewayFT, ExecuteCustomApi299) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, "/success299");

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();
    EXPECT_EQ(299, response.GetStatusCode());
    EXPECT_EQ(string("success"), NbJsonObject(response.GetBody()).GetString("result"));
}

// カスタムAPI呼び出し(共通)
// 555エラー
TEST_F(NbApiGatewayFT, ExecuteCustomApi555) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, "/error555");

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi();

    // 戻り値確認
    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(555, result.GetRestError().status_code);
    EXPECT_EQ(string("CustomError"), NbJsonObject(result.GetRestError().reason).GetString("reason"));
}

// カスタムAPI呼び出し(繰返し)
TEST_F(NbApiGatewayFT, ExecuteCustomApiRepeat) {
    std::string body_str{"abcde"};
    std::vector<char> body_bin{'v','w','x','y','z'};

    // GET 1回目
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

    // PUT 1回目
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

    // POST 1回目
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

    // DELETE 1回目
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

    // GET 2回目
    get.ClearHeaders();
    get.ClearParameters();
    result = get.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    headres = response.GetHeaders();
    EXPECT_FALSE(FTUtil::SearchHeader(headres, "test-header", "get-value"));
    EXPECT_TRUE(response.GetBody().empty());

    // PUT 2回目
    put.SetContentType("");
    result = put.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    headres = response.GetHeaders();
    EXPECT_FALSE(FTUtil::SearchHeader(headres, "Content-Type", "text/plain"));
    EXPECT_TRUE(response.GetBody().empty());

    // POST 2回目
    post.SetContentType("");
    result = post.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    headres = response.GetHeaders();
    EXPECT_FALSE(FTUtil::SearchHeader(headres, "Content-Type", "text/xml"));
    EXPECT_TRUE(response.GetBody().empty());

    // DELETE 2回目
    del.ClearHeaders();
    del.ClearParameters();
    result = del.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    headres = response.GetHeaders();
    EXPECT_FALSE(FTUtil::SearchHeader(headres, "test-header", "delete-value"));
    EXPECT_TRUE(response.GetBody().empty());

    // GET 3回目
    get.AddHeader("test-header", "get-value2");
    get.AddParameter("param3", "xyz");
    get.AddParameter("param4", "789");
    result = get.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "test-header", "get-value2"));
    json = NbJsonObject(response.GetBody());
    param = json.GetJsonObject("param");
    EXPECT_EQ(string("xyz"), param.GetJsonArray("param3").GetString(0));
    EXPECT_EQ(string("789"), param.GetJsonArray("param4").GetString(0));
    EXPECT_TRUE(json.GetString("body").empty());

    // PUT 3回目
    put.SetContentType("text/xml");
    result = put.ExecuteCustomApi(body_bin);
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    json = NbJsonObject(response.GetBody());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "Content-Type", "text/xml"));
    EXPECT_EQ(string(body_bin.begin(), body_bin.end()), json.GetString("body"));

    // POST 3回目
    post.SetContentType("text/plain");
    result = post.ExecuteCustomApi(body_str);
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    json = NbJsonObject(response.GetBody());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "Content-Type", "text/plain"));
    EXPECT_EQ(body_str, json.GetString("body"));

    // DELETE 3回目
    del.AddHeader("test-header", "delete-value2");
    del.AddParameter("param1", "abc");
    del.AddParameter("param2", "123");
    result = del.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "test-header", "delete-value2"));
    json = NbJsonObject(response.GetBody());
    param = json.GetJsonObject("param");
    EXPECT_EQ(string("abc"), param.GetJsonArray("param1").GetString(0));
    EXPECT_EQ(string("123"), param.GetJsonArray("param2").GetString(0));
    EXPECT_TRUE(json.GetString("body").empty());
}

// カスタムAPI呼び出し(マルチテナント)
TEST_F(NbApiGatewayFT, ExecuteCustomApiMultiTenant) {
    shared_ptr<NbService> service_b = NbService::CreateService(kEndPointUrl, kTenantIdB, kAppIdB, kAppKeyB, kProxyB);    
    std::string body_str{"abcde"};
    std::vector<char> body_bin{'v','w','x','y','z'};

    // テナントA GET
    NbApiGateway get_a(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kSubpath);
    get_a.AddHeader("test-header", "get-value");
    get_a.AddParameter("param1", "abc");
    get_a.AddParameter("param2", "123");
    NbResult<NbHttpResponse> result = get_a.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    std::multimap<std::string, std::string> headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "test-header", "get-value"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "X-Application-Id", kAppId));
    NbJsonObject json(response.GetBody());
    NbJsonObject param = json.GetJsonObject("param");
    EXPECT_EQ(string("abc"), param.GetJsonArray("param1").GetString(0));
    EXPECT_EQ(string("123"), param.GetJsonArray("param2").GetString(0));
    EXPECT_TRUE(json.GetString("body").empty());

    // テナントB GET
    NbApiGateway get_b(service_b, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kSubpath);
    get_b.AddHeader("test-header", "get-value");
    get_b.AddParameter("param1", "abc");
    get_b.AddParameter("param2", "123");
    result = get_b.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "test-header", "get-value"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "X-Application-Id", kAppIdB));
    json = NbJsonObject(response.GetBody());
    param = json.GetJsonObject("param");
    EXPECT_EQ(string("abc"), param.GetJsonArray("param1").GetString(0));
    EXPECT_EQ(string("123"), param.GetJsonArray("param2").GetString(0));
    EXPECT_TRUE(json.GetString("body").empty());

    // テナントA PUT
    NbApiGateway put_a(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);
    put_a.SetContentType("text/plain");
    result = put_a.ExecuteCustomApi(body_str);
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    json = NbJsonObject(response.GetBody());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "Content-Type", "text/plain"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "X-Application-Id", kAppId));
    EXPECT_EQ(body_str, json.GetString("body"));

    // テナントB PUT
    NbApiGateway put_b(service_b, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);
    put_b.SetContentType("text/plain");
    result = put_b.ExecuteCustomApi(body_str);
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    json = NbJsonObject(response.GetBody());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "Content-Type", "text/plain"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "X-Application-Id", kAppIdB));
    EXPECT_EQ(body_str, json.GetString("body"));


    // テナントA POST
    NbApiGateway post_a(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kSubpath);
    post_a.SetContentType("text/xml");
    result = post_a.ExecuteCustomApi(body_bin);
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    json = NbJsonObject(response.GetBody());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "Content-Type", "text/xml"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "X-Application-Id", kAppId));
    EXPECT_EQ(string(body_bin.begin(), body_bin.end()), json.GetString("body"));

    // テナントB POST
    NbApiGateway post_b(service_b, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kSubpath);
    post_b.SetContentType("text/xml");
    result = post_b.ExecuteCustomApi(body_bin);
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    json = NbJsonObject(response.GetBody());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "Content-Type", "text/xml"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "X-Application-Id", kAppIdB));
    EXPECT_EQ(string(body_bin.begin(), body_bin.end()), json.GetString("body"));

    // テナントA DELETE
    NbApiGateway del_a(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, kSubpath);
    del_a.AddHeader("test-header", "delete-value");
    del_a.AddParameter("param3", "xyz");
    del_a.AddParameter("param4", "789");
    result = del_a.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "test-header", "delete-value"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "X-Application-Id", kAppId));
    json = NbJsonObject(response.GetBody());
    param = json.GetJsonObject("param");
    EXPECT_EQ(string("xyz"), param.GetJsonArray("param3").GetString(0));
    EXPECT_EQ(string("789"), param.GetJsonArray("param4").GetString(0));
    EXPECT_TRUE(json.GetString("body").empty());

    // テナントB DELETE
    NbApiGateway del_b(service_b, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, kSubpath);
    del_b.AddHeader("test-header", "delete-value");
    del_b.AddParameter("param3", "xyz");
    del_b.AddParameter("param4", "789");
    result = del_b.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(200, response.GetStatusCode());
    headres = response.GetHeaders();
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "test-header", "delete-value"));
    EXPECT_TRUE(FTUtil::SearchHeader(headres, "X-Application-Id", kAppIdB));
    json = NbJsonObject(response.GetBody());
    param = json.GetJsonObject("param");
    EXPECT_EQ(string("xyz"), param.GetJsonArray("param3").GetString(0));
    EXPECT_EQ(string("789"), param.GetJsonArray("param4").GetString(0));
    EXPECT_TRUE(json.GetString("body").empty());
}

// カスタムAPI呼び出し(性能)
// カスタム REST API 応答時間 (エコーバック) 100ミリ以下であること
TEST_F(NbApiGatewayFT, ExecuteCustomApiPerformance) {
    // dockerを考慮して一回実行する
    NbApiGateway get(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kSubpath);
    NbResult<NbHttpResponse> result = get.ExecuteCustomApi();
    ASSERT_TRUE(result.IsSuccess());

    auto start = std::chrono::system_clock::now();
    result = get.ExecuteCustomApi();
    auto end = std::chrono::system_clock::now();

    ASSERT_TRUE(result.IsSuccess());
    EXPECT_GE(100, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}

} //namespace necbaas
