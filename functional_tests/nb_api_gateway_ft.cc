#include "gtest/gtest.h"
#include "ft_data.h"
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
    }
    static void TearDownTestCase() {}

    virtual void SetUp() {}
    virtual void TearDown() {}

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
};

shared_ptr<NbService> NbApiGatewayFT::service_;

// key,valに一致するヘッダを検索する（keyの大文字小文字は問わない）
static bool SearchHeader(const std::multimap<std::string, std::string> &map,
                    const string &key, const string &value) { 
    for (auto search : map) {
        if (NbUtility::CompareCaseInsensitiveString(search.first, key) &&
           (value == search.second)) {
            return true;
        }
    }
    return false;
}

// カスタムAPI呼び出し(GET)
// subpathあり、ヘッダあり(User-Agent設定)、リクエストパラメータあり、Bodyあり、Content-Typeあり
TEST_F(NbApiGatewayFT, ExecuteCustomApiGet) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kSubpath);

    std::vector<char> body{'a','b','c','d','e'};

    apigw.AddHeader("user-agent", "test-agent");
    apigw.AddParameter("param1", "abc");
    apigw.AddParameter("param2", "123");
    apigw.SetContentType("text/plain");

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(body);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();

    EXPECT_EQ(200, response.GetStatusCode());

    std::multimap<std::string, std::string> headres = response.GetHeaders();
    EXPECT_TRUE(SearchHeader(headres, "User-Agent", "test-agent"));
    EXPECT_TRUE(SearchHeader(headres, "Content-Type", "application/json"));

    NbJsonObject json(response.GetBody());
    NbJsonObject param = json.GetJsonObject("param");
    EXPECT_EQ(string("abc"), param.GetJsonArray("param1").GetString(0));
    EXPECT_EQ(string("123"), param.GetJsonArray("param2").GetString(0));

    EXPECT_TRUE(json.GetString("body").empty());
}

// カスタムAPI呼び出し(PUT)
// subpathあり、ヘッダあり(User-Agent設定)、リクエストパラメータあり、Bodyあり、Content-Typeあり
TEST_F(NbApiGatewayFT, ExecuteCustomApiPut) {
    NbApiGateway apigw(service_, kApiName, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kSubpath);

    std::vector<char> body{'a','b','c','d','e'};

    apigw.AddHeader("User-Agent", "test-agent");
    apigw.AddParameter("param1", "abc");
    apigw.AddParameter("param2", "123");
    apigw.SetContentType("text/plain");

    NbResult<NbHttpResponse> result = apigw.ExecuteCustomApi(body);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbHttpResponse response = result.GetSuccessData();

    EXPECT_EQ(200, response.GetStatusCode());

    std::multimap<std::string, std::string> headres = response.GetHeaders();
    EXPECT_TRUE(SearchHeader(headres, "User-Agent", "test-agent"));
    EXPECT_TRUE(SearchHeader(headres, "Content-Type", "text/plain"));

    NbJsonObject json(response.GetBody());
    NbJsonObject param = json.GetJsonObject("param");
    EXPECT_TRUE(param.IsEmpty());

    EXPECT_EQ(string(body.begin(), body.end()), json.GetString("body"));
}

} //namespace necbaas
