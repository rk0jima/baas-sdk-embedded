#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_user.h"

namespace necbaas {

using std::shared_ptr;

class NbUserManual : public ::testing::Test {
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

shared_ptr<NbService> NbUserManual::service_;

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

// ログイン(ユーザ名)
TEST_F(NbUserManual, LoginWithUsername) {
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

    // ログアウト
    result = NbUser::Logout(service_);
    EXPECT_TRUE(result.IsSuccess());
    NbUser user_logout = result.GetSuccessData();
    EXPECT_FALSE(user_logout.GetUserId().empty());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));

    // ログイン情報破棄確認
    NbUser current_user2 = NbUser::GetCurrentUser(service_);
    CheckInitialUser(current_user2);
    EXPECT_TRUE(current_user2.GetSessionToken(service_).empty());
    EXPECT_EQ(0, current_user2.GetSessionTokenExpiration(service_));
}

// ログイン(Email)
TEST_F(NbUserManual, LoginWithEmail) {
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

    // ログアウト
    result = NbUser::Logout(service_);
}

// ログイン(ログアウトなし)
TEST_F(NbUserManual, LoginNotLogout) {
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

// ログイン-ログイン中（期限切れ）の再ログイン
TEST_F(NbUserManual, LoginWithUsernameOverExpire) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());

    // セッショントークン有効期限切れまで待機(0.1h+1s)
    sleep(361);

    // ログイン状態確認
    ASSERT_FALSE(NbUser::IsLoggedIn(service_));

    // 再ログイン
    result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

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

// ログアウト-セッショントークン期限切れ
TEST_F(NbUserManual, LogoutOverExpire) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    EXPECT_TRUE(result.IsSuccess());
    EXPECT_TRUE(NbUser::IsLoggedIn(service_));

    // セッショントークン有効期限切れまで待機(0.1h+1s)
    sleep(361);

    // ログイン状態確認
    ASSERT_FALSE(NbUser::IsLoggedIn(service_));

    // ログアウト
    result = NbUser::Logout(service_);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_NOT_LOGGED_IN, result.GetResultCode());

    // ログイン状態確認
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
}
} //namespace necbaas
