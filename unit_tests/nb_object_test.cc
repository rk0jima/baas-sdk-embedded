#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "necbaas/nb_object.h"
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

static const string kAcl{R"({"owner":"ownerId","r":["g:authenticated"],"w":["g:authenticated"]})"};

static const string kPartUpdateObject{R"({
    "intKey":987,
    "boolKey":false
})"};

static const string kSaveNew{R"({
    "intKey":123,
    "int64Key":12345678901,
    "doubleKey":1234.5678,
    "boolKey":true,
    "stringKey":"stringValue",
    "nullKey":null,
    "objectKey":{"obj1":2222,"obj2":"val","obj3":true},
    "arrayKey":[3333,"abcd",false]
})"};

// テスト名を変更するため、フィクスチャを継承
class NbObjectTest : public RestApiTest {};

static NbResult<NbHttpResponse> PartUpdateObject1(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + "521c36d4ac521e1ffa000007" + "?etag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("Content-Type: application/json") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(1, check_header);
    EXPECT_EQ(NbJsonObject(kPartUpdateObject).ToJsonString(), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbJsonObject resp_json(kDefaultObject);
    NbJsonObject update_json(kPartUpdateObject);
    for (auto key : update_json.GetKeySet()) {
        resp_json[key] = update_json[key];
    }
    string body_str = resp_json.ToJsonString();
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObject::PartUpdateObject(予約フィールドなし,Etagあり)
TEST_F(NbObjectTest, PartUpdateObject1) {
    SetExpect(&executor_, &PartUpdateObject1);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    EXPECT_EQ(kBucketName, object.GetBucketName());

    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbJsonObject json(kPartUpdateObject);
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();
    EXPECT_EQ(987, response.GetInt("intKey"));
    EXPECT_EQ("stringValue", response.GetString("stringKey"));
    EXPECT_EQ(987, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
}

static NbResult<NbHttpResponse> PartUpdateObject2(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + "521c36d4ac521e1ffa000007"),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("Content-Type: application/json") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(1, check_header);
    NbJsonObject part_obj(kPartUpdateObject);
    part_obj["_id"] = "ObjectId";
    part_obj.PutJsonObject("ACL", NbAcl::CreateAclForAnonymous().ToJsonObject());
    part_obj["updatedAt"] = "2015-08-27T05:19:17.000Z";
    part_obj["createdAt"] = "2014-08-27T05:19:17.000Z";
    part_obj["etag"] = "etagValue";
    part_obj["_deleted"] = true;
    EXPECT_EQ(part_obj.ToJsonString(), request.GetBody());
    EXPECT_EQ(10, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbJsonObject resp_json(kDefaultObject);
    NbJsonObject update_json(kPartUpdateObject);
    for (auto key : update_json.GetKeySet()) {
        resp_json[key] = update_json[key];
    }
    resp_json["_id"] = "ObjectId";
    resp_json.PutJsonObject("ACL", NbAcl::CreateAclForAnonymous().ToJsonObject());
    resp_json["updatedAt"] = "2015-08-27T05:19:17.000Z";
    resp_json["createdAt"] = "2014-08-27T05:19:17.000Z";
    resp_json["etag"] = "etagValue";
    resp_json["_deleted"] = true;
    string body_str = resp_json.ToJsonString();
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObject::PartUpdateObject(予約フィールドあり,Etagなし)
TEST_F(NbObjectTest, PartUpdateObject2) {
    SetExpect(&executor_, &PartUpdateObject2);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetTimeout(10);
    EXPECT_EQ(10, object.GetTimeout());
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    object.SetETag(kEmpty);
    NbJsonObject json(kPartUpdateObject);
    json["_id"] = "ObjectId";
    json.PutJsonObject("ACL", NbAcl::CreateAclForAnonymous().ToJsonObject());
    json["updatedAt"] = "2015-08-27T05:19:17.000Z";
    json["createdAt"] = "2014-08-27T05:19:17.000Z";
    json["etag"] = "etagValue";
    json["_deleted"] = true;
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();
    EXPECT_EQ(987, response.GetInt("intKey"));
    EXPECT_EQ("stringValue", response.GetString("stringKey"));
    EXPECT_FALSE(response.IsMember("_id"));
    EXPECT_FALSE(response.IsMember("ACL"));
    EXPECT_FALSE(response.IsMember("createdAt"));
    EXPECT_FALSE(response.IsMember("updatedAt"));
    EXPECT_FALSE(response.IsMember("etag"));
    EXPECT_FALSE(response.IsMember("_deleted"));
    EXPECT_EQ(987, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
    EXPECT_EQ("ObjectId", object.GetObjectId());
    EXPECT_EQ(NbAcl::CreateAclForAnonymous().ToJsonString(), object.GetAcl().ToJsonString());
    EXPECT_EQ("2014-08-27T05:19:17.000Z", NbUtility::TmToDateString(object.GetCreatedTime()));
    EXPECT_EQ("2015-08-27T05:19:17.000Z", NbUtility::TmToDateString(object.GetUpdatedTime()));
    EXPECT_EQ("etagValue", object.GetETag());
    EXPECT_TRUE(object.IsDeleteMark());
}

static NbResult<NbHttpResponse> PartUpdateObjectRestError(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + "521c36d4ac521e1ffa000007" + "?etag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("Content-Type: application/json") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(1, check_header);
    EXPECT_EQ(NbJsonObject(kPartUpdateObject).ToJsonString(), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_ERROR_RESPONSE);
    string body_str{"Http Response Error"};

    NbRestError response{404, body_str};
    tmp_result.SetRestError(response);
    return tmp_result;
}

//NbObject::PartUpdateObject(RESTエラー)
TEST_F(NbObjectTest, PartUpdateObjectestError) {
    SetExpect(&executor_, &PartUpdateObjectRestError);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbJsonObject json(kPartUpdateObject);
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_EQ(string("Http Response Error"), result.GetRestError().reason);

    EXPECT_EQ(123, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
}

static NbResult<NbHttpResponse> PartUpdateObjectFatal(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + "521c36d4ac521e1ffa000007" + "?etag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("Content-Type: application/json") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(1, check_header);
    EXPECT_EQ(NbJsonObject(kPartUpdateObject).ToJsonString(), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);
    return tmp_result;
}

//NbObject::PartUpdateObject(エラー)
TEST_F(NbObjectTest, PartUpdateObjectFatal) {
    SetExpect(&executor_, &PartUpdateObjectFatal);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbJsonObject json(kPartUpdateObject);
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());

    EXPECT_EQ(123, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
}

//NbObject::PartUpdateObject(オブジェクト空)
TEST_F(NbObjectTest, PartUpdateObjectestEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbJsonObject json;
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

//NbObject::PartUpdateObject(バケット名空)
TEST_F(NbObjectTest, PartUpdateObjectBucketNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbObject object(service, kEmpty);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbJsonObject json(kPartUpdateObject);
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

//NbObject::PartUpdateObject(ObjectId空)
TEST_F(NbObjectTest, PartUpdateObjectIdEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    object.SetObjectId(kEmpty);
    NbJsonObject json(kPartUpdateObject);
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_OBJECT_ID, result.GetResultCode());
}

//NbObject::PartUpdateObject(request_factory構築失敗)
TEST_F(NbObjectTest, PartUpdateObjectRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbJsonObject json(kPartUpdateObject);
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
}

//NbObject::PartUpdateObject(接続数オーバー)
TEST_F(NbObjectTest, PartUpdateObjectConnectionOver) {
    SetExpect(nullptr, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbJsonObject json(kPartUpdateObject);
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
}

static NbResult<NbHttpResponse> DeleteObject1(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + "521c36d4ac521e1ffa000007" + "?etag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    string body_str{"{}"};
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObject::DeleteObject(deleteMarkなし、ETagあり)
TEST_F(NbObjectTest, DeleteObject1) {
    SetExpect(&executor_, &DeleteObject1);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbResult<NbObject> result = object.DeleteObject();

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();
    EXPECT_TRUE(response.IsEmpty());
    EXPECT_EQ(123, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
}

static NbResult<NbHttpResponse> DeleteObject2(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + "521c36d4ac521e1ffa000007" + "?deleteMark=1"),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(10, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbJsonObject resp_json(kDefaultObject);
    resp_json["_deleted"] = true;
    string body_str = resp_json.ToJsonString();
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObject::DeleteObject(deleteMarkあり、ETagなし)
TEST_F(NbObjectTest, DeleteObject2) {
    SetExpect(&executor_, &DeleteObject2);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetTimeout(10);
    EXPECT_EQ(10, object.GetTimeout());
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    object.SetETag(kEmpty);
    NbResult<NbObject> result = object.DeleteObject(true);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();
    EXPECT_EQ(123, response.GetInt("intKey"));
    EXPECT_EQ("stringValue", response.GetString("stringKey"));
    EXPECT_TRUE(response.IsDeleteMark());
    EXPECT_EQ(123, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
    EXPECT_TRUE(object.IsDeleteMark());
}

static NbResult<NbHttpResponse> DeleteObjectRestError(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + "521c36d4ac521e1ffa000007" + "?etag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_ERROR_RESPONSE);
    string body_str{"Http Response Error"};

    NbRestError response{404, body_str};
    tmp_result.SetRestError(response);
    return tmp_result;
}

//NbObject::DeleteObject(RESTエラー)
TEST_F(NbObjectTest, DeleteObjectRestError) {
    SetExpect(&executor_, &DeleteObjectRestError);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbResult<NbObject> result = object.DeleteObject();

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_EQ(string("Http Response Error"), result.GetRestError().reason);

    EXPECT_EQ(123, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
}

static NbResult<NbHttpResponse> DeleteObjectFatal(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + "521c36d4ac521e1ffa000007" + "?etag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);
    return tmp_result;
}

//NbObject::DeleteObject(エラー)
TEST_F(NbObjectTest, DeleteObjectFatal) {
    SetExpect(&executor_, &DeleteObjectFatal);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbResult<NbObject> result = object.DeleteObject();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());

    EXPECT_EQ(123, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
}

//NbObject::DeleteObject(バケット名空)
TEST_F(NbObjectTest, DeleteObjectBucketNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbObject object(service, kEmpty);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbResult<NbObject> result = object.DeleteObject();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

//NbObject::DeleteObject(ObjectId空)
TEST_F(NbObjectTest, DeleteObjectIdEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    object.SetObjectId(kEmpty);
    NbResult<NbObject> result = object.DeleteObject();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_OBJECT_ID, result.GetResultCode());
}

//NbObject::DeleteObject(request_factory構築失敗)
TEST_F(NbObjectTest, DeleteObjectRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbResult<NbObject> result = object.DeleteObject();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
}

//NbObject::DeleteObject(接続数オーバー)
TEST_F(NbObjectTest, DeleteObjectConnectionOver) {
    SetExpect(nullptr, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    NbResult<NbObject> result = object.DeleteObject();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
}

static NbResult<NbHttpResponse> SaveNew1(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("Content-Type: application/json") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(1, check_header);
    EXPECT_EQ(NbJsonObject(kSaveNew).ToJsonString(), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbJsonObject resp_json(kDefaultObject);
    string body_str = resp_json.ToJsonString();
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObject::Save(新規、ACLなし)
TEST_F(NbObjectTest, SaveNew1) {
    SetExpect(&executor_, &SaveNew1);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kSaveNew));
    object.SetAcl(NbAcl::CreateAclForAnonymous());
    object["_id"] = "id";
    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();
    EXPECT_EQ(123, response.GetInt("intKey"));
    EXPECT_EQ("stringValue", response.GetString("stringKey"));
    EXPECT_EQ("521c36d4ac521e1ffa000007", response.GetObjectId());
    EXPECT_EQ(123, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
    EXPECT_EQ("521c36d4ac521e1ffa000007", object.GetObjectId());
}

static NbResult<NbHttpResponse> SaveNew2(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("Content-Type: application/json") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(1, check_header);
    NbJsonObject req_json(kSaveNew);
    req_json.PutJsonObject("ACL", NbAcl::CreateAclForAnonymous().ToJsonObject());
    EXPECT_EQ(req_json.ToJsonString(), request.GetBody());
    EXPECT_EQ(10, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbJsonObject resp_json(kDefaultObject);
    string body_str = resp_json.ToJsonString();
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObject::Save(新規、ACLあり)
TEST_F(NbObjectTest, SaveNew2) {
    SetExpect(&executor_, &SaveNew2);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetTimeout(10);
    EXPECT_EQ(10, object.GetTimeout());
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    object.SetObjectId(kEmpty);
    object.SetAcl(NbAcl::CreateAclForAnonymous());
    object["_id"] = "id";
    NbResult<NbObject> result = object.Save(true);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();
    EXPECT_EQ(123, response.GetInt("intKey"));
    EXPECT_EQ("stringValue", response.GetString("stringKey"));
    EXPECT_EQ("521c36d4ac521e1ffa000007", response.GetObjectId());
    EXPECT_EQ(123, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
    EXPECT_EQ("521c36d4ac521e1ffa000007", object.GetObjectId());
}

static NbResult<NbHttpResponse> SaveUpdate1(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + "521c36d4ac521e1ffa000007"),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("Content-Type: application/json") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(1, check_header);
    NbJsonObject req_json;
    NbJsonObject data_json(kSaveNew);
    data_json.PutJsonObject("ACL", NbAcl(NbJsonObject(kAcl)).ToJsonObject());
    req_json.PutJsonObject("$full_update", data_json);
    EXPECT_EQ(req_json.ToJsonString(), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbJsonObject resp_json(kDefaultObject);
    string body_str = resp_json.ToJsonString();
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObject::Save(更新、作成日時なし、Etagなし)
TEST_F(NbObjectTest, SaveUpdate1) {
    SetExpect(&executor_, &SaveUpdate1);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    object.SetCreatedTime(NbUtility::DateStringToTm(kEmpty));
    object.SetETag(kEmpty);
    object["_id"] = "id";
    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();
    EXPECT_EQ(123, response.GetInt("intKey"));
    EXPECT_EQ("stringValue", response.GetString("stringKey"));
    EXPECT_EQ("521c36d4ac521e1ffa000007", response.GetObjectId());
    EXPECT_EQ(123, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
    EXPECT_EQ("521c36d4ac521e1ffa000007", object.GetObjectId());
}

static NbResult<NbHttpResponse> SaveUpdate2(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName + "/" + "521c36d4ac521e1ffa000007" + "?etag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("Content-Type: application/json") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(1, check_header);
    NbJsonObject req_json;
    NbJsonObject data_json(kSaveNew);
    data_json.PutJsonObject("ACL", NbAcl(NbJsonObject(kAcl)).ToJsonObject());
    data_json["createdAt"] = "2013-08-27T05:19:16.000Z";
    req_json.PutJsonObject("$full_update", data_json);
    EXPECT_EQ(req_json.ToJsonString(), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbJsonObject resp_json(kDefaultObject);
    string body_str = resp_json.ToJsonString();
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObject::Save(更新、作成日時あり、Etagあり)
TEST_F(NbObjectTest, SaveUpdate2) {
    SetExpect(&executor_, &SaveUpdate2);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    object["_id"] = "id";
    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();
    EXPECT_EQ(123, response.GetInt("intKey"));
    EXPECT_EQ("stringValue", response.GetString("stringKey"));
    EXPECT_EQ("521c36d4ac521e1ffa000007", response.GetObjectId());
    EXPECT_EQ(123, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
    EXPECT_EQ("521c36d4ac521e1ffa000007", object.GetObjectId());
}

static NbResult<NbHttpResponse> SaveRestError(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    EXPECT_EQ(NbJsonObject(kSaveNew).ToJsonString(), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_ERROR_RESPONSE);
    string body_str{"Http Response Error"};

    NbRestError response{404, body_str};
    tmp_result.SetRestError(response);
    return tmp_result;
}

//NbObject::Save(RESTエラー)
TEST_F(NbObjectTest, SaveRestError) {
    SetExpect(&executor_, &SaveRestError);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kSaveNew));
    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_EQ(string("Http Response Error"), result.GetRestError().reason);

    EXPECT_EQ(123, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
}

static NbResult<NbHttpResponse> SaveFatal(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    EXPECT_EQ(NbJsonObject(kSaveNew).ToJsonString(), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);
    return tmp_result;
}

//NbObject::Save(エラー)
TEST_F(NbObjectTest, SaveFatal) {
    SetExpect(&executor_, &SaveFatal);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kSaveNew));
    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());

    EXPECT_EQ(123, object.GetInt("intKey"));
    EXPECT_EQ("stringValue", object.GetString("stringKey"));
}

//NbObject::Save(バケット名空)
TEST_F(NbObjectTest, SaveBucketNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbObject object(service, kEmpty);
    object.SetCurrentParam(NbJsonObject(kSaveNew));
    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

//NbObject::Save(request_factory構築失敗)
TEST_F(NbObjectTest, SaveRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kSaveNew));
    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
}

//NbObject::Save(接続数オーバー)
TEST_F(NbObjectTest, SaveConnectionOver) {
    SetExpect(nullptr, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kSaveNew));
    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
}

//NbObject::SetCurrentParam
TEST_F(NbObjectTest, SetCurrentParam) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kDefaultObject));
    EXPECT_FALSE(object.IsMember("_id"));
    EXPECT_FALSE(object.IsMember("createdAt"));
    EXPECT_FALSE(object.IsMember("updatedAt"));
    EXPECT_FALSE(object.IsMember("ACL"));
    EXPECT_FALSE(object.IsMember("etag"));
    EXPECT_FALSE(object.IsMember("_deleted"));

    EXPECT_EQ(string("521c36d4ac521e1ffa000007"), object.GetObjectId());
    EXPECT_EQ(string("2013-08-27T05:19:16.000Z"), NbUtility::TmToDateString(object.GetCreatedTime()));
    EXPECT_EQ(string("2013-08-27T05:19:17.000Z"), NbUtility::TmToDateString(object.GetUpdatedTime()));
    EXPECT_EQ(NbAcl(NbJsonObject(kAcl)).ToJsonString(), object.GetAcl().ToJsonString());
    EXPECT_EQ(string("8c92c97e-01a7-11e4-9598-53792c688d1b"), object.GetETag());
    EXPECT_TRUE(object.IsDeleteMark());

    object.SetCurrentParam(NbJsonObject());
    EXPECT_FALSE(object.IsMember("_id"));
    EXPECT_FALSE(object.IsMember("createdAt"));
    EXPECT_FALSE(object.IsMember("updatedAt"));
    EXPECT_FALSE(object.IsMember("ACL"));
    EXPECT_FALSE(object.IsMember("etag"));
    EXPECT_FALSE(object.IsMember("_deleted"));

    EXPECT_TRUE(object.GetObjectId().empty());
    EXPECT_EQ(0, object.GetCreatedTime().tm_year);
    EXPECT_EQ(0, object.GetUpdatedTime().tm_year);
    EXPECT_EQ(NbAcl().ToJsonString(), object.GetAcl().ToJsonString());
    EXPECT_TRUE(object.GetETag().empty());
    EXPECT_FALSE(object.IsDeleteMark());
}

static NbResult<NbHttpResponse> RemoveReservationFields(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/objects/" + kBucketName),
                     request.GetUrl().substr(request.GetUrl().find("/objects/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("Content-Type: application/json") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(1, check_header);
    EXPECT_EQ(NbJsonObject(kSaveNew).ToJsonString(), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), std::vector<char>());
    tmp_result.SetSuccessData(response);
    return tmp_result;
}

//NbObject::RemoveReservationFields
TEST_F(NbObjectTest, RemoveReservationFields) {
    SetExpect(&executor_, &RemoveReservationFields);

    shared_ptr<NbService> service(mock_service_);

    NbObject object(service, kBucketName);
    object.SetCurrentParam(NbJsonObject(kSaveNew));
    object.SetAcl(NbAcl::CreateAclForAnonymous());
    object["_id"] = "id";
    object["createdAt"] = "createdAt";
    object["updatedAt"] = "updatedAt";
    object["ACL"] = "ACL";
    object["etag"] = "etag";
    object["_deleted"] = "_deleted";
    NbResult<NbObject> result = object.Save();
}
} //namespace necbaas
