#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_user.h"
#include "necbaas/internal/nb_utility.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;
using std::time_t;

class NbUserFT : public ::testing::Test {
  protected:
    static void SetUpTestCase() {
        service_ = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
        service_masterkey_ = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
    }
    static void TearDownTestCase() {}

    virtual void SetUp() {}
    virtual void TearDown() {
        if (NbUser::IsLoggedIn(service_)) {
            NbResult<NbUser> result = NbUser::Logout(service_);
        }
        if (NbUser::IsLoggedIn(service_masterkey_)) {
            NbResult<NbUser> result = NbUser::Logout(service_masterkey_);
        }
    }

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
    static shared_ptr<NbService> service_masterkey_;
};

shared_ptr<NbService> NbUserFT::service_;
shared_ptr<NbService> NbUserFT::service_masterkey_;

// 戻り値確認(デフォルトユーザ)
static void CheckDefaultUser(const NbUser &user) {
    EXPECT_EQ(kUserName, user.GetUserName());
    EXPECT_FALSE(user.GetUserId().empty());
    EXPECT_EQ(kEmail, user.GetEmail());
    EXPECT_EQ(kOptions, user.GetOptions());
    FTUtil::CompareList(kGroups, user.GetGroups());
    EXPECT_NE(0, user.GetCreatedTime().tm_year);
    EXPECT_NE(0, user.GetUpdatedTime().tm_year);
}

// 初期値確認
static void CheckInitialUser(const NbUser &initial_user) {
    EXPECT_TRUE(initial_user.GetUserName().empty());
    EXPECT_TRUE(initial_user.GetUserId().empty());
    EXPECT_TRUE(initial_user.GetEmail().empty());
    EXPECT_TRUE(initial_user.GetOptions().IsEmpty());
    EXPECT_TRUE(initial_user.GetGroups().empty());
    EXPECT_EQ(0, initial_user.GetCreatedTime().tm_year);
    EXPECT_EQ(0, initial_user.GetUpdatedTime().tm_year);
}

// ユーザ名でログイン(成功)
TEST_F(NbUserFT, LoginWithUsername) {
    // 初期値確認
    NbUser initial_user = NbUser::GetCurrentUser(service_);
    CheckInitialUser(initial_user);
    EXPECT_TRUE(initial_user.GetSessionToken(service_).empty());
    EXPECT_EQ(0, initial_user.GetSessionTokenExpiration(service_));

    // 未ログイン状態確認
    ASSERT_FALSE(NbUser::IsLoggedIn(service_));

    // ログイン
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    EXPECT_FALSE(NbUser::GetSessionToken(service_).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_));
    NbUser current_user = NbUser::GetCurrentUser(service_);

    // NbUserの比較
    FTUtil::CompareUser(user, current_user);
}

// ユーザ名(@有り)でログイン(成功)
TEST_F(NbUserFT, LoginWithUsernameAtmark) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_,"userTest@", kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    EXPECT_EQ("userTest@", user.GetUserName());
    EXPECT_FALSE(user.GetUserId().empty());
    EXPECT_EQ("userTest2@example.com", user.GetEmail());
    EXPECT_EQ(kOptions, user.GetOptions());
    FTUtil::CompareList(kGroups, user.GetGroups());
    EXPECT_NE(0, user.GetCreatedTime().tm_year);
    EXPECT_NE(0, user.GetUpdatedTime().tm_year);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    EXPECT_FALSE(NbUser::GetSessionToken(service_).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_));
    NbUser current_user = NbUser::GetCurrentUser(service_);

    // NbUserの比較
    FTUtil::CompareUser(user, current_user);

}

// ログイン済み状態で再度同一ユーザでログイン(成功)
TEST_F(NbUserFT, LoginWithUsernameSecond) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));

    NbResult<NbUser> result2 = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result2.IsSuccess());
    NbUser user2 = result2.GetSuccessData();
    CheckDefaultUser(user2);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    EXPECT_FALSE(NbUser::GetSessionToken(service_).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_));
    NbUser current_user2 = NbUser::GetCurrentUser(service_);

    // NbUserの比較
    FTUtil::CompareUser(user2, current_user2);
}

