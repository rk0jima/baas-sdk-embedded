#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_object_bucket.h"
#include "necbaas/nb_user.h"
#include "necbaas/nb_acl.h"
#include "necbaas/internal/nb_utility.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;
using std::chrono::system_clock;

// フィクスチャ定義
class NbObjectFT : public ::testing::Test {
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
        FTUtil::DeleteAllObject();
    }

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
    static shared_ptr<NbService> service_masterkey_;
};

shared_ptr<NbService> NbObjectFT::service_;
shared_ptr<NbService> NbObjectFT::service_masterkey_;

// Query.データ【A】登録
static void RegisterDataA(NbObjectBucket &object_bucket) {
    for (int i = 0; i < 10; ++i) {
        NbObject object = object_bucket.NewObject();
        object["data1"] = 100 + (i * 5);
        object.Save();
    }
}

// Query.データ【B】登録
static void RegisterDataB(NbObjectBucket &object_bucket) {
    NbObject object1 = object_bucket.NewObject();
    NbJsonArray jsonArrayValue(R"([100, 101, 102])");
    object1.PutJsonArray("data1", jsonArrayValue);
    object1.Save();

    for (int i = 0; i < 8; ++i) {
        NbObject object2 = object_bucket.NewObject();
        object2["data1"] = 105 + (i * 5);
        object2.Save();
    }

    NbObject object3 = object_bucket.NewObject();
    NbJsonArray jsonArrayValue2(R"([145, 101, 102])");
    object3.PutJsonArray("data1", jsonArrayValue2);
    object3.Save();

    NbObject object4 = object_bucket.NewObject();
    object4["data2"] = 100;
    object4.Save();
}

// Query.データ【C】登録
static void RegisterDataC(NbObjectBucket &object_bucket) {
    NbObject object1 = object_bucket.NewObject();
    object1["data1"] = "abcdefg";
    object1.Save();

    NbObject object2 = object_bucket.NewObject();
    object2["data1"] = "ABCDEFG";
    object2.Save();

    NbObject object3 = object_bucket.NewObject();
    object3["data1"] = "_012345_";
    object3.Save();

    NbObject object4 = object_bucket.NewObject();
    object4["data1"] = "0123ABC";
    object4.Save();

    NbObject object5 = object_bucket.NewObject();
    object5["data1"] = "0123abc";
    object5.Save();

    NbObject object6 = object_bucket.NewObject();
    object6["data1"] = "012345\n_";
    object6.Save();
}

// Query.データ【D】登録
static void RegisterDataD(NbObjectBucket &object_bucket) {
    NbObject object1 = object_bucket.NewObject();
    object1["name"] = "AAA";
    object1["number"] = 1;
    object1["telno"]["home"] = "04400000001";
    object1["telno"]["mobile"] = "09000000001";
    object1.Save();
    NbObject object2 = object_bucket.NewObject();
    object2["name"] = "BBB";
    object2["number"] = 2;
    object2["telno"]["home"] = "04400000001";
    object2["telno"]["mobile"] = "09000000002";
    object2.Save();
}

// Query.データ200件登録
static void RegisterData200(NbObjectBucket &object_bucket) {
    for (int i = 0; i < 200; ++i) {
        NbObject object = object_bucket.NewObject();
        object["data1"] = 100 + (i * 5);
        object.Save();
    }
}

// Query.データ10000件登録
static void RegisterData10000(NbObjectBucket &object_bucket) {
    for (int i = 0; i < 10000; ++i) {
        NbObject object = object_bucket.NewObject();
        object["data1"] = 1;
        object.Save();
    }
}

// 性能評価.データ(1KB)
static const string kTestData1KB = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyzABC";

// 性能評価.データ(1KB)10000件登録
static void RegisterData1KB10000(NbObjectBucket &object_bucket) {
    for (int i = 0; i < 10000; ++i) {
        NbObject object = object_bucket.NewObject();
        object["DATA"] = kTestData1KB;
        int num = 1 + i;
        std::ostringstream id_org;
        id_org << std::setfill('0') << std::setw(5) << num;
        object["DATA_ID"] = id_org.str();
        NbResult<NbObject> result = object.Save();
        NbJsonObject response = result.GetSuccessData();
    }
}

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

// オブジェクト作成.新規作成
// 未ログイン、ACL指定なし
TEST_F(NbObjectFT, SaveNewNotLoggedIn) {
    // ログイン状態確認
    ASSERT_FALSE(NbUser::IsLoggedIn(service_));

    NbJsonObject json(R"({"r":["g:anonymous"],"w":["g:anonymous"],"c":[],"u":[],"d":[],"admin":[]})");
    NbAcl expected_acl(json);

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

// オブジェクト作成.新規作成
// 未ログイン、ACL指定あり
TEST_F(NbObjectFT, SaveNewNotLoggedInAcl) {
    // ログイン状態確認
    ASSERT_FALSE(NbUser::IsLoggedIn(service_));

    NbAcl expected_acl = NbAcl::CreateAclForAuthenticated();
    NbObjectBucket object_bucket(service_, kObjectBucketName); 
    NbObject object = object_bucket.NewObject();
    object.SetAcl(expected_acl);
    object["testKey"] = "testValue";
    NbResult<NbObject> result = object.Save(true);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("testValue"), response.GetString("testKey"));
    FTUtil::CompareAcl(expected_acl, response.GetAcl());

    FTUtil::CompareObject(object, response);
}

// オブジェクト作成.マスターキー
// バケットcontentACLのcreate権限を空欄、AppKeyにマスターキーを設定
TEST_F(NbObjectFT, SaveNewMaster) {
    NbObjectBucket object_bucket(service_masterkey_, kObjectBucketName);

    // ObjectバケットACL更新
    string content_acl = R"({"r":["g:anonymous"],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("object", kObjectBucketName, "", content_acl);

    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    NbResult<NbObject> result = object.Save();
 
    // バケット初期化
    FTUtil::CreateBucket("object", "objectBucket");

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("testValue"), response.GetString("testKey"));
    EXPECT_FALSE(response.GetObjectId().empty());
    EXPECT_NE(0, response.GetCreatedTime().tm_year);
    EXPECT_NE(0, response.GetUpdatedTime().tm_year);
    EXPECT_FALSE(response.GetETag().empty());

    FTUtil::CompareObject(object, response);
}

// オブジェクト作成.権限エラー
// バケットcontentACLのcreate権限を空欄
TEST_F(NbObjectFT, SaveNewForbidden) {
    NbObjectBucket object_bucket(service_, kObjectBucketName); 

    // ObjectバケットACL更新
    string content_acl = R"({"r":["g:anonymous"],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("object", kObjectBucketName, "", content_acl);

    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    NbResult<NbObject> result = object.Save();

    // バケット初期化
    FTUtil::CreateBucket("object", "objectBucket");

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());

    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(403, result.GetRestError().status_code);
    NbJsonObject json2(rest_error.reason);
    EXPECT_TRUE(json2.IsMember("error"));
}

// オブジェクト作成.新規作成
// パス不正、バケット名に不正値を用いる
TEST_F(NbObjectFT, SaveNewInvalidBucketName) {
    NbObjectBucket object_bucket(service_, "Invalid_objectBucket");
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    NbResult<NbObject> result = object.Save();
 
    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));
}

// オブジェクト作成.新規作成
// バケット名が空文字
TEST_F(NbObjectFT, SaveNewNoBucketName) {
    NbObjectBucket object_bucket(service_, "");
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

// オブジェクトの読み込.ID指定
// 削除マークなし、データに削除マークなし
TEST_F(NbObjectFT, GetObjectNoDeleteMark) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    object.Save();

    NbResult<NbObject> result = object_bucket.GetObject(object.GetObjectId());

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    FTUtil::CompareObject(object, response);
}

// オブジェクトの読み込.ID指定
// 削除マークなし、データに削除マークあり
TEST_F(NbObjectFT, GetObjectDeleteMark) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    object.Save();
    object.DeleteObject(true);

    NbResult<NbObject> result = object_bucket.GetObject(object.GetObjectId());

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));
}

// オブジェクトの読み込.ID指定
// 削除マークあり、データに削除マークあり
TEST_F(NbObjectFT, GetObject) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    object.Save();
    object.DeleteObject(true);

    NbResult<NbObject> result = object_bucket.GetObject(object.GetObjectId(), true);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    FTUtil::CompareObject(object, response);
}

// オブジェクトの読み込.該当オブジェクトなし
TEST_F(NbObjectFT, GetObjectNoObject) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbResult<NbObject> result = object_bucket.GetObject("Invalid_ObjectId");

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));
}

// オブジェクトの読み込み.マスターキー
// バケットcontentACLのread権限を空欄、オブジェクトデータのACL read権限を空欄、AppKeyにマスターキーを設定
TEST_F(NbObjectFT, GetObjectMaster) {
    NbObjectBucket object_bucket(service_masterkey_, kObjectBucketName); 

    // ObjectバケットACL更新
    string content_acl = R"({"r":[],"w":["g:anonymous"],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("object", kObjectBucketName, "", content_acl);

    // ObjectデータACL設定
    NbJsonObject json(R"({"r":[],"w":["g:anonymous"],"c":["g:anonymous"],"u":[],"d":[],"admin":[]})");
    NbAcl expected_acl(json);

    NbObject object = object_bucket.NewObject();
    object.SetAcl(expected_acl);
    object["testKey"] = "testValue";
    object.Save(true);

    NbResult<NbObject> result = object_bucket.GetObject(object.GetObjectId());

    // バケット初期化
    FTUtil::CreateBucket("object", "objectBucket");

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    FTUtil::CompareObject(object, response);
}

// オブジェクトの読み込み.権限エラー(オブジェクト)
// オブジェクトデータのACL read権限を空欄
TEST_F(NbObjectFT, GetObjectForbidden) {
    NbJsonObject json(R"({"r":[],"w":["g:anonymous"],"c":["g:anonymous"],"u":[],"d":[],"admin":[]})");
    NbAcl expected_acl(json);

    NbObjectBucket object_bucket(service_, kObjectBucketName); 

    NbObject object = object_bucket.NewObject();
    object.SetAcl(expected_acl);
    object["testKey"] = "testValue";
    object.Save(true);

    NbResult<NbObject> result = object_bucket.GetObject(object.GetObjectId());

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(403, result.GetRestError().status_code);
    NbJsonObject json2(rest_error.reason);
    EXPECT_TRUE(json2.IsMember("error"));
}

// オブジェクトの読み込.パス不正
TEST_F(NbObjectFT, GetObjectDifferentBucket) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbObjectBucket object_bucket2(service_, "bucket2");

    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    NbResult<NbObject> result0 = object.Save();

    NbResult<NbObject> result = object_bucket2.GetObject(object.GetObjectId());

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));
}

// オブジェクトの読み込み.オブジェクトIDが空文字
TEST_F(NbObjectFT, GetObjectNoObjectId) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbResult<NbObject> result = object_bucket.GetObject("");

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

// オブジェクトの読み込み.バケット名が空文字
TEST_F(NbObjectFT, GetObjectNoBucketName) {
    NbObjectBucket object_bucket(service_, "");

    NbResult<NbObject> result = object_bucket.GetObject("Temporary_ObjectId");

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

// オブジェクトのクエリ.全件クエリ、カウントあり
// 空のバケット
TEST_F(NbObjectFT, Query) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_TRUE(response.empty());
    EXPECT_EQ(0, count);
}

