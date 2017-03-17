#include "gtest/gtest.h"
#include "necbaas/internal/nb_http_request_factory.h"

namespace necbaas {

using std::string;

const string kEndPointUrl{"endPointUrl"};
const string kTenantId{"tenantID"};
const string kAppId{"applicationId"};
const string kAppKey{"applicationKey"};
const string kSessionToken{"sessionToken"};
const string kProxy{"proxyUrl"};
const string kPath{"apiPath"};
const string kBody{"body"};

const string kEmpty{""};

string MakeUrl(const string &end_point_url, const string &tenant_id, const string &path, const string &url_params) {
    return end_point_url + "1/" + tenant_id + path + url_params;
}

std::multimap<std::string, std::string> GetHeaders(const std::list<string> &header_list) {
    //nb_http_handler.ccよりコピー
    std::multimap<std::string, std::string> headers{};
    for (auto header_line : header_list) {
        auto index = header_line.find_first_of(':');
        if (index != string::npos) {
            // assume normal header
            // key
            string header_key = header_line.substr(0, index);
            // value
            string header_value = header_line.substr(index + 1);
            // rmove \r\n at the tail
            //header_value = header_value.substr(0, header_value.length() - 2);
            // 前方スペースを削除
            string::size_type not_space = header_value.find_first_not_of(' ');
            if (not_space == string::npos) {
                header_value.clear();
            } else if (not_space > 0) {
                header_value = header_value.substr(not_space);
            }

            headers.insert(std::make_pair(header_key, header_value));
        }
    }
    return headers;
}

//NbHttpRequestFactory Get
//セッショントークン：あり
//Proxy：あり
//HTTP メソッド：GET
//API Path    ： なし
//API Path追加： なし
//リクエストパラメータ：なし
//リクエストパラメータ追加：なし
//ヘッダ：なし
//ヘッダ追加：なし
//ボディ：なし
TEST(NbHttpRequestFactory, Get) {
    NbHttpRequestFactory factory(kEndPointUrl, kTenantId, kAppId, kAppKey, kSessionToken, kProxy);

    EXPECT_FALSE(factory.IsError());
    
    NbHttpRequest request = factory.Get(kEmpty).Build();

    EXPECT_EQ(MakeUrl(kEndPointUrl + "/", kTenantId, kEmpty, kEmpty), request.GetUrl());
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    std::multimap<std::string, std::string> headers = GetHeaders(request.GetHeaders());
    EXPECT_EQ(4, headers.size());
    EXPECT_EQ(kAppId, headers.find("X-Application-Id")->second);
    EXPECT_EQ(kAppKey, headers.find("X-Application-Key")->second);
    EXPECT_EQ(kSessionToken, headers.find("X-Session-Token")->second);
    EXPECT_EQ("baas embedded sdk", headers.find("User-Agent")->second);
    EXPECT_TRUE(request.GetBody().empty());
    EXPECT_EQ(kProxy, request.GetProxy());
}

//NbHttpRequestFactory Put
//セッショントークン：なし
//Proxy：なし
//HTTP メソッド：PUT
//API Path    ： あり（１）
//API Path追加： なし
//リクエストパラメータ：あり（１）
//リクエストパラメータ追加：なし
//ヘッダ：あり（１）
//ヘッダ追加：なし
//ボディ：あり
TEST(NbHttpRequestFactory, Put) {
    NbHttpRequestFactory factory(kEndPointUrl, kTenantId, kAppId, kAppKey, kEmpty, kEmpty);

    EXPECT_FALSE(factory.IsError());

    std::multimap<std::string, std::string> params;
    params.insert(std::make_pair("where", "12345"));

    std::multimap<std::string, std::string> headers;
    headers.insert(std::make_pair("X-HEADER-KEY", "abcdef"));

    NbHttpRequest request = factory.Put(kPath)
                                   .Params(params)
                                   .Headers(headers)
                                   .Body(kBody)
                                   .Build();

    EXPECT_EQ(MakeUrl(kEndPointUrl + "/", kTenantId, kPath, string("?where=12345")), request.GetUrl());
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    headers = GetHeaders(request.GetHeaders());
    EXPECT_EQ(4, headers.size());
    EXPECT_EQ(kAppId, headers.find("X-Application-Id")->second);
    EXPECT_EQ(kAppKey, headers.find("X-Application-Key")->second);
    EXPECT_EQ(0, headers.count("X-Session-Token"));
    EXPECT_EQ("baas embedded sdk", headers.find("User-Agent")->second);
    EXPECT_EQ("abcdef", headers.find("X-HEADER-KEY")->second);
    EXPECT_EQ(kBody, request.GetBody());
    EXPECT_EQ(kEmpty, request.GetProxy());
}

//NbHttpRequestFactory Post
//セッショントークン：ありだがSessionNone設定
//Proxy：なし
//HTTP メソッド：POST
//API Path    ： なし
//API Path追加： あり
//リクエストパラメータ：なし
//リクエストパラメータ追加：あり
//ヘッダ：なし
//ヘッダ追加：あり
//ボディ：あり
TEST(NbHttpRequestFactory, Post) {
    NbHttpRequestFactory factory(kEndPointUrl + "/", kTenantId, kAppId, kAppKey, kSessionToken, kEmpty);

    EXPECT_FALSE(factory.IsError());

    NbHttpRequest request = factory.Post(kEmpty)
                                   .AppendPath(kPath)
                                   .AppendParam("where", "12345")
                                   .AppendHeader("X-HEADER-KEY", "abcdef")
                                   .Body(kBody)
                                   .SessionNone() 
                                   .Build();

    EXPECT_EQ(MakeUrl(kEndPointUrl + "/", kTenantId, kPath, string("?where=12345")), request.GetUrl());
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    std::multimap<std::string, std::string> headers = GetHeaders(request.GetHeaders());
    EXPECT_EQ(4, headers.size());
    EXPECT_EQ(kAppId, headers.find("X-Application-Id")->second);
    EXPECT_EQ(kAppKey, headers.find("X-Application-Key")->second);
    EXPECT_EQ(0, headers.count("X-Session-Token"));
    EXPECT_EQ("baas embedded sdk", headers.find("User-Agent")->second);
    EXPECT_EQ("abcdef", headers.find("X-HEADER-KEY")->second);
    EXPECT_EQ(kBody, request.GetBody());
    EXPECT_EQ(kEmpty, request.GetProxy());
}

//NbHttpRequestFactory Delete
//セッショントークン：あり
//Proxy：なし
//HTTP メソッド：DELETE
//API Path    ： あり
//API Path追加： あり
//リクエストパラメータ：あり（２つ）
//リクエストパラメータ追加：あり（Key重複）
//ヘッダ：あり（２つ）
//ヘッダ追加：あり（Key重複）
//ボディ：あり
TEST(NbHttpRequestFactory, Delete) {
    NbHttpRequestFactory factory(kEndPointUrl, kTenantId, kAppId, kAppKey, kSessionToken, kEmpty);

    EXPECT_FALSE(factory.IsError());

    std::multimap<std::string, std::string> params;
    params.insert(std::make_pair("where", "12345"));
    params.insert(std::make_pair("order", "alpha"));

    std::multimap<std::string, std::string> headers;
    headers.insert(std::make_pair("X-HEADER-KEY", "abcdef"));
    headers.insert(std::make_pair("X-HEADER-KEY2", "key-2"));

    NbHttpRequest request = factory.Post(kPath)
                                   .AppendPath("appendPath")
                                   .Params(params)
                                   .AppendParam("where", "abc")
                                   .Headers(headers)
                                   .AppendHeader("X-HEADER-KEY2", "key-2-2")
                                   .Body(kBody)
                                   .Build();

    EXPECT_EQ(MakeUrl(kEndPointUrl + "/", kTenantId, kPath + "appendPath", string("?order=alpha&where=12345&where=abc")), request.GetUrl());
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    headers = GetHeaders(request.GetHeaders());
    EXPECT_EQ(7, headers.size());
    EXPECT_EQ(kAppId, headers.find("X-Application-Id")->second);
    EXPECT_EQ(kAppKey, headers.find("X-Application-Key")->second);
    EXPECT_EQ(kSessionToken, headers.find("X-Session-Token")->second);
    EXPECT_EQ("baas embedded sdk", headers.find("User-Agent")->second);
    EXPECT_EQ("abcdef", headers.find("X-HEADER-KEY")->second);
    auto range = headers.equal_range("X-HEADER-KEY2");
    auto iterator = range.first;
    EXPECT_EQ("key-2", iterator->second);
    ++iterator;
    EXPECT_EQ("key-2-2", iterator->second);
    EXPECT_EQ(kBody, request.GetBody());
    EXPECT_EQ(kEmpty, request.GetProxy());
}

//NbHttpRequestFactory
//リクエストパラメータ：あり（Key or Valueが空文字）
//リクエストパラメータ追加：あり（Key or Valueが空文字）
//ヘッダ：あり（Key or Valueが空文字）
//ヘッダ追加：あり（Key or Valueが空文字）
TEST(NbHttpRequestFactory, EmptyParam) {
    NbHttpRequestFactory factory(kEndPointUrl, kTenantId, kAppId, kAppKey, kSessionToken, kEmpty);

    EXPECT_FALSE(factory.IsError());

    std::multimap<std::string, std::string> params;
    params.insert(std::make_pair("", "12345"));
    params.insert(std::make_pair("order", ""));

    std::multimap<std::string, std::string> headers;
    headers.insert(std::make_pair("", "abcdef"));
    headers.insert(std::make_pair("X-HEADER-KEY2", ""));

    NbHttpRequest request = factory.Post(kPath)
                                   .Params(params)
                                   .AppendParam("where", "")
                                   .AppendParam("", "abc")
                                   .Headers(headers)
                                   .AppendHeader("X-HEADER-KEY2", "")
                                   .AppendHeader("", "key-2-2")
                                   .Body(kBody)
                                   .Build();

    EXPECT_EQ(MakeUrl(kEndPointUrl + "/", kTenantId, kPath, string("")), request.GetUrl());
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    headers = GetHeaders(request.GetHeaders());
    EXPECT_EQ(4, headers.size());
    EXPECT_EQ(kAppId, headers.find("X-Application-Id")->second);
    EXPECT_EQ(kAppKey, headers.find("X-Application-Key")->second);
    EXPECT_EQ(kSessionToken, headers.find("X-Session-Token")->second);
    EXPECT_EQ("baas embedded sdk", headers.find("User-Agent")->second);
    EXPECT_EQ(kBody, request.GetBody());
    EXPECT_EQ(kEmpty, request.GetProxy());
}

//NbHttpRequestFactory User-Agent ヘッダあり
TEST(NbHttpRequestFactory, UserAgent) {
    NbHttpRequestFactory factory(kEndPointUrl, kTenantId, kAppId, kAppKey, kEmpty, kEmpty);

    EXPECT_FALSE(factory.IsError());

    std::multimap<std::string, std::string> params;
    params.insert(std::make_pair("where", "12345"));

    std::multimap<std::string, std::string> headers;
    headers.insert(std::make_pair("User-Agent", "test user agent"));

    NbHttpRequest request = factory.Put(kPath)
                                   .Params(params)
                                   .Headers(headers)
                                   .Body(kBody)
                                   .Build();

    EXPECT_EQ(MakeUrl(kEndPointUrl + "/", kTenantId, kPath, string("?where=12345")), request.GetUrl());
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    headers = GetHeaders(request.GetHeaders());
    EXPECT_EQ(3, headers.size());
    EXPECT_EQ(kAppId, headers.find("X-Application-Id")->second);
    EXPECT_EQ(kAppKey, headers.find("X-Application-Key")->second);
    EXPECT_EQ(0, headers.count("X-Session-Token"));
    EXPECT_EQ("test user agent", headers.find("User-Agent")->second);
    EXPECT_EQ(kBody, request.GetBody());
    EXPECT_EQ(kEmpty, request.GetProxy());
}

//NbHttpRequestFactory IsError
TEST(NbHttpRequestFactory, IsError) {
    NbHttpRequestFactory *factory = new NbHttpRequestFactory(kEmpty, kTenantId, kAppId, kAppKey, kSessionToken, kProxy);
    EXPECT_TRUE(factory->IsError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, factory->GetError());
    delete factory;

    factory = new NbHttpRequestFactory(kEndPointUrl, kEmpty, kAppId, kAppKey, kSessionToken, kProxy);
    EXPECT_TRUE(factory->IsError());
    EXPECT_EQ(NbResultCode::NB_ERROR_TENANT_ID, factory->GetError());
    delete factory;

    factory = new NbHttpRequestFactory(kEndPointUrl, kTenantId, kEmpty, kAppKey, kSessionToken, kProxy);
    EXPECT_TRUE(factory->IsError());
    EXPECT_EQ(NbResultCode::NB_ERROR_APP_ID, factory->GetError());
    delete factory;

    factory = new NbHttpRequestFactory(kEndPointUrl, kTenantId, kAppId, kEmpty, kSessionToken, kProxy);
    EXPECT_TRUE(factory->IsError());
    EXPECT_EQ(NbResultCode::NB_ERROR_APP_KEY, factory->GetError());
    delete factory;
}

} //namespace necbaas