// ユーザ名でログイン(成功)(ACL権限無し、マスターキー使用)
TEST_F(NbUserFT, LoginWithUsernameNotACLMasterKey) {
    // _USERSバケットACL更新
    string content_acl = R"({"r":[],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("object", "_USERS", "", content_acl);

    NbResult<NbUser> result = NbUser::LoginWithUsername(service_masterkey_, kUserName, kPassword);

    // _USERSバケットACLデフォルト設定
    content_acl = R"({"r":["g:authenticated"],"w":[],"c":["g:anonymous"],"u":[],"d":[]})";
    FTUtil::CreateBucket("object", "_USERS", "", content_acl);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_masterkey_));
    EXPECT_FALSE(NbUser::GetSessionToken(service_masterkey_).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_masterkey_));
    NbUser current_user = NbUser::GetCurrentUser(service_masterkey_);

    // NbUserの比較
    FTUtil::CompareUser(user, current_user);
}

// ログイン(失敗)(パスワード空)
TEST_F(NbUserFT, LoginWithUsernamePasswordEmpty) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, "");

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
}

// ログイン(失敗)(ユーザ名空)
TEST_F(NbUserFT, LoginWithUsernameEmpty) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, "", kPassword);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
}

// ログイン(失敗)(未登録ユーザ)
TEST_F(NbUserFT, LoginWithUsernameNotUser) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, "Invalid_User", kPassword);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(401, rest_error.status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
}

// ログイン(失敗)(ユーザ名、パスワード不一致)
TEST_F(NbUserFT, LoginWithUsernameUsernamePasswordMismatch) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, "Invalid_Password");

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(401, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
}

// ログイン情報保存(ユーザ名)
TEST_F(NbUserFT, LoginInfoSaveLoginWithUsername) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン情報比較
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    NbUser current_user = NbUser::GetCurrentUser(service_);
    FTUtil::CompareUser(user, current_user);

    EXPECT_FALSE(current_user.GetSessionToken(service_).empty());
    EXPECT_LT(0, current_user.GetSessionTokenExpiration(service_));

    // 再度ログイン(失敗)
    NbResult<NbUser> result2 = NbUser::LoginWithUsername(service_, kUserName, "Invalid_Password");

    // 戻り値確認
    EXPECT_TRUE(result2.IsRestError());
    NbRestError rest_error = result2.GetRestError();
    EXPECT_EQ(401, result2.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));

    // ログイン情報比較
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    NbUser current_user2 = NbUser::GetCurrentUser(service_);
    FTUtil::CompareUser(user, current_user2);

    EXPECT_EQ(user.GetSessionToken(service_), current_user2.GetSessionToken(service_));
    EXPECT_EQ(user.GetSessionTokenExpiration(service_), current_user2.GetSessionTokenExpiration(service_));
}

// Emailでログイン(成功)
TEST_F(NbUserFT, LoginWithEmail) {
    NbResult<NbUser> result = NbUser::LoginWithEmail(service_, kEmail, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    EXPECT_FALSE(NbUser::GetSessionToken(service_).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_));
    NbUser current_user = NbUser::GetCurrentUser(service_);

    // NbUserの比較
    FTUtil::CompareUser(user, current_user);
}

// ログイン済み状態で再度同一Emailでログイン(成功)
TEST_F(NbUserFT, LoginWithEmailSecond) {
    NbResult<NbUser> result = NbUser::LoginWithEmail(service_, kEmail, kPassword);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));

    NbResult<NbUser> result2 = NbUser::LoginWithEmail(service_, kEmail, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result2.IsSuccess());
    NbUser user2 = result2.GetSuccessData();
    CheckDefaultUser(user2);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    EXPECT_FALSE(NbUser::GetSessionToken(service_).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_));
    NbUser current_user2 = NbUser::GetCurrentUser(service_);

    // NbUserの比較
    FTUtil::CompareUser(user2, current_user2);
}