// オブジェクトのクエリ.全件クエリ、カウントあり
TEST_F(NbObjectFT, Query10) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    RegisterDataA(object_bucket);

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(10, count);
}

// オブジェクトのクエリ.全件クエリ、カウントなし
TEST_F(NbObjectFT, Query10NoCount) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;

    RegisterDataA(object_bucket);

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();

    std::vector<int> response2;

    for(int i = 0; i < 10; ++i){
        response2.push_back(response[i].GetInt("data1"));
    }
    std::vector<int> response_org{100, 105, 110, 115, 120, 125, 130, 135, 140, 145};

    EXPECT_EQ(10, response.size());
    FTUtil::CompareList(response_org, response2);
}

// オブジェクトのクエリ.queryが初期状態(limit未設定検証)
TEST_F(NbObjectFT, Query200NotLimitSet) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    RegisterData200(object_bucket);

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(200, count);

    EXPECT_EQ(100, response.size());
}

// オブジェクトのクエリ.queryが初期状態
TEST_F(NbObjectFT, Query10NotLimitSet) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    NbObject object1 = object_bucket.NewObject();
    object1["data1"] = 10;
    object1.Save();

    RegisterDataA(object_bucket);

    object1.DeleteObject(true);

    query.OrderBy(std::vector<std::string>{"data1"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(10, count);

    EXPECT_EQ(10, response.size());
    EXPECT_EQ(100, response[0].GetInt("data1"));
    EXPECT_EQ(105, response[1].GetInt("data1"));
    EXPECT_EQ(110, response[2].GetInt("data1"));
    EXPECT_EQ(115, response[3].GetInt("data1"));
    EXPECT_EQ(120, response[4].GetInt("data1"));
    EXPECT_EQ(125, response[5].GetInt("data1"));
    EXPECT_EQ(130, response[6].GetInt("data1"));
    EXPECT_EQ(135, response[7].GetInt("data1"));
    EXPECT_EQ(140, response[8].GetInt("data1"));
    EXPECT_EQ(145, response[9].GetInt("data1"));
}

// オブジェクトのクエリ.ID検索
TEST_F(NbObjectFT, QueryObjectID) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    NbObject object1 = object_bucket.NewObject();
    object1.Save();

    RegisterDataA(object_bucket);

    query.EqualTo("_id", object1.GetObjectId());
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();

    EXPECT_EQ(1, count);

    EXPECT_EQ(1, response.size());
    EXPECT_EQ(object1.GetObjectId(), response[0].GetObjectId());
}

// オブジェクトのクエリ.limit
TEST_F(NbObjectFT, QueryLimit5) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    RegisterDataA(object_bucket);

    query.Limit(5);
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(10, count);

    EXPECT_EQ(5, response.size());
}

// オブジェクトのクエリ.limit = -1を設定
TEST_F(NbObjectFT, QueryLimitMinus1) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    RegisterData200(object_bucket);

    query.Limit(-1);
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(200, count);

    EXPECT_EQ(200, response.size());
}

// オブジェクトのクエリ.limitに-1未満の負数を設定
TEST_F(NbObjectFT, QueryLimitMinus2) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    RegisterData200(object_bucket);

    query.Limit(-2);
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(200, count);

    EXPECT_EQ(100, response.size());
}

// オブジェクトのクエリ.limitに100を設定
TEST_F(NbObjectFT, QueryLimit100) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    RegisterData200(object_bucket);

    query.Limit(100);
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(200, count);

    EXPECT_EQ(100, response.size());
}

// オブジェクトのクエリ.limitに101を設定
TEST_F(NbObjectFT, QueryLimit101) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    RegisterData200(object_bucket);

    query.Limit(101);
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(200, count);

    EXPECT_EQ(100, response.size());
}

// オブジェクトのクエリ.skip 
TEST_F(NbObjectFT, QuerySkip8) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    RegisterDataA(object_bucket);

    query.Skip(8);
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(10, count);

    EXPECT_EQ(2, response.size());
}

// オブジェクトのクエリ.skipに負の値を設定
TEST_F(NbObjectFT, QuerySkipMinus1) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    RegisterDataA(object_bucket);

    query.Skip(-1);
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(10, count);

    EXPECT_EQ(10, response.size());
}

// オブジェクトのクエリ.order単体キー指定
TEST_F(NbObjectFT, QueryOrderSingleKey) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    RegisterDataA(object_bucket);

    query.OrderBy(std::vector<std::string>{"-data1"});
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(10, count);

    EXPECT_EQ(10, response.size());
    EXPECT_EQ(100, response[9].GetInt("data1"));
    EXPECT_EQ(105, response[8].GetInt("data1"));
    EXPECT_EQ(110, response[7].GetInt("data1"));
    EXPECT_EQ(115, response[6].GetInt("data1"));
    EXPECT_EQ(120, response[5].GetInt("data1"));
    EXPECT_EQ(125, response[4].GetInt("data1"));
    EXPECT_EQ(130, response[3].GetInt("data1"));
    EXPECT_EQ(135, response[2].GetInt("data1"));
    EXPECT_EQ(140, response[1].GetInt("data1"));
    EXPECT_EQ(145, response[0].GetInt("data1"));
}

// オブジェクトのクエリ.order複合キー指定
TEST_F(NbObjectFT, QueryOrderMultiplexKey) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    for (int i = 0; i < 3; ++i) {
        NbObject object1 = object_bucket.NewObject();
        object1["data1"] = 1;
        object1["data2"] = 1 + i;
        object1.Save();
    }

    for (int i = 0; i < 3; ++i) {
        NbObject object2 = object_bucket.NewObject();
        object2["data1"] = 2;
        object2["data2"] = 1 + i;
        object2.Save();
    }

    for (int i = 0; i < 4; ++i) {
        NbObject object3 = object_bucket.NewObject();
        object3["data1"] = 3;
        object3["data2"] = 1 + i;
        object3.Save();
    }

    query.OrderBy(std::vector<std::string>{"data2", "-data1"});
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(10, count);

    EXPECT_EQ(10, response.size());
    EXPECT_EQ(3, response[0].GetInt("data1"));
    EXPECT_EQ(1, response[0].GetInt("data2"));
    EXPECT_EQ(2, response[1].GetInt("data1"));
    EXPECT_EQ(1, response[1].GetInt("data2"));
    EXPECT_EQ(1, response[2].GetInt("data1"));
    EXPECT_EQ(1, response[2].GetInt("data2"));
    EXPECT_EQ(3, response[3].GetInt("data1"));
    EXPECT_EQ(2, response[3].GetInt("data2"));
    EXPECT_EQ(2, response[4].GetInt("data1"));
    EXPECT_EQ(2, response[4].GetInt("data2"));
    EXPECT_EQ(1, response[5].GetInt("data1"));
    EXPECT_EQ(2, response[5].GetInt("data2"));
    EXPECT_EQ(3, response[6].GetInt("data1"));
    EXPECT_EQ(3, response[6].GetInt("data2"));
    EXPECT_EQ(2, response[7].GetInt("data1"));
    EXPECT_EQ(3, response[7].GetInt("data2"));
    EXPECT_EQ(1, response[8].GetInt("data1"));
    EXPECT_EQ(3, response[8].GetInt("data2"));
    EXPECT_EQ(3, response[9].GetInt("data1"));
    EXPECT_EQ(4, response[9].GetInt("data2"));
}

// オブジェクトのクエリ.limit + skip + order + count + projection
TEST_F(NbObjectFT, QueryExcludeWhereOption) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["data1"] = 121;
    object["data2"] = 0;
    NbResult<NbObject> save = object.Save();

    RegisterDataA(object_bucket);

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

// オブジェクトのクエリ.where - （equals)
TEST_F(NbObjectFT, QueryWhereOptionEquals) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query;
    int count;

    query.EqualTo("data1", 120);
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(1, count);

    EXPECT_EQ(1, response.size());
    EXPECT_EQ(120, response[0].GetInt("data1"));
}

// オブジェクトのクエリ.where - $eq
TEST_F(NbObjectFT, QueryWhereOptionEq) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbObject object = object_bucket.NewObject();
    object["data1"] = 1;
    object["data2"]["data3"] = 3;
    object.Save();

    NbQuery query;
    int count;

    NbJsonObject json_data;
    json_data["data3"] = 3;

    query.EqualTo("data2", json_data);
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();

    EXPECT_EQ(1, count);
    EXPECT_EQ(1, response.size());
    EXPECT_EQ(object, response[0]);
}

// オブジェクトのクエリ.where - $ne
TEST_F(NbObjectFT, QueryWhereOptionNe) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query;
    int count;

    query.NotEquals("data1", 120)
         .OrderBy(std::vector<std::string>{"data1"});
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    int response_data1 = response[0].GetInt("data1");

    EXPECT_EQ(10, count);

    EXPECT_EQ(10, response.size());
    EXPECT_EQ(100, response[0].GetInt("data2"));
    EXPECT_EQ(R"([100,101,102])", response[1].GetJsonArray("data1").ToJsonString());
    EXPECT_EQ(R"([145,101,102])", response[2].GetJsonArray("data1").ToJsonString());
    EXPECT_EQ(105, response[3].GetInt("data1"));
    EXPECT_EQ(110, response[4].GetInt("data1"));
    EXPECT_EQ(115, response[5].GetInt("data1"));
    EXPECT_EQ(125, response[6].GetInt("data1"));
    EXPECT_EQ(130, response[7].GetInt("data1"));
    EXPECT_EQ(135, response[8].GetInt("data1"));
    EXPECT_EQ(140, response[9].GetInt("data1"));
}

// オブジェクトのクエリ.where - $lt
TEST_F(NbObjectFT, QueryWhereOptionLt) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query;
    int count;

    query.LessThan("data1", 120)
         .OrderBy(std::vector<std::string>{"data1"});
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(5, count);

    EXPECT_EQ(5, response.size());
    EXPECT_EQ(R"([100,101,102])", response[0].GetJsonArray("data1").ToJsonString());
    EXPECT_EQ(R"([145,101,102])", response[1].GetJsonArray("data1").ToJsonString());
    EXPECT_EQ(105, response[2].GetInt("data1"));
    EXPECT_EQ(110, response[3].GetInt("data1"));
    EXPECT_EQ(115, response[4].GetInt("data1"));
}

// オブジェクトのクエリ.where - $lte
TEST_F(NbObjectFT, QueryWhereOptionLte) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query;
    int count;

    query.LessThanOrEqual("data1", 120)
         .OrderBy(std::vector<std::string>{"data1"});
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(6, count);

    EXPECT_EQ(6, response.size());
    EXPECT_EQ(R"([100,101,102])", response[0].GetJsonArray("data1").ToJsonString());
    EXPECT_EQ(R"([145,101,102])", response[1].GetJsonArray("data1").ToJsonString());
    EXPECT_EQ(105, response[2].GetInt("data1"));
    EXPECT_EQ(110, response[3].GetInt("data1"));
    EXPECT_EQ(115, response[4].GetInt("data1"));
    EXPECT_EQ(120, response[5].GetInt("data1"));
}

// オブジェクトのクエリ.where - $gt
TEST_F(NbObjectFT, QueryWhereOptionGt) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query;
    int count;

    query.GreaterThan("data1", 120)
         .OrderBy(std::vector<std::string>{"_id"});
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(5, count);

    EXPECT_EQ(5, response.size());
    EXPECT_EQ(125, response[0].GetInt("data1"));
    EXPECT_EQ(130, response[1].GetInt("data1"));
    EXPECT_EQ(135, response[2].GetInt("data1"));
    EXPECT_EQ(140, response[3].GetInt("data1"));
    EXPECT_EQ(R"([145,101,102])", response[4].GetJsonArray("data1").ToJsonString());
}

