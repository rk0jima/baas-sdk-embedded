#include "gtest/gtest.h"
#include "ft_data.h"
#include "necbaas/nb_object_bucket.h"
#include "ft_util.h"
#include "necbaas/internal/nb_utility.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;

// フィクスチャ定義
class NbObjectManual : public ::testing::Test {
  protected:
    static void SetUpTestCase() {
        service_ = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    }
    static void TearDownTestCase() {}

    virtual void SetUp() {}
    virtual void TearDown() {
        FTUtil::DeleteAllObject();
    }

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
};

shared_ptr<NbService> NbObjectManual::service_;

// オブジェクトの作成
TEST_F(NbObjectManual, SaveNew) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    object["_id"] = "invalid";
    object["createdAt"] = "invalid";
    object["updatedAt"] = "invalid";
    object["ACL"] = R"({"r":[],"w":[],"c":[],"u":[],"d":[],"admin":[]})";
    object["etag"] = "invalid";
    object["_deleted"] = "invalid";

    NbResult<NbObject> result = object.Save(true);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
}

// オブジェクトの更新
TEST_F(NbObjectManual, Save) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    object.Save();

    NbObject object2 = object_bucket.NewObject();
    object2["testKey"] = "testValue";
    NbResult<NbObject> result = object2.Save();

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(409, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_EQ("duplicate_key", json.GetString("reasonCode"));
    EXPECT_FALSE(json.GetString("detail").empty());
}

// オブジェクトのクエリ
TEST_F(NbObjectManual, Query) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;

    // リクエストパラメータに含まれる
    query.Limit(2)
         .Skip(3)
         .OrderBy(std::vector<std::string>{"-data1"})
         .Projection(std::map<std::string, bool>{{"data2", false}})
         .EqualTo("data2", 2)
         .DeleteMark(true)
         .ReadPreference(NbReadPreference::SECONDARY_PREFERRED)
         .Timeout(1);

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
}

//　オブジェクトのクエリ.デフォルト
TEST_F(NbObjectManual, QueryDefault) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;

    query.Skip(0)
         .Limit(100)
         .ReadPreference(NbReadPreference::PRIMARY)
         .Timeout(0);

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
}

//testA
TEST_F(NbObjectManual, UpdateConflictA) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    object.Save();

    // キー入力待ち(コンソール)
    getchar();

    NbJsonObject json(R"({"UpdateKey": "UpdateValue"})");

    for (int i = 0; i < 1000; ++i ) {
        object.SetETag("");
        NbResult<NbObject> result2 = object.PartUpdateObject(json);

        if (result2.IsRestError()){
            NbJsonObject json2(result2.GetRestError().reason);
            if (409 == result2.GetRestError().status_code && "request_conflicted" == json2.GetString("reasonCode")) {
                std::cout << "request_conflicted" << std::endl;

                // detailチェック
                EXPECT_TRUE(json2.IsMember("detail"));
            } else {
                // 異常発生：アサートさせる
                ASSERT_EQ(200, result2.GetRestError().status_code);
            }
        } else if (result2.IsFatalError()) {
            // 異常発生：アサートさせる
            ASSERT_EQ(NbResultCode::NB_OK, result2.GetResultCode());
        }
    }
}

//testB
TEST_F(NbObjectManual, UpdateConflictB) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    // キー入力待ち(コンソール)
    getchar();

    NbQuery query;
    int count;

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);
    EXPECT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(1,count);

    NbObject &object = response[0];
    NbJsonObject json(R"({"UpdateKey": "UpdateValue"})");

    for (int i = 0; i < 1000; ++i ) {
        object.SetETag("");
        NbResult<NbObject> result2 = object.PartUpdateObject(json);

        if (result2.IsRestError()){
            NbJsonObject json2(result2.GetRestError().reason);
            if (409 == result2.GetRestError().status_code && "request_conflicted" == json2.GetString("reasonCode")) {
                std::cout << "request_conflicted" << std::endl;

                // detailチェック
                EXPECT_TRUE(json2.IsMember("detail"));
            } else {
                // 異常発生：アサートさせる
                ASSERT_EQ(200, result2.GetRestError().status_code);
            }
        } else if (result2.IsFatalError()) {
            // 異常発生：アサートさせる
            ASSERT_EQ(NbResultCode::NB_OK, result2.GetResultCode());
        }
    }
}
} //namespace necbaas