// ユーザ名でログイン(成功)(ACL権限なし)
TEST_F(NbUserFT, LoginWithEmailNotACL) {
    // _USERSバケットACL更新
    string content_acl = R"({"r":[],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("object", "_USERS", "", content_acl);

    NbResult<NbUser> result = NbUser::LoginWithEmail(service_, kEmail, kPassword);

    // _USERSバケットACLデフォルト設定
    content_acl = R"({"r":["g:authenticated"],"w":[],"c":["g:anonymous"],"u":[],"d":[]})";
    FTUtil::CreateBucket("object", "_USERS", "", content_acl);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    EXPECT_FALSE(NbUser::GetSessionToken(service_).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_));
    NbUser current_user = NbUser::GetCurrentUser(service_);

    // NbUserの比較
    FTUtil::CompareUser(user, current_user);
}

// ログイン(失敗)(パスワード空)
TEST_F(NbUserFT, LoginWithEmailPasswordEmpty) {
    NbResult<NbUser> result = NbUser::LoginWithEmail(service_, kEmail, "");

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
}

// ログイン(失敗)(Email空)
TEST_F(NbUserFT, LoginWithEmailEmpty) {
    NbResult<NbUser> result = NbUser::LoginWithEmail(service_, "", kPassword);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
}

// ログイン(失敗)(未登録ユーザ)
TEST_F(NbUserFT, LoginWithEmailNotUser) {
    NbResult<NbUser> result = NbUser::LoginWithEmail(service_, "Invalid_Email", kPassword);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(401, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
}

// ログイン(失敗)(Email、パスワード不一致)
TEST_F(NbUserFT, LoginWithEmailEmailPasswordMismatch) {
    NbResult<NbUser> result = NbUser::LoginWithEmail(service_, kEmail, "Invalid_Password");

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(401, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
}

// ログイン情報保存(Email)
TEST_F(NbUserFT, LoginInfoSaveLoginWithEmail) {
    NbResult<NbUser> result = NbUser::LoginWithEmail(service_, kEmail, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン情報比較
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    NbUser current_user = NbUser::GetCurrentUser(service_);
    FTUtil::CompareUser(user, current_user);

    EXPECT_FALSE(current_user.GetSessionToken(service_).empty());
    EXPECT_LT(0, current_user.GetSessionTokenExpiration(service_));

    // 再度ログイン(失敗)
    NbResult<NbUser> result2 = NbUser::LoginWithEmail(service_, kEmail, "Invalid_Password");

    // 戻り値確認
    EXPECT_TRUE(result2.IsRestError());
    NbRestError rest_error = result2.GetRestError();
    EXPECT_EQ(401, result2.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));

    // ログイン情報比較
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    NbUser current_user2 = NbUser::GetCurrentUser(service_);
    FTUtil::CompareUser(user, current_user2);

    EXPECT_EQ(user.GetSessionToken(service_), current_user2.GetSessionToken(service_));
    EXPECT_EQ(user.GetSessionTokenExpiration(service_), current_user2.GetSessionTokenExpiration(service_));

}


// ログアウト(成功)
TEST_F(NbUserFT, Logout) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_TRUE(NbUser::IsLoggedIn(service_));

    // ログアウト実行
    result = NbUser::Logout(service_);
    EXPECT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    EXPECT_FALSE(user.GetUserId().empty());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));

    // ログイン情報破棄確認
    NbUser current_user = NbUser::GetCurrentUser(service_);
    CheckInitialUser(current_user);
    EXPECT_TRUE(current_user.GetSessionToken(service_).empty());
    EXPECT_EQ(0, current_user.GetSessionTokenExpiration(service_));
}

// ログアウト(失敗)(未ログイン状態)
TEST_F(NbUserFT, LogoutNotLoggedIn) {
    // ログアウト実行
    NbResult<NbUser> result = NbUser::Logout(service_);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_NOT_LOGGED_IN, result.GetResultCode());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
}


// セッショントークン無効
TEST_F(NbUserFT, LogoutSessionTokenNG) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_TRUE(NbUser::IsLoggedIn(service_));

    // ログアウト REST API
    NbResult<NbHttpResponse> rest_result = service_->ExecuteRequest(
        [](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Delete("/login")
                          .Build();
        }, 60);
        EXPECT_TRUE(rest_result.IsSuccess());

    // ログアウト実行
    result = NbUser::Logout(service_);

    // RESTエラー確認
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(401, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
}