// オブジェクトのクエリ.where - $gte
TEST_F(NbObjectFT, QueryWhereOptionGte) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query;
    int count;

    query.GreaterThanOrEqual("data1", 120)
         .OrderBy(std::vector<std::string>{"_id"});
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(6, count);

    EXPECT_EQ(6, response.size());
    EXPECT_EQ(120, response[0].GetInt("data1"));
    EXPECT_EQ(125, response[1].GetInt("data1"));
    EXPECT_EQ(130, response[2].GetInt("data1"));
    EXPECT_EQ(135, response[3].GetInt("data1"));
    EXPECT_EQ(140, response[4].GetInt("data1"));
    EXPECT_EQ(R"([145,101,102])", response[5].GetJsonArray("data1").ToJsonString());
}

// オブジェクトのクエリ.where - $in
TEST_F(NbObjectFT, QueryWhereOptionIn) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query;
    int count;

    NbJsonArray jsonArrayValue(R"([120, 130, 135, 150])");
    query.In("data1", jsonArrayValue)
         .OrderBy(std::vector<std::string>{"data1"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(3, count);

    EXPECT_EQ(3, response.size());
    EXPECT_EQ(120, response[0].GetInt("data1"));
    EXPECT_EQ(130, response[1].GetInt("data1"));
    EXPECT_EQ(135, response[2].GetInt("data1"));
}

// オブジェクトのクエリ.where - $all
TEST_F(NbObjectFT, QueryWhereOptionAll) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query;
    int count;

    NbJsonArray jsonArrayValue(R"([101, 102])");
    query.All("data1", jsonArrayValue)
         .OrderBy(std::vector<std::string>{"data1"});
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(2, count);

    EXPECT_EQ(2, response.size());
    EXPECT_EQ(R"([100,101,102])", response[0].GetJsonArray("data1").ToJsonString());
    EXPECT_EQ(R"([145,101,102])", response[1].GetJsonArray("data1").ToJsonString());
}

// オブジェクトのクエリ.where - $exists (false)
TEST_F(NbObjectFT, QueryWhereOptionExistsFalse) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query;
    int count;

    query.NotExists("data1");
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(1, count);

    EXPECT_EQ(1, response.size());
    EXPECT_EQ(100, response[0].GetInt("data2"));
}

// オブジェクトのクエリ.where - $exists (true)
TEST_F(NbObjectFT, QueryWhereOptionExistsTrue) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query;
    int count;

    query.Exists("data1")
         .OrderBy(std::vector<std::string>{"data1"});
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(10, count);

    EXPECT_EQ(10, response.size());
    EXPECT_EQ(R"([100,101,102])", response[0].GetJsonArray("data1").ToJsonString());
    EXPECT_EQ(R"([145,101,102])", response[1].GetJsonArray("data1").ToJsonString());
    EXPECT_EQ(105, response[2].GetInt("data1"));
    EXPECT_EQ(110, response[3].GetInt("data1"));
    EXPECT_EQ(115, response[4].GetInt("data1"));
    EXPECT_EQ(120, response[5].GetInt("data1"));
    EXPECT_EQ(125, response[6].GetInt("data1"));
    EXPECT_EQ(130, response[7].GetInt("data1"));
    EXPECT_EQ(135, response[8].GetInt("data1"));
    EXPECT_EQ(140, response[9].GetInt("data1"));
}

// オブジェクトのクエリ.where - $not
TEST_F(NbObjectFT, QueryWhereOptionNot) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query;
    int count;

    query.Exists("data1")
         .Not("data1");
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(1, count);

    EXPECT_EQ(1, response.size());
    EXPECT_EQ(100, response[0].GetInt("data2"));
}

// オブジェクトのクエリ.where - $or
TEST_F(NbObjectFT, QueryWhereOptionOr) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query1, query2;
    int count;

    NbJsonArray jsonArrayValue1(R"([120, 130, 135, 150])");
    NbJsonArray jsonArrayValue2(R"([101, 102])");

    query1.In("data1", jsonArrayValue1);
    query2.All("data1", jsonArrayValue2);

    std::vector<NbQuery> queries{query2};

    query1.Or(queries)
          .OrderBy(std::vector<std::string>{"data1"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query1, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(5, count);

    EXPECT_EQ(5, response.size());
    EXPECT_EQ(R"([100,101,102])", response[0].GetJsonArray("data1").ToJsonString());
    EXPECT_EQ(R"([145,101,102])", response[1].GetJsonArray("data1").ToJsonString());
    EXPECT_EQ(120, response[2].GetInt("data1"));
    EXPECT_EQ(130, response[3].GetInt("data1"));
    EXPECT_EQ(135, response[4].GetInt("data1"));

    // 追加確認（条件未設定のQueryにOR条件を設定）
    NbQuery query = NbQuery().Or(queries)
                             .OrderBy(std::vector<std::string>{"data1"});
    result = object_bucket.Query(query, &count);
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(2, count);

    EXPECT_EQ(2, response.size());
    EXPECT_EQ(R"([100,101,102])", response[0].GetJsonArray("data1").ToJsonString());
    EXPECT_EQ(R"([145,101,102])", response[1].GetJsonArray("data1").ToJsonString());
}

// オブジェクトのクエリ.where - $and
TEST_F(NbObjectFT, QueryWhereOptionAnd) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataB(object_bucket);

    NbQuery query1, query2, query3;
    int count;

    NbJsonArray jsonArrayValue(R"([120, 130, 135, 150])");


    query1.LessThanOrEqual("data1", 120);
    query2.GreaterThanOrEqual("data1", 120);
    query3.In("data1", jsonArrayValue);

    std::vector<NbQuery> queries;
    queries.push_back(query2);
    queries.push_back(query3);

    query1.And(queries);

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query1, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(1, count);

    EXPECT_EQ(1, response.size());
    EXPECT_EQ(120, response[0].GetInt("data1"));

    // 追加確認（条件未設定のQueryにAND条件を設定）
    NbQuery query = NbQuery().And(std::vector<NbQuery>{query2})
                             .OrderBy(std::vector<std::string>{"_id"});
    result = object_bucket.Query(query, &count);
    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();
    EXPECT_EQ(6, count);

    EXPECT_EQ(6, response.size());
    EXPECT_EQ(120, response[0].GetInt("data1"));
    EXPECT_EQ(125, response[1].GetInt("data1"));
    EXPECT_EQ(130, response[2].GetInt("data1"));
    EXPECT_EQ(135, response[3].GetInt("data1"));
    EXPECT_EQ(140, response[4].GetInt("data1"));
    EXPECT_EQ(R"([145,101,102])", response[5].GetJsonArray("data1").ToJsonString());
}

