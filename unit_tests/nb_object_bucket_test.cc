#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <curlpp/cURLpp.hpp>
#include "necbaas/nb_object_bucket.h"
#include "necbaas/internal/nb_utility.h"
#include "rest_api_mock.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

static const string kEmpty{""};

static const string kBucketName{"bucketName"};

static const string kObjectId{"521c36d4ac521e1ffa000007"};

static const string kDefaultObject{R"({
    "_id": "521c36d4ac521e1ffa000007",
    "ACL": {
        "owner": "ownerId",
        "r": ["g:authenticated"],
        "w": ["g:authenticated"]
    },
    "createdAt": "2013-08-27T05:19:16.000Z",
    "updatedAt": "2013-08-27T05:19:17.000Z",
    "etag": "8c92c97e-01a7-11e4-9598-53792c688d1b",
    "intKey":123,
    "int64Key":12345678901,
    "doubleKey":1234.5678,
    "boolKey":true,
    "stringKey":"stringValue",
    "nullKey":null,
    "objectKey":{"obj1":2222,"obj2":"val","obj3":true},
    "arrayKey":[3333,"abcd",false],
    "_deleted":true
})"};

static const string kQureyResponse{R"(
{
    "results":[
        {
            "_id":"52117490ac521e5637000001",
            "name":"Foo1",
            "score":80,
            "ACL": {"owner":"ownerId","r":["g:authenticated"],"w":["g:authenticated"]},
            "createdAt": "2013-08-27T05:19:16.000Z",
            "updatedAt": "2013-08-27T05:19:16.000Z",
            "etag": "8c92c97e-01a7-11e4-9598-53792c688d1b"
        },
        {
            "_id":"52117490ac521e5637000002",
            "name":"Foo2",
            "score":81,
            "ACL": {"owner":"ownerId","r":["g:authenticated"],"w":["g:authenticated"]},
            "createdAt": "2013-08-27T05:19:16.000Z",
            "updatedAt": "2013-08-27T05:19:16.000Z",
            "etag": "8c92c97e-01a7-11e4-9598-53792c688d1b"
        },
        {
            "_id":"52117490ac521e5637000003",
            "name":"Foo3",
            "score":82,
            "ACL": {"owner":"ownerId","r":["g:authenticated"],"w":["g:authenticated"]},
            "createdAt": "2013-08-27T05:19:16.000Z",
            "updatedAt": "2013-08-27T05:19:16.000Z",
            "etag": "8c92c97e-01a7-11e4-9598-53792c688d1b"
        }
    ],
    "currentTime": "2013-09-01T12:34:56.000Z",
    "count": 3
})"};


// テスト名を変更するため、フィクスチャを継承
class NbObjectBucketTest : public RestApiTest {};

static NbResult<NbHttpResponse> GetObject1(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + kObjectId),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_TRUE(request.GetBody().empty());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    string body_str = NbJsonObject(kDefaultObject).ToJsonString();
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObjectBuckt::GetObject(delete_markなし)
TEST_F(NbObjectBucketTest, GetObject1) {
    SetExpect(&executor_, &GetObject1);

    shared_ptr<NbService> service(mock_service_);

    NbObjectBucket object_bucket(service, kBucketName);
    EXPECT_EQ(kBucketName, object_bucket.GetBucketName());
    NbResult<NbObject> result = object_bucket.GetObject(kObjectId);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();
    EXPECT_EQ(123, response.GetInt("intKey"));
    EXPECT_EQ("stringValue", response.GetString("stringKey"));
}

static NbResult<NbHttpResponse> GetObject2(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + kObjectId + "?deleteMark=1"),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_TRUE(request.GetBody().empty());
    EXPECT_EQ(10, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    string body_str = NbJsonObject(kDefaultObject).ToJsonString();
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObjectBuckt::GetObject(delete_markあり)
TEST_F(NbObjectBucketTest, GetObject2) {
    SetExpect(&executor_, &GetObject2);

    shared_ptr<NbService> service(mock_service_);

    NbObjectBucket object_bucket(service, kBucketName);
    object_bucket.SetTimeout(10);
    EXPECT_EQ(10, object_bucket.GetTimeout());
    NbResult<NbObject> result = object_bucket.GetObject(kObjectId, true);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();
    EXPECT_EQ(123, response.GetInt("intKey"));
    EXPECT_EQ("stringValue", response.GetString("stringKey"));
}

static NbResult<NbHttpResponse> GetObjectRestError(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + kObjectId),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_TRUE(request.GetBody().empty());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_ERROR_RESPONSE);
    string body_str{"Http Response Error"};

    NbRestError response{404, body_str};
    tmp_result.SetRestError(response);
    return tmp_result;
}

