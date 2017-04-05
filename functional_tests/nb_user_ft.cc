#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_user.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;

class NbUserFT : public ::testing::Test {
  protected:
    static void SetUpTestCase() {
        service_ = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    }
    static void TearDownTestCase() {}

    virtual void SetUp() {}
    virtual void TearDown() {
        if (NbUser::IsLoggedIn(service_)) {
            NbResult<NbUser> result = NbUser::Logout(service_);
        }
    }

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
};

shared_ptr<NbService> NbUserFT::service_;

// ユーザ名でログイン(成功)
TEST_F(NbUserFT, LoginWithUsername) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    EXPECT_EQ(kUserName, user.GetUserName());
    EXPECT_EQ(kEmail, user.GetEmail());
    EXPECT_EQ(kOptions, user.GetOptions());
    FTUtil::CompareList(kGroups, user.GetGroups());
    EXPECT_FALSE(user.GetUserId().empty());
    EXPECT_NE(0, user.GetCreatedTime().tm_year);
    EXPECT_NE(0, user.GetUpdatedTime().tm_year);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    EXPECT_FALSE(NbUser::GetSessionToken(service_).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_));
    NbUser current_user = NbUser::GetCurrentUser(service_);
    EXPECT_EQ(kUserName, current_user.GetUserName());
    EXPECT_EQ(kEmail, current_user.GetEmail());
    EXPECT_EQ(kOptions, current_user.GetOptions());
    FTUtil::CompareList(kGroups, current_user.GetGroups());
    EXPECT_FALSE(current_user.GetUserId().empty());
    EXPECT_NE(0, current_user.GetCreatedTime().tm_year);
    EXPECT_NE(0, current_user.GetUpdatedTime().tm_year);
}

// Emailでログイン(成功)
TEST_F(NbUserFT, LoginWithEmail) {
    NbResult<NbUser> result = NbUser::LoginWithEmail(service_, kEmail, kPassword);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    EXPECT_EQ(kUserName, user.GetUserName());
    EXPECT_EQ(kEmail, user.GetEmail());
    EXPECT_EQ(kOptions, user.GetOptions());
    FTUtil::CompareList(kGroups, user.GetGroups());
    EXPECT_FALSE(user.GetUserId().empty());
    EXPECT_NE(0, user.GetCreatedTime().tm_year);
    EXPECT_NE(0, user.GetUpdatedTime().tm_year);

    // ログイン状態確認
    ASSERT_TRUE(NbUser::IsLoggedIn(service_));
    EXPECT_FALSE(NbUser::GetSessionToken(service_).empty());
    EXPECT_LT(0, NbUser::GetSessionTokenExpiration(service_));
    NbUser current_user = NbUser::GetCurrentUser(service_);
    EXPECT_EQ(kUserName, current_user.GetUserName());
    EXPECT_EQ(kEmail, current_user.GetEmail());
    EXPECT_EQ(kOptions, current_user.GetOptions());
    FTUtil::CompareList(kGroups, current_user.GetGroups());
    EXPECT_FALSE(current_user.GetUserId().empty());
    EXPECT_NE(0, current_user.GetCreatedTime().tm_year);
    EXPECT_NE(0, current_user.GetUpdatedTime().tm_year);
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
}
} //namespace necbaas