// オブジェクトのクエリ.where - $regex(INSENSITIVITY)
TEST_F(NbObjectFT, QueryWhereOptionRegexI) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataC(object_bucket);

    NbQuery query;
    int count;

    query.Regex("data1", ".*abc.*", "i")
         .OrderBy(std::vector<std::string>{"data1"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(4, count);

    EXPECT_EQ(4, response.size());
    EXPECT_EQ("0123ABC", response[0].GetString("data1"));
    EXPECT_EQ("0123abc", response[1].GetString("data1"));
    EXPECT_EQ("ABCDEFG", response[2].GetString("data1"));
    EXPECT_EQ("abcdefg", response[3].GetString("data1"));
}

// オブジェクトのクエリ.where - $regex(MULTILINE)
TEST_F(NbObjectFT, QueryWhereOptionRegexM) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataC(object_bucket);

    NbQuery query;
    int count;

    query.Regex("data1", "^_.*", "m")
         .OrderBy(std::vector<std::string>{"data1"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(2, count);

    EXPECT_EQ(2, response.size());
    EXPECT_EQ("012345\n_", response[0].GetString("data1"));
    EXPECT_EQ("_012345_", response[1].GetString("data1"));
}

// オブジェクトのクエリ.where - $regex(DOT_MULTILINE)
TEST_F(NbObjectFT, QueryWhereOptionRegexDM) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataC(object_bucket);

    NbObject object = object_bucket.NewObject();
    object["data1"] = "_0123456_";
    object.Save();

    NbQuery query;
    int count;

    query.Regex("data1", ".*5._", "s")
         .OrderBy(std::vector<std::string>{"data1"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(2, count);

    EXPECT_EQ(2, response.size());
    EXPECT_EQ("012345\n_", response[0].GetString("data1"));
    EXPECT_EQ("_0123456_", response[1].GetString("data1"));
}

// オブジェクトのクエリ.where - $regex(EXTENDED)
TEST_F(NbObjectFT, QueryWhereOptionRegexE) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataC(object_bucket);

    NbObject object = object_bucket.NewObject();
    object["data1"] = "_0123456_";
    object.Save();

    NbQuery query;
    int count;

    query.Regex("data1", ".*5._  # comment\n", "x");

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(1, count);

    EXPECT_EQ(1, response.size());
    EXPECT_EQ("_0123456_", response[0].GetString("data1"));
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

// オブジェクトのクエリ.deleteMark
TEST_F(NbObjectFT, QueryDeletemark) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbObject object = object_bucket.NewObject();
    object["data1"] = 10;
    object.Save();
    NbResult<NbObject> result_delete = object.DeleteObject(true);

    RegisterDataA(object_bucket);

    NbQuery query;
    int count;

    query.DeleteMark(true)
         .OrderBy(std::vector<std::string>{"data1"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(11, count);

    EXPECT_EQ(11, response.size());
    EXPECT_EQ(10, response[0].GetInt("data1"));
    EXPECT_TRUE(response[0].IsDeleteMark());
}

// オブジェクトのクエリ.projection（特定フィールドのみ取得）
TEST_F(NbObjectFT, QueryProjectionSpecificField) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataD(object_bucket);

    NbQuery query;
    int count;

    std::map<std::string, bool> map;
    map["name"] = true;
    map["number"] = true;
    query.Projection(map)
         .OrderBy(std::vector<std::string>{"name"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(2, count);

    EXPECT_EQ(2, response.size());

    EXPECT_EQ("AAA", response[0].GetString("name"));
    EXPECT_EQ(1, response[0].GetInt("number"));
    EXPECT_FALSE(response[0].GetObjectId().empty());
    EXPECT_FALSE(response[0].IsMember("telno"));
    EXPECT_TRUE(response[0].GetETag().empty());

    EXPECT_EQ("BBB", response[1].GetString("name"));
    EXPECT_EQ(2, response[1].GetInt("number"));
    EXPECT_FALSE(response[1].GetObjectId().empty());
    EXPECT_FALSE(response[1].IsMember("telno"));
    EXPECT_TRUE(response[1].GetETag().empty());
}

// オブジェクトのクエリ.projection（特定フィールドのみ抑制）
TEST_F(NbObjectFT, QueryProjectionSpecificFieldSuppression) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataD(object_bucket);

    NbQuery query;
    int count;

    std::map<std::string, bool> map;
    map["telno"] = false;
    query.Projection(map)
         .OrderBy(std::vector<std::string>{"name"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(2, count);

    EXPECT_EQ(2, response.size());

    EXPECT_EQ("AAA", response[0].GetString("name"));
    EXPECT_EQ(1, response[0].GetInt("number"));
    EXPECT_FALSE(response[0].GetObjectId().empty());
    EXPECT_FALSE(response[0].IsMember("telno"));
    EXPECT_FALSE(response[0].GetETag().empty());
    
    EXPECT_EQ("BBB", response[1].GetString("name"));
    EXPECT_EQ(2, response[1].GetInt("number"));
    EXPECT_FALSE(response[1].GetObjectId().empty());
    EXPECT_FALSE(response[1].IsMember("telno"));
    EXPECT_FALSE(response[1].GetETag().empty());
}

// オブジェクトのクエリ.projection（"_id"フィールドのみ抑制）
TEST_F(NbObjectFT, QueryProjectionIdFieldSuppression) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataD(object_bucket);

    NbQuery query;
    int count;

    std::map<std::string, bool> map;
    map["_id"] = false;
    query.Projection(map)
         .OrderBy(std::vector<std::string>{"name"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(2, count);

    EXPECT_EQ(2, response.size());

    EXPECT_EQ("AAA", response[0].GetString("name"));
    EXPECT_EQ(1, response[0].GetInt("number"));
    EXPECT_TRUE(response[0].GetObjectId().empty());
    EXPECT_EQ("04400000001", response[0].GetJsonObject("telno").GetString("home"));
    EXPECT_EQ("09000000001", response[0].GetJsonObject("telno").GetString("mobile"));
    EXPECT_FALSE(response[0].GetETag().empty());

    EXPECT_EQ("BBB", response[1].GetString("name"));
    EXPECT_EQ(2, response[1].GetInt("number"));
    EXPECT_TRUE(response[1].GetObjectId().empty());
    EXPECT_EQ("04400000001", response[1].GetJsonObject("telno").GetString("home"));
    EXPECT_EQ("09000000002", response[1].GetJsonObject("telno").GetString("mobile"));
    EXPECT_FALSE(response[1].GetETag().empty());
}

// オブジェクトのクエリ.projection（入れ子構造）
TEST_F(NbObjectFT, QueryProjectionNestedStructure) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataD(object_bucket);

    NbQuery query;
    int count;

    std::map<std::string, bool> map;
    map["telno"] = true;
    map["telno.home"] = true;
    query.Projection(map)
         .OrderBy(std::vector<std::string>{"name"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(2, count);

    EXPECT_EQ(2, response.size());

    EXPECT_FALSE(response[0].IsMember("name"));
    EXPECT_FALSE(response[0].IsMember("number"));
    EXPECT_FALSE(response[0].GetObjectId().empty());
    EXPECT_EQ("04400000001", response[0].GetJsonObject("telno").GetString("home"));
    EXPECT_EQ("", response[0].GetJsonObject("telno").GetString("mobile"));
    EXPECT_TRUE(response[0].GetETag().empty());

    EXPECT_FALSE(response[1].IsMember("name"));
    EXPECT_FALSE(response[1].IsMember("number"));
    EXPECT_FALSE(response[1].GetObjectId().empty());
    EXPECT_EQ("04400000001", response[1].GetJsonObject("telno").GetString("home"));
    EXPECT_EQ("", response[1].GetJsonObject("telno").GetString("mobile"));
    EXPECT_TRUE(response[1].GetETag().empty());
}

// オブジェクトのクエリ.projection（取得と抑制の混在、"_id"フィールドを抑制）
TEST_F(NbObjectFT, QueryProjectionMixIdFieldSuppression) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataD(object_bucket);

    NbQuery query;
    int count;

    std::map<std::string, bool> map;
    map["name"] = true;
    map["_id"] = false;
    query.Projection(map)
         .OrderBy(std::vector<std::string>{"name"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(2, count);

    EXPECT_EQ(2, response.size());

    EXPECT_EQ("AAA", response[0].GetString("name"));
    EXPECT_FALSE(response[0].IsMember("number"));
    EXPECT_TRUE(response[0].GetObjectId().empty());
    EXPECT_FALSE(response[0].IsMember("telno"));
    EXPECT_TRUE(response[0].GetETag().empty());

    EXPECT_EQ("BBB", response[1].GetString("name"));
    EXPECT_FALSE(response[1].IsMember("number"));
    EXPECT_TRUE(response[1].GetObjectId().empty());
    EXPECT_FALSE(response[1].IsMember("telno"));
    EXPECT_TRUE(response[1].GetETag().empty());
}

// オブジェクトのクエリ.projection（存在しないキーを設定）
TEST_F(NbObjectFT, QueryProjectionNoKey) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataD(object_bucket);

    NbQuery query;
    int count;

    std::map<std::string, bool> map;
    map["name"] = true;
    map["aaa"] = true;
    query.Projection(map)
         .OrderBy(std::vector<std::string>{"name"});

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(2, count);

    EXPECT_EQ(2, response.size());

    EXPECT_EQ("AAA", response[0].GetString("name"));
    EXPECT_FALSE(response[0].IsMember("number"));
    EXPECT_FALSE(response[0].GetObjectId().empty());
    EXPECT_FALSE(response[0].IsMember("telno"));
    EXPECT_TRUE(response[0].GetETag().empty());

    EXPECT_EQ("BBB", response[1].GetString("name"));
    EXPECT_FALSE(response[1].IsMember("number"));
    EXPECT_FALSE(response[1].GetObjectId().empty());
    EXPECT_FALSE(response[1].IsMember("telno"));
    EXPECT_TRUE(response[1].GetETag().empty());
}

// オブジェクトのクエリ.projection（取得と抑制の混在）
TEST_F(NbObjectFT, QueryProjectionMix) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    RegisterDataD(object_bucket);
    
    NbQuery query;
    int count;

    std::map<std::string, bool> map;
    map["name"] = true;
    map["number"] = false;
    query.Projection(map);

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());

    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(500, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));
}

// オブジェクトのクエリ.参照する DB の指定(セカンダリ)
TEST_F(NbObjectFT, QueryReferenceDBSecondary) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    RegisterDataA(object_bucket);

    query.ReadPreference(NbReadPreference::SECONDARY_PREFERRED);

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(10, count);
    EXPECT_EQ(10, response.size());
}

// オブジェクトのクエリ.タイムアウト
TEST_F(NbObjectFT, QueryTimeoutSlowTest) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    RegisterData10000(object_bucket);

    query.EqualTo("NoKey", "NoData")
         .Timeout(1);
    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());

    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(504, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));
}

// オブジェクトのクエリ.マスターキー
TEST_F(NbObjectFT, QueryMaster) {
    NbObjectBucket object_bucket(service_masterkey_, kObjectBucketName);

    // ObjectバケットACL更新
    string content_acl = R"({"r":[],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("object", kObjectBucketName, "", content_acl);

    // ObjectデータACL設定
    NbJsonObject json(R"({"r":[],"w":["g:anonymous"],"c":["g:anonymous"],"u":[],"d":[],"admin":[]})");
    NbAcl expected_acl(json);

    NbObject object = object_bucket.NewObject();
    object.SetAcl(expected_acl);
    object["testKey"] = "testValue";
    object.Save(true);

    NbQuery query;
    int count;

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // バケット初期化
    FTUtil::CreateBucket("object", "objectBucket");

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(1, count);
    EXPECT_EQ(1, response.size());
    EXPECT_EQ("testValue", response[0].GetString("testKey"));
}

// オブジェクトのクエリ.権限エラー(バケット)
TEST_F(NbObjectFT, QueryForbiddenBucket) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    // ObjectバケットACL更新
    string content_acl = R"({"r":[],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("object", kObjectBucketName, "", content_acl);

    NbQuery query;
    int count;

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // バケット初期化
    FTUtil::CreateBucket("object", "objectBucket");

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());

    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(403, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));
}

// オブジェクトのクエリ.権限エラー(1オブジェクト)
TEST_F(NbObjectFT, QueryForbiddedObjectOne) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    // ObjectデータACL設定
    NbJsonObject json(R"({"r":[],"w":["g:anonymous"],"c":["g:anonymous"],"u":[],"d":[],"admin":[]})");
    NbAcl expected_acl(json);

    NbObject object1 = object_bucket.NewObject();
    object1["testKey1"] = "testValue1";
    object1.Save();

    NbObject object2 = object_bucket.NewObject();
    object2.SetAcl(expected_acl);
    object2["testKey2"] = "testValue2";
    object2.Save(true);

    NbQuery query;
    int count;

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(1, count);

    EXPECT_EQ(1, response.size());
    EXPECT_EQ("testValue1", response[0].GetString("testKey1"));
}

// オブジェクトのクエリ.権限エラー(全てのオブジェクト)
TEST_F(NbObjectFT, QueryForbiddedObjectAll) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    // ObjectデータACL設定
    NbJsonObject json(R"({"r":[],"w":["g:anonymous"],"c":["g:anonymous"],"u":[],"d":[],"admin":[]})");
    NbAcl expected_acl(json);

    NbObject object1 = object_bucket.NewObject();
    object1.SetAcl(expected_acl);
    object1["testKey1"] = "testValue1";
    object1.Save(true);

    NbObject object2 = object_bucket.NewObject();
    object2.SetAcl(expected_acl);
    object2["testKey2"] = "testValue2";
    object2.Save(true);

    NbQuery query;
    int count;

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(0, count);

    EXPECT_EQ(0, response.size());
}

// オブジェクトのクエリ.リクエストパラメータ誤り
TEST_F(NbObjectFT, QueryInvalid) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    NbJsonArray jsonArrayValue(R"([101, 102])");
    query.All("$data1", jsonArrayValue);

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());

    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(500, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));
}

// オブジェクトのクエリ.バケット名が空文字
TEST_F(NbObjectFT, QueryNoBacketName) {
    NbObjectBucket object_bucket(service_, "");

    NbQuery query;

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

// オブジェクト更新(部分更新)
// 予約フィールドなし
TEST_F(NbObjectFT, PartUpdateObject) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbAcl acl = NbAcl::CreateAclForAnonymous();
    object.SetAcl(acl);
    object["testKey"] = "testValue";
    NbResult<NbObject> save = object.Save(true);

    NbJsonObject json(R"({"UpdateKey": "UpdateValue"})");

    // 更新時間を変化させるためsleep
    sleep(1);
    NbObject original = object;
    NbResult<NbObject> result = object.PartUpdateObject(json);

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

// オブジェクト更新(部分更新)
// _id更新
TEST_F(NbObjectFT, PartUpdateObjectId) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.Save();

    NbJsonObject json(R"({"_id":"invalid"})");
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());

    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(500, result.GetRestError().status_code);
    NbJsonObject json2(rest_error.reason);
    EXPECT_TRUE(json2.IsMember("error"));
}

// オブジェクト更新(部分更新)
// ACL、作成日時、deleteMark更新
TEST_F(NbObjectFT, PartUpdateObjectACLCreatedAtDeleteMark) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(NbAcl::CreateAclForAnonymous());
    object.Save(true);

    NbJsonObject json;
    NbAcl acl;

    acl.AddEntry(NbAclPermission::WRITE ,"testUser");
    json.PutJsonObject("ACL", acl.ToJsonObject());
    json["createdAt"] = "1999-05-15T10:02:35.280Z";
    json["_deleted"] = true;
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    FTUtil::CompareTime(NbUtility::DateStringToTm("1999-05-15T10:02:35.280Z"), response.GetCreatedTime(), true);
    FTUtil::CompareAcl(acl, response.GetAcl());
    EXPECT_TRUE(response.IsDeleteMark());
}