// ログイン情報破棄(RESTエラー)
TEST_F(NbUserFT, LogoutLoginInfoDestructRestError) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_TRUE(NbUser::IsLoggedIn(service_));

    // ログアウト REST API
    NbResult<NbHttpResponse> rest_result = service_->ExecuteRequest(
        [](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Delete("/login")
                          .Build();
        }, 60);
        EXPECT_TRUE(rest_result.IsSuccess());

    // ログイン状態確認
    EXPECT_TRUE(NbUser::IsLoggedIn(service_));

    // ログアウト実行
    result = NbUser::Logout(service_);

    // RESTエラー確認
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(401, result.GetRestError().status_code);

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));

    // ログイン情報破棄確認
    NbUser current_user = NbUser::GetCurrentUser(service_);
    CheckInitialUser(current_user);
    EXPECT_TRUE(current_user.GetSessionToken(service_).empty());
    EXPECT_EQ(0, current_user.GetSessionTokenExpiration(service_));
}

// ログイン情報破棄(FATALエラー)
TEST_F(NbUserFT, LogoutLoginInfoDestructFatalError) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_TRUE(NbUser::IsLoggedIn(service_));

    // 有効期限切れに設定
    NbSessionToken token_org = service_->GetSessionToken();
    string token = token_org.GetSessionToken();
    time_t expire = std::time(nullptr) - 1;
    token_org.SetSessionToken(token, expire);
    service_->SetSessionToken(token_org);

    // ログアウト実行
    result = NbUser::Logout(service_);

    // FATALエラー確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_NOT_LOGGED_IN, result.GetResultCode());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));

    // ログイン情報破棄確認
    NbUser current_user = NbUser::GetCurrentUser(service_);
    CheckInitialUser(current_user);
    EXPECT_TRUE(current_user.GetSessionToken(service_).empty());
    EXPECT_EQ(0, current_user.GetSessionTokenExpiration(service_));
}

// セッショントークン Export
TEST_F(NbUserFT, Export) {
    // ログイン状態確認
    ASSERT_FALSE(NbUser::IsLoggedIn(service_));

    // Export
    string export_data = NbUser::ExportCurrentLogin(service_);
    EXPECT_TRUE(export_data.empty());

    // ログイン
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    EXPECT_FALSE(NbUser::GetSessionToken(service_).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_));
    NbUser current_user = NbUser::GetCurrentUser(service_);

    // Export
    string export_data2 = NbUser::ExportCurrentLogin(service_);
    EXPECT_FALSE(export_data2.empty());
    NbJsonObject json(export_data2);

    // 戻り値確認
    EXPECT_EQ(user.GetUserName(), json.GetString("username"));
    EXPECT_EQ(user.GetUserId(), json.GetString("_id"));
    EXPECT_EQ(user.GetEmail(), json.GetString("email"));
    EXPECT_EQ(user.GetOptions(), json.GetJsonObject("options"));

    vector<string> group_list;
    NbJsonArray group_array = json.GetJsonArray("groups");
    group_array.GetAllString(&group_list);
    FTUtil::CompareList(user.GetGroups(), group_list);

    string created_at = json.GetString("createdAt");
    FTUtil::CompareTime(user.GetCreatedTime(), NbUtility::DateStringToTm(created_at), true);

    string updated_at = json.GetString("updatedAt");
    FTUtil::CompareTime(user.GetUpdatedTime(), NbUtility::DateStringToTm(updated_at), true);

    EXPECT_EQ(user.GetSessionToken(service_), json.GetString("sessionToken"));

    EXPECT_EQ(user.GetSessionTokenExpiration(service_), json.GetInt("expire"));

    // 有効期限切れに設定
    NbSessionToken token_org = service_->GetSessionToken();
    string token = token_org.GetSessionToken();
    time_t expire = std::time(nullptr) - 1;
    token_org.SetSessionToken(token, expire);
    service_->SetSessionToken(token_org);

    // Export
    string export_data3 = NbUser::ExportCurrentLogin(service_);
    EXPECT_TRUE(export_data3.empty());
}

