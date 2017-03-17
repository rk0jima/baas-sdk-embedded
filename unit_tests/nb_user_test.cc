#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "necbaas/nb_user.h"
#include "necbaas/internal/nb_utility.h"
#include "rest_api_mock.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;

static const string kEmpty{""};

static const string kUsername{"userName"};
static const string kEmail{"e-mail@nbaas.com"};
static const string kPassword{"password"};
static const vector<string> kGroups{"group1", "group2", "group3"};
static const string kOptions{R"({"displayName":"taro","division":"business"})"};

static const string kUserJson{R"({
    "_id":"52116f01ac521e1742000001",
    "sessionToken":"sessionToken",
    "expire":1976878239,
    "username": "userName",
    "email": "e-mail@nbaas.com",
    "groups": [ "group1", "group2", "group3" ],
    "options": {
        "displayName": "taro",
        "division": "business"
    },
    "createdAt": "2013-08-27T04:37:30.000Z",
    "updatedAt": "2013-08-27T04:37:30.000Z"
})"};

// テスト名を変更するため、フィクスチャを継承
class NbUserTest : public RestApiTest {};

static NbResult<NbHttpResponse> LoginCheckRequestNorm(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/login"), request.GetUrl().substr(request.GetUrl().rfind('/')));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(string("Content-Type: application/json"), request.GetHeaders().front());
    NbJsonObject json_body(R"({"username":"userName","password":"password"})");
    EXPECT_EQ(json_body.ToJsonString(), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    std::vector<char> body(kUserJson.length());
    std::copy(kUserJson.c_str(), kUserJson.c_str() + kUserJson.length(), body.begin());

    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

//NbUser::LoginWithUsername(正常)
TEST_F(NbUserTest, LoginWithUsername) {
    SetExpect(&executor_, &LoginCheckRequestNorm);

    shared_ptr<NbService> service(mock_service_);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUsername, kPassword, 60);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    EXPECT_EQ(kUsername, user.GetUserName());
    EXPECT_EQ(kEmail, user.GetEmail());
    EXPECT_EQ(kOptions, user.GetOptions().ToJsonString());
    EXPECT_EQ(kGroups, user.GetGroups());
    EXPECT_EQ(string("52116f01ac521e1742000001"), user.GetUserId());
    EXPECT_EQ(string("2013-08-27T04:37:30.000Z"), NbUtility::TmToDateString(user.GetCreatedTime()));
    EXPECT_EQ(string("2013-08-27T04:37:30.000Z"), NbUtility::TmToDateString(user.GetUpdatedTime()));

    EXPECT_TRUE(NbUser::IsLoggedIn(service));
    EXPECT_EQ(string("sessionToken"), NbUser::GetSessionToken(service));
    EXPECT_EQ((std::time_t)1976878239, NbUser::GetSessionTokenExpiration(service));
    NbUser current_user = NbUser::GetCurrentUser(service);
    EXPECT_EQ(kUsername, current_user.GetUserName());    
}

static NbResult<NbHttpResponse> LoginCheckRequestRestError(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/login"), request.GetUrl().substr(request.GetUrl().rfind('/')));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(string("Content-Type: application/json"), request.GetHeaders().front());
    NbJsonObject json_body(R"({"email":"e-mail@nbaas.com","password":"password"})");
    EXPECT_EQ(json_body.ToJsonString(), request.GetBody());
    EXPECT_EQ(10, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_ERROR_RESPONSE);
    string body_str{"Http Response Error"};

    NbRestError response{404, body_str};
    tmp_result.SetRestError(response);

    return tmp_result;
}

//NbUser::LoginWithEmail(RESTエラー)
TEST_F(NbUserTest, LoginWithEmailRestError) {
    SetExpect(&executor_, &LoginCheckRequestRestError);

    shared_ptr<NbService> service(mock_service_);

    NbResult<NbUser> result = NbUser::LoginWithEmail(service, kEmail, kPassword, 10);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_EQ(string("Http Response Error"), result.GetRestError().reason);

    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

static NbResult<NbHttpResponse> LoginCheckRequestFatalError(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/login"), request.GetUrl().substr(request.GetUrl().rfind('/')));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(string("Content-Type: application/json"), request.GetHeaders().front());
    NbJsonObject json_body(R"({"username":"userName","password":"password"})");
    EXPECT_EQ(json_body.ToJsonString(), request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);

    return tmp_result;
}

//NbUser::LoginWithUsername(エラー)
TEST_F(NbUserTest, LoginWithUsernameFatal) {
    SetExpect(&executor_, &LoginCheckRequestFatalError);

    shared_ptr<NbService> service(mock_service_);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUsername, kPassword, 60);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

//NbUser::LoginWithUsername(ユーザ名空)
TEST(NbUser, LoginWithUsernameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kEmpty, kPassword, 60);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

//NbUser::LoginWithEmail(E-mail空)
TEST(NbUser, LoginWithEmailEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbResult<NbUser> result = NbUser::LoginWithEmail(service, kEmpty, kPassword, 60);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

//NbUser::LoginWithUsername(パスワード空)
TEST(NbUser, LoginWithUsernamePasswordEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUsername, kEmpty, 60);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

//NbUser::LoginWithUsername(request_factory構築失敗)
TEST(NbUser, LoginWithUsernameRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUsername, kPassword, 60);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

//NbUser::LoginWithUsername(接続数オーバー)
TEST_F(NbUserTest, LoginWithUsernameConnectionOver) {
    SetExpect(nullptr, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUsername, kPassword, 60);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

static NbResult<NbHttpResponse> LogoutCheckRequestNorm(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/login"), request.GetUrl().substr(request.GetUrl().rfind('/')));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, request.GetMethod());
    EXPECT_EQ(string("X-Session-Token: sessionToken"), request.GetHeaders().back());
    EXPECT_TRUE(request.GetBody().empty());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    string body_str{R"({"_id":"52116f01ac521e1742000001"})"};
    std::vector<char> body(body_str.length());
    std::copy(body_str.c_str(), body_str.c_str() + body_str.length(), body.begin());

    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

//NbUser::Logout(正常)
TEST_F(NbUserTest, Logout) {
    SetExpect(&executor_, &LogoutCheckRequestNorm);

    shared_ptr<NbService> service(mock_service_);

    NbUser::ImportCurrentLogin(service, kUserJson);
    EXPECT_TRUE(NbUser::IsLoggedIn(service));

    NbResult<NbUser> result = NbUser::Logout(service);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    EXPECT_EQ(string("52116f01ac521e1742000001"), user.GetUserId());

    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

static NbResult<NbHttpResponse> LogoutCheckRequestRestError(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/login"), request.GetUrl().substr(request.GetUrl().rfind('/')));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, request.GetMethod());
    EXPECT_EQ(string("X-Session-Token: sessionToken"), request.GetHeaders().back());
    EXPECT_TRUE(request.GetBody().empty());
    EXPECT_EQ(10, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_ERROR_RESPONSE);
    string body_str{"Http Response Error"};

    NbRestError response{401, body_str};
    tmp_result.SetRestError(response);

    return tmp_result;
}

//NbUser::Logout(RESTエラー)
TEST_F(NbUserTest, LogoutRestError) {
    SetExpect(&executor_, &LogoutCheckRequestRestError);

    shared_ptr<NbService> service(mock_service_);

    NbUser::ImportCurrentLogin(service, kUserJson);
    EXPECT_TRUE(NbUser::IsLoggedIn(service));

    NbResult<NbUser> result = NbUser::Logout(service, 10);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(401, result.GetRestError().status_code);
    EXPECT_EQ(string("Http Response Error"), result.GetRestError().reason);

    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

static NbResult<NbHttpResponse> LogoutCheckRequestFatalError(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/login"), request.GetUrl().substr(request.GetUrl().rfind('/')));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, request.GetMethod());
    EXPECT_EQ(string("X-Session-Token: sessionToken"), request.GetHeaders().back());
    EXPECT_TRUE(request.GetBody().empty());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);

    return tmp_result;
}

//NbUser::Logout(エラー)
TEST_F(NbUserTest, LogoutFatal) {
    SetExpect(&executor_, &LogoutCheckRequestFatalError);

    shared_ptr<NbService> service(mock_service_);

    NbUser::ImportCurrentLogin(service, kUserJson);
    EXPECT_TRUE(NbUser::IsLoggedIn(service));

    NbResult<NbUser> result = NbUser::Logout(service);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_TRUE(NbUser::IsLoggedIn(service));
}

//NbUser::Logout(未ログイン)
TEST(NbUser, LogoutNotLoggedIn) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbResult<NbUser> result = NbUser::Logout(service);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_NOT_LOGGED_IN, result.GetResultCode());
    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

//NbUser::Logout(request_factory構築失敗)
TEST(NbUser, LogoutRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbUser::ImportCurrentLogin(service, kUserJson);
    EXPECT_TRUE(NbUser::IsLoggedIn(service));

    NbResult<NbUser> result = NbUser::Logout(service);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
    EXPECT_TRUE(NbUser::IsLoggedIn(service));
}

//NbUser::Logout(接続数オーバー)
TEST_F(NbUserTest, LogoutConnectionOver) {
    SetExpect(nullptr, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbUser::ImportCurrentLogin(service, kUserJson);
    EXPECT_TRUE(NbUser::IsLoggedIn(service));

    NbResult<NbUser> result = NbUser::Logout(service);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
    EXPECT_TRUE(NbUser::IsLoggedIn(service));
}

//NbUser::IsLoggedIn
TEST(NbUser, IsLoggedIn) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbUser::ImportCurrentLogin(service, kUserJson);
    EXPECT_TRUE(NbUser::IsLoggedIn(service));

    NbSessionToken token_org = service->GetSessionToken();
    NbSessionToken token = token_org;
    token.SetSessionToken(kEmpty, token.GetExpireAt());
    service->SetSessionToken(token);
    EXPECT_FALSE(NbUser::IsLoggedIn(service));

    token = token_org;
    token.SetSessionToken(token.GetSessionToken(), std::time(nullptr) - 1);
    service->SetSessionToken(token);
    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

//NbUser::Import/Export
TEST(NbUser, ImportExport) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbUser::ImportCurrentLogin(service, kUserJson);
    EXPECT_TRUE(NbUser::IsLoggedIn(service));

    string export_data = NbUser::ExportCurrentLogin(service);
    NbJsonObject json(kUserJson);
    EXPECT_EQ(json.ToJsonString(), export_data);
}

//NbUser::Export(NotLogin)
TEST(NbUser, ExportNotLogin) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    EXPECT_FALSE(NbUser::IsLoggedIn(service));

    string export_data = NbUser::ExportCurrentLogin(service);
    EXPECT_EQ(kEmpty, export_data);
}

//NbUser::Import(セッショントークン空)
TEST(NbUser, ImportSessionTokenEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbJsonObject json(kUserJson);
    json["sessionToken"] = kEmpty;
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, NbUser::ImportCurrentLogin(service, json.ToJsonString()));
}

//NbUser::Import(有効期限切れ)
TEST(NbUser, ImportExpired) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbJsonObject json(kUserJson);
    json["expire"] = std::time(nullptr) - 1;
    EXPECT_EQ(NbResultCode::NB_ERROR_SESSION_EXPIRED, NbUser::ImportCurrentLogin(service, json.ToJsonString()));
}
} //namespace necbaas