// オブジェクト更新(部分更新)
// 更新日時、Etag更新
TEST_F(NbObjectFT, PartUpdateObjectUpdatedTimeEtag) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.Save();

    NbJsonObject json;

    // 更新時間を変化させるためsleep
    sleep(1);
    NbObject original = object;

    json["updatedAt"] = "1999-05-15T10:02:35.280Z";
    json["etag"] = "invalid";
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    FTUtil::CompareTime(NbUtility::DateStringToTm("1999-05-15T10:02:35.280Z"), response.GetUpdatedTime(), false);
    FTUtil::CompareTime(original.GetUpdatedTime(), response.GetUpdatedTime(), false);
    EXPECT_NE(original.GetETag(), response.GetETag());
}

// オブジェクトの更新（部分更新）.削除済みマークされたオブジェクトの部分更新
// 作成日時を変更
TEST_F(NbObjectFT, PartUpdateObjectDeleteMark) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbAcl acl1 = NbAcl::CreateAclForAnonymous();
    object.SetAcl(acl1);
    object["testKey"] = "testValue";
    object.Save(true);
    object.DeleteObject(true);

    NbJsonObject json(R"({"UpdateKey": "UpdateValue"})");
    json["createdAt"] = "1999-05-15T10:02:35.280Z";

    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("testValue"), response.GetString("testKey"));
    EXPECT_EQ(string("UpdateValue"), response.GetString("UpdateKey"));

    FTUtil::CompareTime(NbUtility::DateStringToTm("1999-05-15T10:02:35.280Z"), response.GetCreatedTime(), true);

    FTUtil::CompareAcl(acl1, response.GetAcl());
    EXPECT_TRUE(response.IsDeleteMark());
    FTUtil::CompareObject(object, response);
}

// オブジェクトの更新（部分更新）.オプションパラメータ(etag)未設定
TEST_F(NbObjectFT, PartUpdateObjectNotSetOptionParameter) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    object.Save();
        
    NbJsonObject json(R"({"UpdateKey": "UpdateValue"})");

    object.SetETag("");
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("testValue"), response.GetString("testKey"));
    EXPECT_EQ(string("UpdateValue"), response.GetString("UpdateKey"));

    FTUtil::CompareObject(object, response);
}

// オブジェクトの更新（部分更新）.$inc
TEST_F(NbObjectFT, PartUpdateObjectInc) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["Key1"] = 1;
    object.Save();

    NbJsonObject json1(R"({"Key1": 5})");
    NbJsonObject json2;
    json2.PutJsonObject("$inc", json1);

    NbResult<NbObject> result = object.PartUpdateObject(json2);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(6, response.GetInt("Key1"));

    FTUtil::CompareObject(object, response);
}

// オブジェクトの更新（部分更新）.$rename
TEST_F(NbObjectFT, PartUpdateObjectRename) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["Key1"] = 2;
    object.Save();

    NbJsonObject json1(R"({"Key1": "Key1_1"})");
    NbJsonObject json2;
    json2.PutJsonObject("$rename", json1);

    NbResult<NbObject> result = object.PartUpdateObject(json2);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(2, response.GetInt("Key1_1"));

    FTUtil::CompareObject(object, response);
}

// オブジェクトの更新（部分更新）.$set
TEST_F(NbObjectFT, PartUpdateObjectSet) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["Key1"] = 1;
    object.Save();

    NbJsonObject json1(R"({"Key1": 5})");
    NbJsonObject json2;
    json2.PutJsonObject("$set", json1);

    NbResult<NbObject> result = object.PartUpdateObject(json2);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(5, response.GetInt("Key1"));

    FTUtil::CompareObject(object, response);
}

// オブジェクトの更新（部分更新）.$unset
TEST_F(NbObjectFT, PartUpdateObjectUnset) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["Key1"] = 1;
    object.Save();

    NbJsonObject json1(R"({"Key1": ""})");
    NbJsonObject json2;
    json2.PutJsonObject("$unset", json1);

    NbResult<NbObject> result = object.PartUpdateObject(json2);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_FALSE(response.IsMember("Key1"));
}

// オブジェクトの更新（部分更新）.$addToSet
TEST_F(NbObjectFT, PartUpdateObjectAddToSet) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbJsonArray jsonArrayValue(R"(["electronics","camera"])");
    object.PutJsonArray("Key1", jsonArrayValue);
    object.Save();

    NbJsonObject json1(R"({"Key1": "accessories"})");
    NbJsonObject json2;
    json2.PutJsonObject("$addToSet", json1);

    NbResult<NbObject> result = object.PartUpdateObject(json2);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(R"(["electronics","camera","accessories"])", response.GetJsonArray("Key1").ToJsonString());
}

// オブジェクトの更新（部分更新）.$pop
TEST_F(NbObjectFT, PartUpdateObjectPop) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbJsonArray jsonArrayValue(R"([8,9,10])");
    object.PutJsonArray("Key1", jsonArrayValue);
    object.Save();

    NbJsonObject json1(R"({"Key1": -1})");
    NbJsonObject json2;
    json2.PutJsonObject("$pop", json1);

    NbResult<NbObject> result = object.PartUpdateObject(json2);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(R"([9,10])", response.GetJsonArray("Key1").ToJsonString());
}

// オブジェクトの更新（部分更新）.$pullAll
TEST_F(NbObjectFT, PartUpdateObjectPullAll) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbJsonArray jsonArrayValue(R"([0, 2, 5, 5, 1, 0])");
    object.PutJsonArray("Key1", jsonArrayValue);
    object.Save();

    NbJsonObject json1;
    NbJsonArray jsonArrayValue2(R"([0, 5])");
    json1.PutJsonArray("Key1", jsonArrayValue2);

    NbJsonObject json2;
    json2.PutJsonObject("$pullAll", json1);

    NbResult<NbObject> result = object.PartUpdateObject(json2);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(R"([2,1])", response.GetJsonArray("Key1").ToJsonString());
}

// オブジェクトの更新（部分更新）.$pull
TEST_F(NbObjectFT, PartUpdateObjectPull) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbJsonArray jsonArrayValue(R"(["vme", "de", "msr", "tsc", "pse", "msr"])");
    object.PutJsonArray("Key1", jsonArrayValue);
    object.Save();

    NbJsonObject json1(R"({"Key1":"msr"})");
    NbJsonObject json2;
    json2.PutJsonObject("$pull", json1);

    NbResult<NbObject> result = object.PartUpdateObject(json2);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(R"(["vme","de","tsc","pse"])", response.GetJsonArray("Key1").ToJsonString());
}

// オブジェクトの更新（部分更新）.$push + $each
TEST_F(NbObjectFT, PartUpdateObjectPushEach) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbJsonArray jsonArrayValue(R"([90, 91])");
    object.PutJsonArray("Key1", jsonArrayValue);
    object.Save();

    NbJsonObject each;
    NbJsonArray jsonArrayValue2(R"([88,89])");
    each.PutJsonArray("$each", jsonArrayValue2);

    NbJsonObject json1;
    json1.PutJsonObject("Key1", each);

    NbJsonObject json2;
    json2.PutJsonObject("$push", json1);

    NbResult<NbObject> result = object.PartUpdateObject(json2);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(R"([90,91,88,89])", response.GetJsonArray("Key1").ToJsonString());
}

// オブジェクトの更新（部分更新）.$push
TEST_F(NbObjectFT, PartUpdateObjectPush) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbJsonArray jsonArrayValue(R"([90, 91])");
    object.PutJsonArray("Key1", jsonArrayValue);
    object.Save();

    NbJsonObject json1(R"({"Key1": 89})");
    NbJsonObject json2;
    json2.PutJsonObject("$push", json1);

    NbResult<NbObject> result = object.PartUpdateObject(json2);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(R"([90,91,89])", response.GetJsonArray("Key1").ToJsonString());
}

// オブジェクトの更新（部分更新）.$each
TEST_F(NbObjectFT, PartUpdateObjectEach) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbJsonArray jsonArrayValue(R"(["electronics","supples"])");
    object.PutJsonArray("Key1", jsonArrayValue);
    object.Save();

    NbJsonObject json2;
    NbJsonArray jsonArrayValue2(R"(["camera","electronics","accessories"])");
    json2.PutJsonArray("$each", jsonArrayValue2);

    NbJsonObject json3;
    json3.PutJsonObject("Key1", json2);

    NbJsonObject json4;
    json4.PutJsonObject("$addToSet", json3);

    NbResult<NbObject> result = object.PartUpdateObject(json4);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(R"(["electronics","supples","camera","accessories"])", response.GetJsonArray("Key1").ToJsonString());
}

// オブジェクトの更新（部分更新）.$slice
TEST_F(NbObjectFT, PartUpdateObjectSlice) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbJsonArray jsonArrayValue(R"([80,90])");
    object.PutJsonArray("Key1", jsonArrayValue);
    object.Save();

    NbJsonObject json2;
    NbJsonArray jsonArrayValue2(R"([100,20])");

    json2["$slice"] = -3;
    json2.PutJsonArray("$each", jsonArrayValue2);

    NbJsonObject json3;
    json3.PutJsonObject("Key1", json2);

    NbJsonObject json4;
    json4.PutJsonObject("$push", json3);

    NbResult<NbObject> result = object.PartUpdateObject(json4);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(R"([90,100,20])", response.GetJsonArray("Key1").ToJsonString());
}

// オブジェクトの更新（部分更新）.$sort
TEST_F(NbObjectFT, PartUpdateObjectSort) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbJsonArray jsonArrayValue(R"([89, 70, 89, 50])");
    object.PutJsonArray("Key1", jsonArrayValue);
    object.Save();

    NbJsonObject json2;
    NbJsonArray jsonArrayValue2(R"([40,60])");
    json2.PutJsonArray("$each", jsonArrayValue2);
    json2["$sort"] = -1;

    NbJsonObject json3;
    json3.PutJsonObject("Key1", json2);

    NbJsonObject json4;
    json4.PutJsonObject("$push", json3);

    NbResult<NbObject> result = object.PartUpdateObject(json4);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(R"([89,89,70,60,50,40])", response.GetJsonArray("Key1").ToJsonString());
}

// オブジェクトの更新（部分更新）.$bit
TEST_F(NbObjectFT, PartUpdateObjectBit) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["Key1"] = 13;
    object.Save();

    NbJsonObject json2;
    json2["and"] = 10;

    NbJsonObject json3;
    json3.PutJsonObject("Key1", json2);

    NbJsonObject json4;
    json4.PutJsonObject("$bit", json3);

    NbResult<NbObject> result = object.PartUpdateObject(json4);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(8, response.GetInt("Key1"));
}

// オブジェクトの更新（部分更新）.リクエストパラメータ誤り
// Etagに不正値を用いる
TEST_F(NbObjectFT, PartUpdateObjectInvalidEtag) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbAcl acl = NbAcl::CreateAclForAnonymous();
    object.SetAcl(acl);
    object["testKey"] = "testValue";
    object.Save(true);
    
    NbJsonObject json(R"({"UpdateKey": "UpdateValue"})");

    NbObject original = object;

    object.SetETag("invalid");

    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(409, result.GetRestError().status_code);
    NbJsonObject json2(rest_error.reason);
    EXPECT_EQ("etag_mismatch", json2.GetString("reasonCode"));

    // detailチェック
    EXPECT_EQ(original.GetObjectId(), json2.GetJsonObject("detail").GetString("_id"));
    EXPECT_EQ("testValue", json2.GetJsonObject("detail").GetString("testKey"));
    string created_at = json2.GetJsonObject("detail").GetString("createdAt");
    FTUtil::CompareTime(original.GetCreatedTime(), NbUtility::DateStringToTm(created_at), true);
    string updated_at = json2.GetJsonObject("detail").GetString("updatedAt");
    FTUtil::CompareTime(original.GetUpdatedTime(), NbUtility::DateStringToTm(updated_at), true);
    EXPECT_EQ(original.GetETag(), json2.GetJsonObject("detail").GetString("etag"));
    EXPECT_EQ(acl.ToJsonObject(), json2.GetJsonObject("detail").GetJsonObject("ACL"));
}

