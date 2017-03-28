#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_object_bucket.h"
#include "necbaas/nb_user.h"
#include "necbaas/nb_acl.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;

// フィクスチャ定義
class NbObjectFT : public ::testing::Test {
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
        FTUtil::DeleteAllObject();
    }

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
};

shared_ptr<NbService> NbObjectFT::service_;

// オブジェクト作成.新規作成
// ログイン済み、ACL指定なし
TEST_F(NbObjectFT, SaveNew) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    NbAcl expected_acl;
    expected_acl.SetOwner(login.GetSuccessData().GetUserId());

    NbObjectBucket object_bucket(service_, kObjectBucketName); 
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    NbResult<NbObject> result = object.Save();
 
    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("testValue"), response.GetString("testKey"));
    EXPECT_FALSE(response.GetObjectId().empty());
    EXPECT_NE(0, response.GetCreatedTime().tm_year);
    EXPECT_NE(0, response.GetUpdatedTime().tm_year);
    EXPECT_FALSE(response.GetETag().empty());
    FTUtil::CompareAcl(expected_acl, response.GetAcl());

    FTUtil::CompareObject(object, response);
}

// オブジェクトの読み込.ID指定
// 削除マークあり、データに削除マークあり
TEST_F(NbObjectFT, GetObject) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    NbResult<NbObject> save = object.Save();
    NbResult<NbObject> del = object.DeleteObject(true);

    NbResult<NbObject> result = object_bucket.GetObject(object.GetObjectId(), true);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    FTUtil::CompareObject(object, response);
}

// オブジェクトのクエリ.limit + skip + order + count + projection
TEST_F(NbObjectFT, QueryExcludeWhereOption) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["data1"] = 121;
    object["data2"] = 0;
    NbResult<NbObject> save = object.Save();

    for (int i = 0; i < 10; ++i) {
        object = object_bucket.NewObject();
        object["data1"] = 100 + (i * 5);
        save = object.Save();
    }

    NbQuery query;
    int count;

    query.Limit(5)
         .Skip(3)
         .OrderBy(std::vector<std::string>{"-data1"})
         .Projection(std::map<std::string, bool>{{"data2", false}});
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(11, count);

    EXPECT_EQ(5, response.size());
    EXPECT_EQ(130, response[0].GetInt("data1"));
    EXPECT_EQ(125, response[1].GetInt("data1"));
    EXPECT_EQ(121, response[2].GetInt("data1"));
    EXPECT_FALSE(response[2].IsMember("data2"));
    EXPECT_EQ(120, response[3].GetInt("data1"));
    EXPECT_EQ(115, response[4].GetInt("data1"));
}

// オブジェクトのクエリ.where組み合わせ
TEST_F(NbObjectFT, QueryWhereCombination) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object1 = object_bucket.NewObject();
    object1["data1"] = 1;
    object1["data2"]["data3"] = 3;
    NbResult<NbObject> save = object1.Save();

    NbObject object2 = object_bucket.NewObject();
    object2["data1"] = 3;
    object2["data2"]["data3"] = 3;
    save = object2.Save();

    NbQuery query;
    NbJsonObject json_data2;
    json_data2["data3"] = 3;

    query.LessThan("data1", 2)
         .EqualTo("data2", json_data2);
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();

    EXPECT_EQ(1, response.size());
    EXPECT_EQ(object1, response[0]);
}

// オブジェクト更新(部分更新)
// ACLを変更
TEST_F(NbObjectFT, PartUpdateObject) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(NbAcl::CreateAclForAnonymous());
    object["testKey"] = "testValue";
    NbResult<NbObject> save = object.Save(true);

    NbAcl acl;
    acl.AddEntry(NbAclPermission::WRITE ,"testUser");
    object.SetAcl(acl);

    NbJsonObject json(R"({"UpdateKey": "UpdateValue"})");

    // 更新時間を変化させるためsleep
    sleep(1);
    NbObject original = object;
    NbResult<NbObject> result = object.PartUpdateObject(json, true);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("testValue"), response.GetString("testKey"));
    EXPECT_EQ(string("UpdateValue"), response.GetString("UpdateKey"));
    EXPECT_EQ(original.GetObjectId(), response.GetObjectId());
    FTUtil::CompareTime(original.GetCreatedTime(), response.GetCreatedTime(), true);
    FTUtil::CompareTime(original.GetUpdatedTime(), response.GetUpdatedTime(), false);
    EXPECT_NE(original.GetETag(), response.GetETag());
    FTUtil::CompareAcl(acl, response.GetAcl());

    FTUtil::CompareObject(object, response);
}

// オブジェクト更新(完全上書き)
// 作成日時、ACLを更新
TEST_F(NbObjectFT, SaveFullUpdate) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(NbAcl::CreateAclForAnonymous());
    object["testKey"] = "testValue";
    NbResult<NbObject> save = object.Save(true);

    NbAcl acl;
    acl.AddEntry(NbAclPermission::WRITE ,"testUser");
    object.SetAcl(acl);

    // 更新時間を変化させるためsleep
    sleep(1);
    NbObject original = object;

    object["testKey"] = "testValue2";
    object["UpdateKey"] = "UpdateValue";
    std::tm new_time = FTUtil::IncrementTime(object.GetCreatedTime());
    object.SetCreatedTime(new_time);
    NbResult<NbObject> result = object.Save(true);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("testValue2"), response.GetString("testKey"));
    EXPECT_EQ(string("UpdateValue"), response.GetString("UpdateKey"));
    EXPECT_EQ(original.GetObjectId(), response.GetObjectId());
    FTUtil::CompareTime(new_time, response.GetCreatedTime(), true);
    FTUtil::CompareTime(original.GetUpdatedTime(), response.GetUpdatedTime(), false);
    EXPECT_NE(original.GetETag(), response.GetETag());
    FTUtil::CompareAcl(acl, response.GetAcl());

    FTUtil::CompareObject(object, response);
}

// オブジェクトの削除
// 論理削除
TEST_F(NbObjectFT, DeleteObject) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbAcl acl = NbAcl::CreateAclForAnonymous();
    object.SetAcl(acl);
    object["testKey"] = "testValue";
    NbResult<NbObject> save = object.Save(true);

    // 更新時間を変化させるためsleep
    sleep(1);
    NbObject original = object;

    NbResult<NbObject> result = object.DeleteObject(true);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("testValue"), response.GetString("testKey"));
    EXPECT_EQ(original.GetObjectId(), response.GetObjectId());
    FTUtil::CompareTime(original.GetCreatedTime(), response.GetCreatedTime(), true);
    FTUtil::CompareTime(original.GetUpdatedTime(), response.GetUpdatedTime(), false);
    EXPECT_NE(original.GetETag(), response.GetETag());
    FTUtil::CompareAcl(acl, response.GetAcl());
    EXPECT_TRUE(response.IsDeleteMark());

    FTUtil::CompareObject(object, response);
}
} //namespace necbaas