// セッショントークン Import
// データ正常
TEST_F(NbUserFT, Import) {
    shared_ptr<NbService> service_2 = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    // ログイン(service_)
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));

    // Export
    string export_data = NbUser::ExportCurrentLogin(service_);
    EXPECT_FALSE(export_data.empty());

    // Import
    NbUser::ImportCurrentLogin(service_2, export_data);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_2));

    //current_userとexport_dataの比較
    NbUser current_user2 = NbUser::GetCurrentUser(service_2);
    NbJsonObject json(export_data);
    EXPECT_EQ(current_user2.GetUserName(), json.GetString("username"));
    EXPECT_EQ(current_user2.GetUserId(), json.GetString("_id"));
    EXPECT_EQ(current_user2.GetEmail(), json.GetString("email"));
    EXPECT_EQ(current_user2.GetOptions(), json.GetJsonObject("options"));

    vector<string> group_list;
    NbJsonArray group_array = json.GetJsonArray("groups");
    group_array.GetAllString(&group_list);
    FTUtil::CompareList(current_user2.GetGroups(), group_list);

    string created_at = json.GetString("createdAt");
    FTUtil::CompareTime(current_user2.GetCreatedTime(), NbUtility::DateStringToTm(created_at), true);

    string updated_at = json.GetString("updatedAt");
    FTUtil::CompareTime(current_user2.GetUpdatedTime(), NbUtility::DateStringToTm(updated_at), true);

    EXPECT_EQ(current_user2.GetSessionToken(service_), json.GetString("sessionToken"));

    EXPECT_EQ(current_user2.GetSessionTokenExpiration(service_), json.GetInt("expire"));

    // ログアウト実行
    result = NbUser::Logout(service_2);
    EXPECT_TRUE(result.IsSuccess());
    NbUser user3 = result.GetSuccessData();
    EXPECT_FALSE(user3.GetUserId().empty());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_2));

    // ログイン情報破棄確認
    NbUser current_user3 = NbUser::GetCurrentUser(service_2);
    CheckInitialUser(current_user3);
    EXPECT_TRUE(current_user3.GetSessionToken(service_2).empty());
    EXPECT_EQ(0, current_user3.GetSessionTokenExpiration(service_2));
}

// セッショントークン Import
// セッショントークンなし
TEST_F(NbUserFT, ImportNotSessionToken) {
    // ログイン
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));

    // Export
    string export_data = NbUser::ExportCurrentLogin(service_);
    EXPECT_FALSE(export_data.empty());

    // セッショントークン削除
    NbJsonObject json(export_data);
    json.Remove("sessionToken");

    // Import 戻り値確認
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, NbUser::ImportCurrentLogin(service_, json.ToJsonString()));
}

// セッショントークン Import
// セッショントークンValueなし
TEST_F(NbUserFT, ImportNotSessionTokenValue) {
    // ログイン
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));

    // Export
    string export_data = NbUser::ExportCurrentLogin(service_);
    EXPECT_FALSE(export_data.empty());

    // セッショントークンValueに空文字
    NbJsonObject json(export_data);
    json["sessionToken"] = "";

    // Import 戻り値確認
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, NbUser::ImportCurrentLogin(service_, json.ToJsonString()));
}

// セッショントークン Import
// 有効期限データなし
TEST_F(NbUserFT, ImportNotExpire) {
    // ログイン
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));

    // Export
    string export_data = NbUser::ExportCurrentLogin(service_);
    EXPECT_FALSE(export_data.empty());

    // 有効期限データ削除 
    NbJsonObject json(export_data);
    json.Remove("expire");

    // Import 戻り値確認
    EXPECT_EQ(NbResultCode::NB_ERROR_SESSION_EXPIRED , NbUser::ImportCurrentLogin(service_, json.ToJsonString()));
}

// セッショントークン Import
// 有効期限切れ
TEST_F(NbUserFT, ImportOverExpire) {
    // ログイン
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));

    // Export
    string export_data = NbUser::ExportCurrentLogin(service_);
    EXPECT_FALSE(export_data.empty());

    // 有効期限切れに設定
    NbJsonObject json(export_data);
    json["expire"] = (int32_t)std::time(nullptr) - 1;

    // Import 戻り値確認
    EXPECT_EQ(NbResultCode::NB_ERROR_SESSION_EXPIRED, NbUser::ImportCurrentLogin(service_, json.ToJsonString()));
}