// オブジェクトの更新（部分更新）.リクエストパラメータ誤り
// クエリに不正な演算子設定を行う
TEST_F(NbObjectFT, PartUpdateObjectInvalid) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["Key1"] = 1;
    object.Save();

    NbJsonObject json1(R"({"Key1": 5})");
    NbJsonObject json2;
    json2.PutJsonObject("$inc_invalid", json1);

    NbResult<NbObject> result = object.PartUpdateObject(json2);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());

    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(400, result.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_TRUE(json.IsMember("error"));
}

// オブジェクトの更新（部分更新）.リクエストパラメータ誤り
// IDを変更
TEST_F(NbObjectFT, PartUpdateObjectInvalidId) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["Key1"] = 1;
    object.Save();

    NbJsonObject json1;
    json1["_id"] = "invalid";

    NbResult<NbObject> result = object.PartUpdateObject(json1);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());

    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(500, result.GetRestError().status_code);
    NbJsonObject json2(rest_error.reason);
    EXPECT_TRUE(json2.IsMember("error"));
}

// オブジェクトの更新（部分更新）.部分更新用 JSONが空
TEST_F(NbObjectFT, PartUpdateObjectEmpty) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["Key1"] = 1;
    object.Save();

    NbJsonObject json;

    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

// オブジェクトの更新（部分更新）.オブジェクトIDが空文字
TEST_F(NbObjectFT, PartUpdateObjectNoObjectId) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["Key1"] = 1;
    object.Save();

    NbJsonObject json;
    json["UpdateKey"] = "UpdateValue";
    object.SetObjectId("");

    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_OBJECT_ID, result.GetResultCode());
}

// オブジェクトの更新（部分更新）.バケット名が空文字
TEST_F(NbObjectFT, PartUpdateObjectNoBucketName) {
    NbObjectBucket object_bucket(service_, "");
    NbObject object = object_bucket.NewObject();

    NbJsonObject json;
    json["Key1"] = 2;
    NbResult<NbObject> result = object.PartUpdateObject(json);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

// オブジェクト更新(完全上書き)
// 作成日時、ACLを更新
TEST_F(NbObjectFT, SaveFullUpdate) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(NbAcl::CreateAclForAnonymous());
    object["testKey"] = "testValue";
    object.Save(true);

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

// オブジェクト更新(完全上書き).データの内容に変更なし
TEST_F(NbObjectFT, SaveFullUpdateNoChange) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(NbAcl::CreateAclForAnonymous());
    object["testKey"] = "testValue";
    object.Save(true);

    // 更新時間を変化させるためsleep
    sleep(1);
    NbObject original = object;

    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("testValue"), response.GetString("testKey"));
    FTUtil::CompareTime(original.GetCreatedTime(), response.GetCreatedTime(), true);
    FTUtil::CompareTime(original.GetUpdatedTime(), response.GetUpdatedTime(), false);
    EXPECT_NE(original.GetETag(), response.GetETag());
    FTUtil::CompareAcl(original.GetAcl(), response.GetAcl());

    FTUtil::CompareObject(object, response);
}

// オブジェクト更新(完全上書き).削除済みマークされたオブジェクトの上書き
TEST_F(NbObjectFT, SaveFullUpdateDeleteMark) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    object.Save();
    object.DeleteObject(true);

    object["UpdateKey"] = "UpdateValue";

    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("UpdateValue"), response.GetString("UpdateKey"));
    FTUtil::CompareObject(object, response);
}

// オブジェクト更新(完全上書き).予約フィールド変更
TEST_F(NbObjectFT, SaveFullUpdateReservedField) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbAcl acl1 = NbAcl::CreateAclForAnonymous();
    object.SetAcl(acl1);
    object["testKey"] = "testValue";
    object.Save(true);

    object["UpdateKey"] = "UpdateValue";
    object["_id"] = "invalid";
    object["createdAt"] = "invalid";
    object["updatedAt"] = "invalid";
    object["ACL"] = R"({"r":[],"w":[],"c":[],"u":[],"d":[],"admin":[]})";
    object["etag"] = "invalid";
    object["_deleted"] = "invalid";

    // 更新時間を変化させるためsleep
    sleep(1);
    NbObject original = object;
    NbResult<NbObject> result = object.Save();;

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("testValue"), response.GetString("testKey"));
    EXPECT_EQ(string("UpdateValue"), response.GetString("UpdateKey"));
    EXPECT_EQ(original.GetObjectId(), response.GetObjectId());
    FTUtil::CompareTime(original.GetCreatedTime(), response.GetCreatedTime(), true);
    FTUtil::CompareTime(original.GetUpdatedTime(), response.GetUpdatedTime(), false);
    EXPECT_NE(original.GetETag(), response.GetETag());
    FTUtil::CompareAcl(acl1, response.GetAcl());
    EXPECT_EQ(original.IsDeleteMark(), response.IsDeleteMark());
    FTUtil::CompareObject(object, response);
}

// オブジェクト更新(完全上書き).オプションパラメータ(etag)未設定
TEST_F(NbObjectFT, SaveFullUpdateNotSetOptionParameter) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    object.Save();

    object["UpdateKey"] = "UpdateValue";

    object.SetETag("");
    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("testValue"), response.GetString("testKey"));
    EXPECT_EQ(string("UpdateValue"), response.GetString("UpdateKey"));

    FTUtil::CompareObject(object, response);
}

// オブジェクト更新(完全上書き).マスターキー
TEST_F(NbObjectFT, SaveFullUpdateMaster) {
    NbObjectBucket object_bucket(service_masterkey_, kObjectBucketName);

    // ObjectバケットACL更新
    string content_acl = R"({"r":[],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("object", kObjectBucketName, "", content_acl);

    // ObjectデータACL設定
    NbJsonObject json(R"({"r":[],"w":[],"c":["g:anonymous"],"u":[],"d":[],"admin":[]})");
    NbAcl expected_acl(json);

    NbObject object = object_bucket.NewObject();
    object.SetAcl(expected_acl);
    object["testKey"] = "testValue";
    object.Save(true);

    object["UpdateKey"] = "UpdateValue";

    NbResult<NbObject> result = object.Save();

    // バケット初期化
    FTUtil::CreateBucket("object", "objectBucket");

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ(string("UpdateValue"), response.GetString("UpdateKey"));
    FTUtil::CompareObject(object, response);
}

// オブジェクト更新(完全上書き).権限エラー(オブジェクト：admin)
TEST_F(NbObjectFT, SaveFullUpdateForbiddenAdmin) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbObject object = object_bucket.NewObject();

    // ObjectデータACL設定
    NbJsonObject json1(R"({"r":["g:anonymous"],"w":[],"c":[],"u":["g:anonymous"],"d":[],"admin":[]})");
    NbAcl expected_acl1(json1);

    object.SetAcl(expected_acl1);
    object["testKey"] = "testValue";
    object.Save(true);

    NbJsonObject json2(R"({"r":[],"w":[],"c":[],"u":["g:anonymous"],"d":[],"admin":[]})");
    NbAcl expected_acl2(json2);

    object.SetAcl(expected_acl2);
    object["UpdateKey"] = "UpdateValue";

    NbResult<NbObject> result = object.Save(true);

    // バケット初期化
    FTUtil::CreateBucket("object", "objectBucket");

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());

    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(403, result.GetRestError().status_code);
    NbJsonObject json3(rest_error.reason);
    EXPECT_TRUE(json3.IsMember("error"));
}

// オブジェクト更新(完全上書き).権限エラー(オブジェクト：update)
TEST_F(NbObjectFT, SaveFullUpdateForbiddenUpdate) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbObject object = object_bucket.NewObject();

    // ObjectデータACL設定
    NbJsonObject json1(R"({"r":[],"w":[],"c":["g:anonymous"],"u":[],"d":[],"admin":["g:anonymous"]})");
    NbAcl expected_acl(json1);

    object.SetAcl(expected_acl);
    object["testKey"] = "testValue";
    object.Save(true);

    object["UpdateKey"] = "UpdateValue";

    NbResult<NbObject> result = object.Save();

    // バケット初期化
    FTUtil::CreateBucket("object", "objectBucket");

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());

    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(403, result.GetRestError().status_code);
    NbJsonObject json2(rest_error.reason);
    EXPECT_TRUE(json2.IsMember("error"));
}

// オブジェクト更新(完全上書き).バケット名が空文字
TEST_F(NbObjectFT, SaveFullUpdateNoBucketName) {
    NbObjectBucket object_bucket(service_, "");
    NbObject object = object_bucket.NewObject();
    object.Save();

    NbResult<NbObject> result = object.Save();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

// オブジェクトの削除
// 論理削除
TEST_F(NbObjectFT, DeleteObject) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbAcl acl = NbAcl::CreateAclForAnonymous();
    object.SetAcl(acl);
    object["testKey"] = "testValue";
    object.Save(true);

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

    NbQuery query;
    int count;

    query.DeleteMark(true);

    NbResult<std::vector<NbObject>> result2 = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result2.IsSuccess());
    std::vector<NbObject> response2 = result2.GetSuccessData();
    EXPECT_EQ(1, response2.size());
    EXPECT_EQ(1, count);
}

// オブジェクトの削除
// 物理削除
TEST_F(NbObjectFT, DeleteObjectPhysical) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbAcl acl = NbAcl::CreateAclForAnonymous();
    object.SetAcl(acl);
    object["testKey"] = "testValue";
    object.Save(true);

    // 更新時間を変化させるためsleep
    sleep(1);

    NbResult<NbObject> result1 = object.DeleteObject(false);

    // 戻り値確認
    ASSERT_TRUE(result1.IsSuccess());
    NbObject response1 = result1.GetSuccessData();

    EXPECT_EQ("{}", response1.ToJsonString());

    NbQuery query;
    int count;

    query.DeleteMark(true);

    NbResult<std::vector<NbObject>> result2 = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result2.IsSuccess());
    std::vector<NbObject> response2 = result2.GetSuccessData();
    EXPECT_TRUE(response2.empty());
    EXPECT_EQ(0, count);
}

// オブジェクトの削除.削除の指定なし
TEST_F(NbObjectFT, DeleteObjectNotSelect) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbAcl acl = NbAcl::CreateAclForAnonymous();
    object.SetAcl(acl);
    object["testKey"] = "testValue";
    object.Save(true);

    // 更新時間を変化させるためsleep
    sleep(1);

    NbResult<NbObject> result1 = object.DeleteObject();

    // 戻り値確認
    ASSERT_TRUE(result1.IsSuccess());
    NbObject response1 = result1.GetSuccessData();

    EXPECT_EQ("{}", response1.ToJsonString());

    NbQuery query;
    int count;

    NbResult<std::vector<NbObject>> result2 = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result2.IsSuccess());
    std::vector<NbObject> response2 = result2.GetSuccessData();
    EXPECT_TRUE(response2.empty());
    EXPECT_EQ(0, count);
}