//NbObjectBuckt::GetObject(RESTエラー)
TEST_F(NbObjectBucketTest, GetObjectRestError) {
    SetExpect(&executor_, &GetObjectRestError);

    shared_ptr<NbService> service(mock_service_);

    NbObjectBucket object_bucket(service, kBucketName);
    NbResult<NbObject> result = object_bucket.GetObject(kObjectId);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_EQ(string("Http Response Error"), result.GetRestError().reason);
}

static NbResult<NbHttpResponse> GetObjectFatal(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + kObjectId),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_TRUE(request.GetBody().empty());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);
    return tmp_result;
}

//NbObjectBuckt::GetObject(エラー)
TEST_F(NbObjectBucketTest, GetObjectFatal) {
    SetExpect(&executor_, &GetObjectFatal);

    shared_ptr<NbService> service(mock_service_);

    NbObjectBucket object_bucket(service, kBucketName);
    NbResult<NbObject> result = object_bucket.GetObject(kObjectId);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
}

//NbObjectBucket::GetObject(ObjectId空)
TEST_F(NbObjectBucketTest, GetObjectIdEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbObjectBucket object_bucket(service, kBucketName);
    NbResult<NbObject> result = object_bucket.GetObject(kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

//NbObjectBucket::GetObject(バケット名空)
TEST_F(NbObjectBucketTest, GetObjectBucketNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbObjectBucket object_bucket(service, kEmpty);
    NbResult<NbObject> result = object_bucket.GetObject(kObjectId);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

//NbObjectBucket::GetObject(request_factory構築失敗)
TEST_F(NbObjectBucketTest, GetObjectRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbObjectBucket object_bucket(service, kBucketName);
    NbResult<NbObject> result = object_bucket.GetObject(kObjectId);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
}

//NbObjectBucket::GetObject(接続数オーバー)
TEST_F(NbObjectBucketTest, GetObjectConnectionOver) {
    SetExpect(nullptr, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbObjectBucket object_bucket(service, kBucketName);
    NbResult<NbObject> result = object_bucket.GetObject(kObjectId);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
}

static NbResult<NbHttpResponse> Query1(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_TRUE(request.GetBody().empty());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    string body_str = {R"({"results": [], "currentTime":"2013-09-01T12:34:56.000Z"})"};
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObjectBuckt::Query(queryなし、ヒットなし、countなし)
TEST_F(NbObjectBucketTest, Query1) {
    SetExpect(&executor_, &Query1);

    shared_ptr<NbService> service(mock_service_);

    NbObjectBucket object_bucket(service, kBucketName);
    NbQuery query;
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, nullptr);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_TRUE(response.empty());
}

static NbResult<NbHttpResponse> Query2(const NbHttpRequest &request, int timeout) {
    string query_str = "?count=1&where=" + curlpp::escape(R"({"key1":"abc","key2":{"$gt":123}})");
    EXPECT_EQ(string("/objects/" + kBucketName + query_str),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_TRUE(request.GetBody().empty());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    string body_str = kQureyResponse;
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObjectBuckt::Query(queryあり、ヒットあり、countあり)
TEST_F(NbObjectBucketTest, Query2) {
    SetExpect(&executor_, &Query2);

    shared_ptr<NbService> service(mock_service_);

    NbObjectBucket object_bucket(service, kBucketName);
    NbQuery query;
    query.EqualTo(string("key1"),string("abc")).GreaterThan(string("key2"),123);
    int count;
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(3, count);
    EXPECT_EQ(3, response.size());
    EXPECT_EQ("Foo1", response[0].GetString("name"));
    EXPECT_EQ("Foo2", response[1].GetString("name"));
    EXPECT_EQ("Foo3", response[2].GetString("name"));
    EXPECT_EQ(80, response[0].GetInt("score"));
    EXPECT_EQ(81, response[1].GetInt("score"));
    EXPECT_EQ(82, response[2].GetInt("score"));
}

static NbResult<NbHttpResponse> QueryRestError(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_TRUE(request.GetBody().empty());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_ERROR_RESPONSE);
    string body_str{"Http Response Error"};

    NbRestError response{404, body_str};
    tmp_result.SetRestError(response);
    return tmp_result;
}

//NbObjectBuckt::Query(RESTエラー)
TEST_F(NbObjectBucketTest, QueryRestError) {
    SetExpect(&executor_, &QueryRestError);

    shared_ptr<NbService> service(mock_service_);

    NbObjectBucket object_bucket(service, kBucketName);
    NbQuery query;
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, nullptr);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_EQ(string("Http Response Error"), result.GetRestError().reason);
}

static NbResult<NbHttpResponse> QueryFatal(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_TRUE(request.GetBody().empty());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);
    return tmp_result;
}

//NbObjectBuckt::Query(RESTエラー)
TEST_F(NbObjectBucketTest, QueryFatal) {
    SetExpect(&executor_, &QueryFatal);

    shared_ptr<NbService> service(mock_service_);

    NbObjectBucket object_bucket(service, kBucketName);
    NbQuery query;
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, nullptr);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
}

//NbObjectBucket::Query(バケット名空)
TEST_F(NbObjectBucketTest, QueryBucketNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbObjectBucket object_bucket(service, kEmpty);
    NbQuery query;
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, nullptr);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

//NbObjectBucket::Query(request_factory構築失敗)
TEST_F(NbObjectBucketTest, QueryRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbObjectBucket object_bucket(service, kBucketName);
    NbQuery query;
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, nullptr);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
}

//NbObjectBucket::Query(接続数オーバー)
TEST_F(NbObjectBucketTest, QueryConnectionOver) {
    SetExpect(nullptr, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbObjectBucket object_bucket(service, kBucketName);
    NbQuery query;
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, nullptr);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
}

static NbResult<NbHttpResponse> GetParams(const NbHttpRequest &request, int timeout) {
    string query_str = curlpp::unescape(request.GetUrl().substr(request.GetUrl().find("?") + 1));

    std::stringstream stream(query_str);
    std::string buffer;
    std::map<string, string> query_map;
    while(std::getline(stream, buffer, '&')) {
        string key = buffer.substr(0, buffer.find("="));
        string val = buffer.substr(buffer.find("=") + 1);
        query_map[key] = val;
    }

    EXPECT_EQ(string(R"({"key1":"abc"})"), query_map["where"]);
    EXPECT_EQ(string("name"), query_map["order"]);
    EXPECT_EQ(string("10"), query_map["skip"]);
    EXPECT_EQ(string("20"), query_map["limit"]);
    EXPECT_EQ(string("1"), query_map["count"]);
    EXPECT_EQ(string("1"), query_map["deleteMark"]);
    EXPECT_EQ(string(R"({"score":1})"), query_map["projection"]);
    EXPECT_EQ(string("secondaryPreferred"), query_map["readPreference"]);
    EXPECT_EQ(string("30"), query_map["timeout"]);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);
    return tmp_result;
}

//NbObjectBuckt::GetParams
TEST_F(NbObjectBucketTest, GetParams) {
    SetExpect(&executor_, &GetParams);

    shared_ptr<NbService> service(mock_service_);

    NbObjectBucket object_bucket(service, kBucketName);
    NbQuery query;
    query.EqualTo(string("key1"),string("abc"))
         .OrderBy(std::vector<std::string>{"name"})
         .Skip(10)
         .Limit(20)
         .DeleteMark(true)
         .Projection(std::map<std::string, bool>{{"score",true}})
         .ReadPreference(NbReadPreference::SECONDARY_PREFERRED)
         .Timeout(30);
         
    int count;
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);
}

//NbObjectBuckt::NewObject
TEST_F(NbObjectBucketTest, NewObject) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbObjectBucket object_bucket(service, kBucketName);
    NbObject object = object_bucket.NewObject();
    EXPECT_EQ(kBucketName, object.GetBucketName());
    EXPECT_TRUE(object.IsEmpty());
}
} // namespace necbaas