// セッショントークン Import
// ユーザ情報なし
TEST_F(NbUserFT, ImportNotUserInfo) {
    // ログイン
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));

    // Export
    string export_data = NbUser::ExportCurrentLogin(service_);
    EXPECT_FALSE(export_data.empty());

    // ユーザデータ削除 
    NbJsonObject json(export_data);
    json.Remove("_id");
    json.Remove("username");
    json.Remove("email");
    json.Remove("options");

    // Import
    NbUser::ImportCurrentLogin(service_, json.ToJsonString());

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));

    // カレントユーザ確認
    NbUser current_user = NbUser::GetCurrentUser(service_);
    EXPECT_EQ("", current_user.GetUserName());
    EXPECT_EQ("", current_user.GetUserId());
    EXPECT_EQ("", current_user.GetEmail());
    EXPECT_EQ("{}", current_user.GetOptions().ToJsonString());
}

// マルチテナント
TEST_F(NbUserFT, MultiTenant) {
    shared_ptr<NbService> service_2 = NbService::CreateService(kEndPointUrl, kTenantIdB, kAppIdB, kAppKeyB, kProxyB);

    ASSERT_FALSE(NbUser::IsLoggedIn(service_));

    // ログイン
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    CheckDefaultUser(user);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    ASSERT_FALSE(NbUser::IsLoggedIn(service_2));
    EXPECT_FALSE(NbUser::GetSessionToken(service_).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_));
    NbUser current_user = NbUser::GetCurrentUser(service_);

    // NbUserの比較
    FTUtil::CompareUser(user, current_user);

    // 1秒スリープ
    sleep(1);

    // 別テナントログイン
    NbResult<NbUser> result2 = NbUser::LoginWithUsername(service_2, kUserName, kPassword);
    ASSERT_TRUE(result2.IsSuccess());
    NbUser user2 = result2.GetSuccessData();
    CheckDefaultUser(user2);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    ASSERT_TRUE(NbUser::IsLoggedIn(service_2));
    EXPECT_FALSE(NbUser::GetSessionToken(service_2).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_2));
    NbUser current_user2 = NbUser::GetCurrentUser(service_2);

    // NbUserの比較
    FTUtil::CompareUser(user2, current_user2);

    // セッショントークン有効期限比較
    EXPECT_NE(NbUser::GetSessionTokenExpiration(service_), NbUser::GetSessionTokenExpiration(service_2));

    // ログアウト実行
    result = NbUser::Logout(service_);
    EXPECT_TRUE(result.IsSuccess());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
    EXPECT_TRUE(NbUser::IsLoggedIn(service_2));

    // ログアウト実行(Tenant2)
    result2 = NbUser::Logout(service_2);
    EXPECT_TRUE(result2.IsSuccess());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
    EXPECT_FALSE(NbUser::IsLoggedIn(service_2));
}

// 繰り返し評価(ログイン、ログアウト各3回実施)
TEST_F(NbUserFT, Login3Logout3) {
    for (int i = 0; i < 3; ++i ) {
        //ログイン
        NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

        // 戻り値確認
        ASSERT_TRUE(result.IsSuccess());
        NbUser user = result.GetSuccessData();
        CheckDefaultUser(user);

        // ログイン状態確認
        ASSERT_TRUE(NbUser::IsLoggedIn(service_));
        EXPECT_FALSE(NbUser::GetSessionToken(service_).empty());
        EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_));
        NbUser current_user = NbUser::GetCurrentUser(service_);

        // NbUserの比較
        FTUtil::CompareUser(user, current_user);

        //ログアウト
        result = NbUser::Logout(service_);
        EXPECT_TRUE(result.IsSuccess());
        NbUser user_logout = result.GetSuccessData();
        EXPECT_FALSE(user_logout.GetUserId().empty());

        // ログイン状態確認
        EXPECT_FALSE(NbUser::IsLoggedIn(service_));
    }
}
} //namespace necbaas