// オブジェクトの削除.オプションパラメータ(etag、DeleteMark)未設定
TEST_F(NbObjectFT, DeleteObjectNotSetOptionParameter) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbAcl acl = NbAcl::CreateAclForAnonymous();
    object.SetAcl(acl);
    object["testKey"] = "testValue";
    object.Save(true);

    // 更新時間を変化させるためsleep
    sleep(1);
    object.SetETag("");
    NbResult<NbObject> result1 = object.DeleteObject();

    // 戻り値確認
    ASSERT_TRUE(result1.IsSuccess());
    NbObject response1 = result1.GetSuccessData();

    EXPECT_EQ("{}", response1.ToJsonString());


    NbQuery query;
    int count;

    NbResult<std::vector<NbObject>> result2 = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result2.IsSuccess());
    std::vector<NbObject> response2 = result2.GetSuccessData();
    EXPECT_TRUE(response2.empty());
    EXPECT_EQ(0, count);
}

// オブジェクトの削除.マスターキー
TEST_F(NbObjectFT, DeleteObjectMaster) {
    NbObjectBucket object_bucket(service_masterkey_, kObjectBucketName);

    // ObjectバケットACL更新
    string content_acl = R"({"r":[],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("object", kObjectBucketName, "", content_acl);

    // ObjectデータACL設定
    NbJsonObject json(R"({"r":["g:anonymous"],"w":[],"c":["g:anonymous"],"u":[],"d":[],"admin":[]})");
    NbAcl expected_acl(json);

    NbObject object = object_bucket.NewObject();
    object.SetAcl(expected_acl);
    object["testKey"] = "testValue";
    object.Save(true);

    NbResult<NbObject> result = object.DeleteObject();

    // バケット初期化
    FTUtil::CreateBucket("object", "objectBucket");

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbObject response = result.GetSuccessData();

    EXPECT_EQ("{}", response.ToJsonString());
}

// オブジェクトの削除.権限エラー(オブジェクト)
TEST_F(NbObjectFT, DeleteObjectForbidden) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    // ObjectデータACL設定
    NbJsonObject json(R"({"r":[],"w":[],"c":["g:anonymous"],"u":["g:anonymous"],"d":[],"admin":[]})");
    NbAcl expected_acl(json);

    NbObject object = object_bucket.NewObject();
    object.SetAcl(expected_acl);
    object["testKey"] = "testValue";
    object.Save(true);

    NbResult<NbObject> result = object.DeleteObject();

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());

    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(403, result.GetRestError().status_code);
    NbJsonObject json2(rest_error.reason);
    EXPECT_TRUE(json2.IsMember("error"));
}

// オブジェクトの削除.リクエストパラメータ誤り
// Etagに不正値を用いる
TEST_F(NbObjectFT, DeleteObjectInvalidEtag) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbAcl acl = NbAcl::CreateAclForAnonymous();
    object.SetAcl(acl);
    object["testKey"] = "testValue";
    object.Save(true);

    NbObject original = object;

    object.SetETag("invalid");

    NbResult<NbObject> result = object.DeleteObject();

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(409, result.GetRestError().status_code);
    NbJsonObject json2(rest_error.reason);
    EXPECT_EQ("etag_mismatch", json2.GetString("reasonCode"));

    // detailチェック
    EXPECT_EQ(original.GetObjectId(), json2.GetJsonObject("detail").GetString("_id"));
    EXPECT_EQ("testValue", json2.GetJsonObject("detail").GetString("testKey"));
    string created_at = json2.GetJsonObject("detail").GetString("createdAt");
    FTUtil::CompareTime(original.GetCreatedTime(), NbUtility::DateStringToTm(created_at), true);
    string updated_at = json2.GetJsonObject("detail").GetString("updatedAt");
    FTUtil::CompareTime(original.GetUpdatedTime(), NbUtility::DateStringToTm(updated_at), true);
    EXPECT_EQ(original.GetETag(), json2.GetJsonObject("detail").GetString("etag"));
    EXPECT_EQ(acl.ToJsonObject(), json2.GetJsonObject("detail").GetJsonObject("ACL"));
}

// オブジェクトの削除.オブジェクトIDが空文字
TEST_F(NbObjectFT, DeleteObjectNoObjectId) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object["Key1"] = 1;
    object.Save();

    object.SetObjectId("");
    NbResult<NbObject> result = object.DeleteObject();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_OBJECT_ID, result.GetResultCode());
}

// オブジェクトの削除.バケット名が空文字
TEST_F(NbObjectFT, DeleteObjectNoBucketName) {
    NbObjectBucket object_bucket(service_, "");
    NbObject object = object_bucket.NewObject();
    object["Key1"] = 1;
    object.Save();

    NbResult<NbObject> result = object.DeleteObject();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

// マルチテナント
TEST_F(NbObjectFT, MultiTenant) {
    shared_ptr<NbService> service_2 = NbService::CreateService(kEndPointUrl, kTenantIdB, kAppIdB, kAppKeyB, kProxyB);

    NbQuery query;
    int count;

    // バケット作成
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObjectBucket object_bucket2(service_2, kObjectBucketName);

    // TenantA.オブジェクト作成
    NbObject object = object_bucket.NewObject();
    object["testKey"] = "testValue";
    NbResult<NbObject> result_create = object.Save();

    // 戻り値確認
    ASSERT_TRUE(result_create.IsSuccess());
    NbObject response_create = result_create.GetSuccessData();
    EXPECT_EQ(string("testValue"), response_create.GetString("testKey"));

    // TenantA.クエリ
    NbResult<std::vector<NbObject>> result_query = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query.IsSuccess());
    std::vector<NbObject> response_query = result_query.GetSuccessData();
    FTUtil::CompareObject(object, response_query[0]);
    EXPECT_EQ("testValue", response_query[0].GetString("testKey"));
    EXPECT_EQ(1, count);

    // TenantB.クエリ
    NbResult<std::vector<NbObject>> result_query2 = object_bucket2.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query2.IsSuccess());
    std::vector<NbObject> response_query2 = result_query2.GetSuccessData();
    EXPECT_EQ(0, count);

    // TenantB.オブジェクト作成
    NbObject object2 = object_bucket2.NewObject();
    object2["testKey2"] = "testValue2";
    NbResult<NbObject> result_create2 = object2.Save();

    // 戻り値確認
    ASSERT_TRUE(result_create2.IsSuccess());
    NbObject response_create2 = result_create2.GetSuccessData();
    EXPECT_EQ("testValue2", response_create2.GetString("testKey2"));

    // TenantA.クエリ
    NbResult<std::vector<NbObject>> result_query3 = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query3.IsSuccess());
    std::vector<NbObject> response_query3 = result_query3.GetSuccessData();
    FTUtil::CompareObject(object, response_query3[0]);
    EXPECT_FALSE(response_query3[0].IsMember("testKey2"));
    EXPECT_EQ("testValue", response_query3[0].GetString("testKey"));
    EXPECT_EQ(1, count);

    // TenantB.クエリ
    NbResult<std::vector<NbObject>> result_query4 = object_bucket2.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query4.IsSuccess());
    std::vector<NbObject> response_query4 = result_query4.GetSuccessData();
    FTUtil::CompareObject(object2, response_query4[0]);
    EXPECT_FALSE(response_query4[0].IsMember("testKey"));
    EXPECT_EQ("testValue2", response_query4[0].GetString("testKey2"));
    EXPECT_EQ(1, count);


    // TenantA.オブジェクト更新
    object["testKey"] = "testValue2";
    object["UpdateKey"] ="UpdateValue";
    NbResult<NbObject> result_update = object.Save();

    // 戻り値確認
    ASSERT_TRUE(result_update.IsSuccess());
    NbObject response_update = result_update.GetSuccessData();
    EXPECT_EQ(string("testValue2"), response_update.GetString("testKey"));
    EXPECT_EQ("UpdateValue", response_update.GetString("UpdateKey"));

    // TenantA.クエリ
    NbResult<std::vector<NbObject>> result_query5 = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query5.IsSuccess());
    std::vector<NbObject> response_query5 = result_query5.GetSuccessData();
    FTUtil::CompareObject(object, response_query5[0]);
    EXPECT_EQ("testValue2", response_query5[0].GetString("testKey"));
    EXPECT_EQ("UpdateValue", response_query5[0].GetString("UpdateKey"));
    EXPECT_EQ(1, count);

    // TenantB.クエリ
    NbResult<std::vector<NbObject>> result_query6 = object_bucket2.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query6.IsSuccess());
    std::vector<NbObject> response_query6 = result_query6.GetSuccessData();
    FTUtil::CompareObject(object2, response_query6[0]);
    EXPECT_FALSE(response_query6[0].IsMember("testKey"));
    EXPECT_FALSE(response_query6[0].IsMember("UpdateKey"));
    EXPECT_EQ(1, count);

    // TenantB.オブジェクト更新
    object2["testKey2"] = "testValue3";
    object2["UpdateKey2"] ="UpdateValue2";
    NbResult<NbObject> result_update2 = object2.Save();

    // 戻り値確認
    ASSERT_TRUE(result_update2.IsSuccess());
    NbObject response_update2 = result_update2.GetSuccessData();
    EXPECT_EQ(string("testValue3"), response_update2.GetString("testKey2"));
    EXPECT_EQ("UpdateValue2", response_update2.GetString("UpdateKey2"));

    // TenantA.クエリ
    NbResult<std::vector<NbObject>> result_query7 = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query7.IsSuccess());
    std::vector<NbObject> response_query7 = result_query7.GetSuccessData();
    FTUtil::CompareObject(object, response_query7[0]);
    EXPECT_FALSE(response_query7[0].IsMember("testKey2"));
    EXPECT_FALSE(response_query7[0].IsMember("UpdateKey2"));
    EXPECT_EQ(1, count);

    // TenantB.クエリ
    NbResult<std::vector<NbObject>> result_query8 = object_bucket2.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query8.IsSuccess());
    std::vector<NbObject> response_query8 = result_query8.GetSuccessData();
    FTUtil::CompareObject(object2, response_query8[0]);
    EXPECT_EQ("testValue3", response_query8[0].GetString("testKey2"));
    EXPECT_EQ("UpdateValue2", response_query8[0].GetString("UpdateKey2"));
    EXPECT_EQ(1, count);


    // TenantA.オブジェクト削除
    NbResult<NbObject> result_delete = object.DeleteObject();

    // 戻り値確認
    ASSERT_TRUE(result_delete.IsSuccess());
    NbObject response_delete = result_delete.GetSuccessData();
    EXPECT_EQ("{}", response_delete.ToJsonString());

    // TenantA.クエリ
    NbResult<std::vector<NbObject>> result_query9 = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query9.IsSuccess());
    EXPECT_EQ(0, count);

    // TenantB.クエリ
    NbResult<std::vector<NbObject>> result_query10 = object_bucket2.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query10.IsSuccess());
    std::vector<NbObject> response_query10 = result_query10.GetSuccessData();
    FTUtil::CompareObject(object2, response_query10[0]);
    EXPECT_EQ(1, count);

    // TenantB.オブジェクト削除
    NbResult<NbObject> result_delete2 = object2.DeleteObject();

    // 戻り値確認
    ASSERT_TRUE(result_delete2.IsSuccess());
    NbObject response_delete2 = result_delete2.GetSuccessData();
    EXPECT_EQ("{}", response_delete2.ToJsonString());

    // TenantA.クエリ
    NbResult<std::vector<NbObject>> result_query11 = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query11.IsSuccess());
    EXPECT_EQ(0, count);

    // TenantB.クエリ
    NbResult<std::vector<NbObject>> result_query12 = object_bucket2.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query12.IsSuccess());
    EXPECT_EQ(0, count);
}

