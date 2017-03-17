#include "gtest/gtest.h"
#include "necbaas/internal/nb_user_entity.h"
#include "necbaas/internal/nb_utility.h"

namespace necbaas {

using std::string;
using std::vector;

const string kJsonString{R"(
    {"_id":"58a57e2dcf81aa0f604d0389",
     "username":"testuser",
     "email":"test@hogehoge.com",
     "groups":["group1","group2"],
     "options":{"option":"value"},
     "createdAt":"2017-02-16T10:25:49.248Z",
     "updatedAt":"2017-02-16T10:27:57.045Z",
     "etag":"454c0291-8dcc-4983-a84c-f528832ed307",
     "sessionToken":"spZr0gEzsWpejvTEmcUYrVvrH4ShtfnkzKnc86NE",
     "expire":1488454881,
     "lastLoginAt":"2017-03-01T10:19:15.862Z"})"};

//NbUserEntity::NbUserEntity(文字列あり)
TEST(NbUserEntity, NbUserEntity) {

    NbJsonObject json{kJsonString};

    NbUserEntity user{json};

    {
        EXPECT_EQ(string("58a57e2dcf81aa0f604d0389"), user.GetId());
        EXPECT_EQ(string("testuser"), user.GetUserName());
        EXPECT_EQ(string("test@hogehoge.com"), user.GetEmail());
        NbJsonObject option = user.GetOptions();
        EXPECT_EQ(string("value"), option.GetString("option"));
        vector<string> groups = user.GetGroups();
        EXPECT_EQ(2, groups.size());
        EXPECT_EQ(string("group1"), groups[0]);
        EXPECT_EQ(string("group2"), groups[1]);
        //ミリ秒は0にまるめられる
        EXPECT_EQ(string("2017-02-16T10:25:49.000Z"), NbUtility::TmToDateString(user.GetCreatedTime()));
        EXPECT_EQ(string("2017-02-16T10:27:57.000Z"), NbUtility::TmToDateString(user.GetUpdatedTime()));
    }
    {
        //ToJsonObject
        NbJsonObject user_json = user.ToJsonObject();
        EXPECT_EQ(string("58a57e2dcf81aa0f604d0389"), user_json.GetString("_id"));
        EXPECT_EQ(string("testuser"), user_json.GetString("username"));
        EXPECT_EQ(string("test@hogehoge.com"), user_json.GetString("email"));
        NbJsonObject option = user_json.GetJsonObject("options");
        EXPECT_EQ(string("value"), option.GetString("option"));
        NbJsonArray groups = user_json.GetJsonArray("groups");
        EXPECT_EQ(2, groups.GetSize());
        EXPECT_EQ(string("group1"), groups.GetString(0));
        EXPECT_EQ(string("group2"), groups.GetString(1));
        EXPECT_EQ(string("2017-02-16T10:25:49.248Z"), user_json.GetString("createdAt"));
        EXPECT_EQ(string("2017-02-16T10:27:57.045Z"), user_json.GetString("updatedAt"));
    }
}

//NbUserEntity::NbUserEntity(文字列なし)
TEST(NbUserEntity, NbUserEntityEmpty) {

    NbJsonObject json{};

    NbUserEntity user{json};

    {
        EXPECT_EQ(string(""), user.GetId());
        EXPECT_EQ(string(""), user.GetUserName());
        EXPECT_EQ(string(""), user.GetEmail());
        NbJsonObject option = user.GetOptions();
        EXPECT_EQ(true, option.IsEmpty());
        vector<string> groups = user.GetGroups();
        EXPECT_EQ(true, groups.empty());
        EXPECT_EQ(string(""), NbUtility::TmToDateString(user.GetCreatedTime()));
        EXPECT_EQ(string(""), NbUtility::TmToDateString(user.GetUpdatedTime()));
    }

    {
        //ToJsonObject
        NbJsonObject user_json = user.ToJsonObject();
        EXPECT_EQ(string(""), user_json.GetString("_id"));
        EXPECT_EQ(string(""), user_json.GetString("username"));
        EXPECT_EQ(string(""), user_json.GetString("email"));
        NbJsonObject option = user_json.GetJsonObject("options");
        EXPECT_EQ(true, option.IsEmpty());
        NbJsonArray groups = user_json.GetJsonArray("groups");
        EXPECT_EQ(true, groups.IsEmpty());
        EXPECT_EQ(string(""), user_json.GetString("createdAt"));
        EXPECT_EQ(string(""), user_json.GetString("updatedAt"));
    }
}

} //namespace necbaas
