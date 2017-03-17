#include "gtest/gtest.h"
#include "necbaas/internal/nb_session_token.h"
#include "necbaas/internal/nb_session_token.h"

namespace necbaas {

using std::string;

const string kUserEntity{R"(
    {"_id":"58a57e2dcf81aa0f604d0389",
     "username":"testuser",
     "email":"test@hogehoge.com",
     "groups":["group1","group2"],
     "options":{"option":"value"},
     "createdAt":"2017-02-16T10:25:49.248Z",
     "updatedAt":"2017-02-16T10:27:57.045Z",
     "expire":1488454881})"};

const string kSessionToken{"sessionToken"};
const int kExpireAt{0x12345678};

//NbSessionToken::NbSessionToken(デフォルトコンストラクタ)
TEST(NbSessionToken, NbSessionToken) {
    NbSessionToken session_token;
    EXPECT_EQ(string(""), session_token.GetSessionToken());
    EXPECT_EQ(0, session_token.GetExpireAt());
    EXPECT_EQ(string(""), session_token.GetSessionUserEntity().GetUserName());
}

//NbSessionToken::NbSessionToken(session_token)
TEST(NbSessionToken, NbSessionTokenJson) {
    NbSessionToken session_token(kSessionToken, kExpireAt);
    EXPECT_EQ(kSessionToken, session_token.GetSessionToken());
    EXPECT_EQ(kExpireAt, static_cast<int>(session_token.GetExpireAt()));
    EXPECT_EQ(string(""), session_token.GetSessionUserEntity().GetUserName());
}

//NbSessionToken::UserEntity&Clear
TEST(NbSessionToken, UserEntity) {
    NbSessionToken session_token(kSessionToken, kExpireAt);
    NbJsonObject json(kUserEntity);
    NbUserEntity entity(json);
    session_token.SetSessionUserEntity(entity);
    
    EXPECT_EQ(kSessionToken, session_token.GetSessionToken());
    EXPECT_EQ(kExpireAt, static_cast<int>(session_token.GetExpireAt()));
    EXPECT_EQ(string("58a57e2dcf81aa0f604d0389"), session_token.GetSessionUserEntity().GetId());
    EXPECT_EQ(string("testuser"), session_token.GetSessionUserEntity().GetUserName());
    EXPECT_EQ(string("test@hogehoge.com"), session_token.GetSessionUserEntity().GetEmail());

    // クリア
    session_token.ClearSessionToken();
    EXPECT_EQ(string(""), session_token.GetSessionToken());
    EXPECT_EQ(0, session_token.GetExpireAt());
    EXPECT_EQ(string(""), session_token.GetSessionUserEntity().GetId());
    EXPECT_EQ(string(""), session_token.GetSessionUserEntity().GetUserName());
    EXPECT_EQ(string(""), session_token.GetSessionUserEntity().GetEmail());
}

//NbSessionToken::IsExpired
TEST(NbSessionToken, IsExpired) {
    EXPECT_TRUE(NbSessionToken::IsExpired(std::time(nullptr)));
    EXPECT_FALSE(NbSessionToken::IsExpired(std::time(nullptr) + 1));
}
} //namespace necbaas