// 各種データ型が混在するオブジェクトの作成・クエリ
TEST_F(NbObjectFT, SaveNewQueryMix) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();

    NbJsonObject jsonobject;
    NbJsonArray jsonarray(R"(["Car","Music"])");

    NbJsonObject a;

    object["name"] = "Taro";
    object["age"] = 13;
    object["option"] = true;
    object.PutJsonArray("hobby", jsonarray);

    jsonobject["surrogate"] = "\u3046\u3048";
    object.PutNull("type");
    jsonobject["mark"]["mark1"] = " !\"#$%&'()*+-.,/:;<=>?@[]^_`{|}~";
    jsonobject["mark"]["mark2"] = "aaa";
    object.PutJsonObject("other", jsonobject);

    NbResult<NbObject> result_create = object.Save();

    // 戻り値確認
    ASSERT_TRUE(result_create.IsSuccess());

    // クエリ

    NbQuery query;
    int count;

    NbResult<std::vector<NbObject>> result_query = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result_query.IsSuccess());
    std::vector<NbObject> response_query = result_query.GetSuccessData();
    EXPECT_EQ("Taro", response_query[0].GetString("name"));
    EXPECT_EQ(13, response_query[0].GetInt("age"));
    EXPECT_EQ(true, response_query[0].GetBoolean("option"));
    EXPECT_EQ("\u3046\u3048", response_query[0].GetJsonObject("other").GetString("surrogate"));
    EXPECT_EQ(R"(["Car","Music"])", response_query[0].GetJsonArray("hobby").ToJsonString());
    EXPECT_EQ(NbJsonType::NB_JSON_NULL , response_query[0].GetType("type"));
    EXPECT_EQ(" !\"#$%&'()*+-.,/:;<=>?@[]^_`{|}~", response_query[0].GetJsonObject("other").GetJsonObject("mark").GetString("mark1"));
    EXPECT_EQ("aaa", response_query[0].GetJsonObject("other").GetJsonObject("mark").GetString("mark2"));
    EXPECT_EQ(1, count);
}

// 繰り返し評価
TEST_F(NbObjectFT, CURQD3) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);

    NbQuery query;
    int count;

    for (int i = 0; i < 3; ++i ) {
        // 作成
        NbObject object = object_bucket.NewObject();
        object["testKey"] = "testValue";
        NbResult<NbObject> result_create = object.Save();

        // 戻り値確認
        ASSERT_TRUE(result_create.IsSuccess());
        NbObject response_create = result_create.GetSuccessData();
        EXPECT_EQ("testValue", response_create.GetString("testKey"));

        // 更新
        object["UpdateKey"] ="UpdateValue";
        NbResult<NbObject> result_update = object.Save();

        // 戻り値確認
        ASSERT_TRUE(result_update.IsSuccess());
        NbObject response_update = result_update.GetSuccessData();
        EXPECT_EQ("UpdateValue", response_update.GetString("UpdateKey"));

        // 読み込み
        NbResult<NbObject> result_get = object_bucket.GetObject(object.GetObjectId());

        // 戻り値確認
        ASSERT_TRUE(result_get.IsSuccess());
        NbObject response_get = result_get.GetSuccessData();
        FTUtil::CompareObject(object, response_get);
        
        // クエリ;
        NbResult<std::vector<NbObject>> result_query = object_bucket.Query(query, &count);

        // 戻り値確認
        ASSERT_TRUE(result_query.IsSuccess());
        std::vector<NbObject> response_query = result_query.GetSuccessData();
        EXPECT_EQ("testValue", response_query[0].GetString("testKey"));
        EXPECT_EQ("UpdateValue", response_query[0].GetString("UpdateKey"));
    
        // 削除
        NbResult<NbObject> result_delete = object.DeleteObject();

        // 戻り値確認
        ASSERT_TRUE(result_delete.IsSuccess());
        NbObject response_delete = result_delete.GetSuccessData();
        EXPECT_EQ("{}", response_delete.ToJsonString());
    }
}

// 繰り返し評価.異常系
TEST_F(NbObjectFT, UpdateDeleteException) {
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    NbAcl acl = NbAcl::CreateAclForAnonymous();
    object.SetAcl(acl);
    object["testKey"] = "testValue";
    object.Save(true);
    NbObject object2 = object;

    std::thread update_thread( [&object]() {
            object["UpdateKey"] ="UpdateValue";
            object.Save();
        });

    struct timespec time{0, 10000000}; //10ms WAIT
    nanosleep(&time, nullptr);
    NbResult<NbObject> result_delete = object2.DeleteObject();

    update_thread.join();

    // 戻り値確認
    EXPECT_TRUE(result_delete.IsRestError());
    NbRestError rest_error = result_delete.GetRestError();
    EXPECT_EQ(409, result_delete.GetRestError().status_code);
    NbJsonObject json(rest_error.reason);
    EXPECT_EQ("etag_mismatch", json.GetString("reasonCode"));

    // detailチェック
    EXPECT_EQ(object.GetObjectId(), json.GetJsonObject("detail").GetString("_id"));
    EXPECT_EQ("testValue", json.GetJsonObject("detail").GetString("testKey"));
    EXPECT_EQ("UpdateValue", json.GetJsonObject("detail").GetString("UpdateKey"));
    string created_at = json.GetJsonObject("detail").GetString("createdAt");
    FTUtil::CompareTime(object.GetCreatedTime(), NbUtility::DateStringToTm(created_at), true);
    string updated_at = json.GetJsonObject("detail").GetString("updatedAt");
    FTUtil::CompareTime(object.GetUpdatedTime(), NbUtility::DateStringToTm(updated_at), true);
    EXPECT_EQ(object.GetETag(), json.GetJsonObject("detail").GetString("etag"));
    EXPECT_EQ(acl.ToJsonObject(), json.GetJsonObject("detail").GetJsonObject("ACL"));
}

// 性能.オブジェクト数
TEST_F(NbObjectFT, SaveNewMAXCountPerformanceSlowTest) {
    NbObjectBucket object_bucket(service_, kObjectBucketName); 

    RegisterData1KB10000(object_bucket);

    NbQuery query;
    int count;

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(10000, count);
}

// 性能.最大オブジェクトサイズ
TEST_F(NbObjectFT, SaveNewMAXSizePerformance) {
    NbObjectBucket object_bucket(service_, kObjectBucketName); 

    int length = 1024*256;
    string data;

    while(data.length() < length){
        data += kTestData1KB;
    }

    string data2 = data.substr(0, length - 29);
    NbObject object = object_bucket.NewObject();
    object["DATA"] = data2;
    object["DATA_ID"] = "00000";
    object.Save();

    NbQuery query;
    int count;

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(1, count);

    // DATAサイズチェック
    EXPECT_EQ(length, response[0].ToJsonString().length());
}

// 性能.最大保証サイズオブジェクトの保持
TEST_F(NbObjectFT, SaveNewMAXSizeInOneBacketCountPerformanceSlowTest) {
    NbObjectBucket object_bucket(service_, kObjectBucketName); 

    int length = 1024*256;
    string data;

    while(data.length() < length){
        data += kTestData1KB;
    }

    for (int i = 0; i < 8; ++i) {
        string data2 = data.substr(0, length - 29);
        NbObject object = object_bucket.NewObject();
        object["DATA"] = data2;
        int num = 1 + i;
        std::ostringstream id_org;
        id_org << std::setfill('0') << std::setw(5) << num;
        object["DATA_ID"] = id_org.str();
        NbResult<NbObject> result = object.Save();
        
        // 戻り値確認
        ASSERT_TRUE(result.IsSuccess());
    }

    NbQuery query;
    int count;

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(8, count);

    for(NbObject response_data : response) {
        // DATAサイズチェック
        EXPECT_EQ(length, response_data.ToJsonString().length());
    }
}

// 性能.検索時間 (100件)
TEST_F(NbObjectFT, QueryTimePerformanceSlowTest) {
    NbObjectBucket object_bucket(service_, kObjectBucketName); 

    RegisterData1KB10000(object_bucket);

    NbQuery query;
    int count;

    for(int i = 0; i < 5; ++i){
        system_clock::time_point start_time = system_clock::now();
        NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);
        system_clock::time_point end_time = system_clock::now();

        // 戻り値確認
        ASSERT_TRUE(result.IsSuccess());
        std::vector<NbObject> response = result.GetSuccessData();
        EXPECT_EQ(10000, count);
        EXPECT_EQ(100, response.size());

        EXPECT_GT(300, std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());
    }
}

// 性能.作成時間 (1件)
TEST_F(NbObjectFT, SaveNewTimePerformanceSlowTest) {
    NbObjectBucket object_bucket(service_, kObjectBucketName); 

    RegisterData1KB10000(object_bucket);

    for(int i = 0; i < 5; ++i){
        NbObject object = object_bucket.NewObject();
        object["key"] = "value";
        system_clock::time_point start_time = system_clock::now();
        NbResult<NbObject> result = object.Save();
        system_clock::time_point end_time = system_clock::now();

        // 戻り値確認
        ASSERT_TRUE(result.IsSuccess());
        NbJsonObject response = result.GetSuccessData();
        EXPECT_TRUE(response.IsMember("key"));

        EXPECT_GT(100, std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());
    }
}

// 性能.更新時間 (1件)
TEST_F(NbObjectFT, UpdateTimePerformanceSlowTest) {
    NbObjectBucket object_bucket(service_, kObjectBucketName); 

    RegisterData1KB10000(object_bucket);

    NbQuery query;
    int count;

    query.Limit(5);

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);
    EXPECT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(5, response.size());

    for(int i = 0; i < 5; i++){
        response[i]["key2"] = "value2";
        system_clock::time_point start_time = system_clock::now();
        NbResult<NbObject> result2 = response[i].Save();
        system_clock::time_point end_time = system_clock::now();
    
        // 戻り値確認
        ASSERT_TRUE(result2.IsSuccess());
        NbJsonObject response2 = result2.GetSuccessData();
        EXPECT_TRUE(response2.IsMember("key2"));

        EXPECT_GT(100, std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());
    }
}

// 性能.削除時間 (1件)
TEST_F(NbObjectFT, DeleteTimePerformanceSlowTest) {
    NbObjectBucket object_bucket(service_, kObjectBucketName); 

    RegisterData1KB10000(object_bucket);

    NbQuery query;
    int count;

    query.Limit(5);

    NbResult<std::vector<NbObject>> result = object_bucket.Query(query, &count);
    EXPECT_TRUE(result.IsSuccess());
    std::vector<NbObject> response = result.GetSuccessData();
    EXPECT_EQ(5, response.size());

    for(int i = 0; i < 5; i++){
        system_clock::time_point start_time = system_clock::now();
        NbResult<NbObject> result2 = response[i].DeleteObject();
        system_clock::time_point end_time = system_clock::now();
    
        // 戻り値確認
        ASSERT_TRUE(result2.IsSuccess());

        EXPECT_GT(100, std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());
    }
}

} //namespace necbaas
