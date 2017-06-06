#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_user.h"
#include "necbaas/nb_object_bucket.h"

namespace necbaas {

using std::string;
using std::list;
using std::shared_ptr;

class NbAclFT : public ::testing::Test {
  protected:
    static void SetUpTestCase() {
        service_ = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    }
    static void TearDownTestCase() {
        FTUtil::DeleteAllObject();
    }

    virtual void SetUp() {}
    virtual void TearDown() {
        if (NbUser::IsLoggedIn(service_)) {
            NbResult<NbUser> result = NbUser::Logout(service_);
        }
    }

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
};

shared_ptr<NbService> NbAclFT::service_;

static const string kDefaultAclString{R"({
    "owner":"ownerid",
    "r":["g:authenticated"],
    "w":["write1","write2","write3"],
    "c":["create1","create2"],
    "u":["update1"],
    "d":["delete1","delete2","delete3"],
    "admin":["adminid1","adminid2"]
})"};

// デフォルトコンストラクタ
TEST_F(NbAclFT, Constructor) {
    NbAcl acl;
    EXPECT_TRUE(acl.GetOwner().empty());
    EXPECT_TRUE(acl.GetRead().empty());
    EXPECT_TRUE(acl.GetWrite().empty());
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    EXPECT_TRUE(acl.GetAdmin().empty());
}

// コンストラクタ(JSON)
TEST_F(NbAclFT, ConstructorJson) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);
    EXPECT_EQ("ownerid", acl.GetOwner());
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"write1","write2","write3"}, acl.GetWrite());
    FTUtil::CompareList(list<string>{"create1","create2"}, acl.GetCreate());
    FTUtil::CompareList(list<string>{"update1"}, acl.GetUpdate());
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3"}, acl.GetDelete());
    FTUtil::CompareList(list<string>{"adminid1","adminid2"}, acl.GetAdmin());

    // キーを削除
    json.Remove("w");
    json.Remove("u");
    acl.SetJson(json);
    EXPECT_EQ("ownerid", acl.GetOwner());
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetRead());
    EXPECT_TRUE(acl.GetWrite().empty());
    FTUtil::CompareList(list<string>{"create1","create2"}, acl.GetCreate());
    EXPECT_TRUE(acl.GetUpdate().empty());
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3"}, acl.GetDelete());
    FTUtil::CompareList(list<string>{"adminid1","adminid2"}, acl.GetAdmin());
}

// ACLテンプレート生成(Anonymous)
TEST_F(NbAclFT, CreateAclForAnonymous) {
    NbAcl acl = NbAcl::CreateAclForAnonymous();
    EXPECT_TRUE(acl.GetOwner().empty());
    FTUtil::CompareList(list<string>{"g:anonymous"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"g:anonymous"}, acl.GetWrite());
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    FTUtil::CompareList(list<string>{"g:anonymous"}, acl.GetAdmin());
}

// ACLテンプレート生成(Authenticated)
TEST_F(NbAclFT, CreateAclForAuthenticated) {
    NbAcl acl = NbAcl::CreateAclForAuthenticated();
    EXPECT_TRUE(acl.GetOwner().empty());
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetWrite());
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetAdmin());
}

// ACLテンプレート生成(特定ユーザのみがアクセス可能)
TEST_F(NbAclFT, CreateAclForUser) {
    NbResult<NbUser> result = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(result.IsSuccess());
    NbUser user = result.GetSuccessData();
    
    NbAcl acl = NbAcl::CreateAclForUser(user);
    EXPECT_EQ(user.GetUserId(), acl.GetOwner());
    FTUtil::CompareList(list<string>{user.GetUserId()}, acl.GetRead());
    FTUtil::CompareList(list<string>{user.GetUserId()}, acl.GetWrite());
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    FTUtil::CompareList(list<string>{user.GetUserId()}, acl.GetAdmin());
}

// ACLテンプレート生成(ユーザID空)
TEST_F(NbAclFT, CreateAclForUserEmpty) {
    NbUser user;

    NbAcl acl = NbAcl::CreateAclForUser(user);
    EXPECT_TRUE(acl.GetOwner().empty());
    EXPECT_TRUE(acl.GetRead().empty());
    EXPECT_TRUE(acl.GetWrite().empty());
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    EXPECT_TRUE(acl.GetAdmin().empty());
}

// ACLテンプレート生成(R/W/Adminが同一の ACL)
TEST_F(NbAclFT, CreateAclFor) {
    NbAcl acl = NbAcl::CreateAclFor("userId");
    EXPECT_TRUE(acl.GetOwner().empty());
    FTUtil::CompareList(list<string>{"userId"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"userId"}, acl.GetWrite());
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    FTUtil::CompareList(list<string>{"userId"}, acl.GetAdmin());
}

// ACLテンプレート生成(R/W/Adminが同一の ACL, Entity空文字)
TEST_F(NbAclFT, CreateAclForEmpty) {
    NbAcl acl = NbAcl::CreateAclFor(kEmpty);  
    EXPECT_TRUE(acl.GetOwner().empty());
    EXPECT_TRUE(acl.GetRead().empty());
    EXPECT_TRUE(acl.GetWrite().empty());
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    EXPECT_TRUE(acl.GetAdmin().empty());
}

// ACLテンプレート生成(R/W/Adminが同一リストの ACL)
TEST_F(NbAclFT, CreateAclForList) {
    NbAcl acl = NbAcl::CreateAclFor(list<string>{"entry1", "entry2", "entry3"});
    EXPECT_TRUE(acl.GetOwner().empty());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetWrite());
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetAdmin());
}

// ACLテンプレート生成(R/W/Adminが同一リストの ACL: 重複)
TEST_F(NbAclFT, CreateAclForListDuplication) {
    NbAcl acl = NbAcl::CreateAclFor(list<string>{"entry1", "entry2", "entry1", "entry3", "entry3"});
    EXPECT_TRUE(acl.GetOwner().empty());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetWrite());
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetAdmin());
}

// ACLテンプレート生成(R/W/Adminが同一リストの ACL: 空)
TEST_F(NbAclFT, CreateAclForListEmpty) {
    NbAcl acl = NbAcl::CreateAclFor(list<string>{});
    EXPECT_TRUE(acl.GetOwner().empty());
    EXPECT_TRUE(acl.GetRead().empty());
    EXPECT_TRUE(acl.GetWrite().empty());
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    EXPECT_TRUE(acl.GetAdmin().empty());
}

// ACLテンプレート生成(R/W/Adminが同一リストの ACL: 空文字が含まれる)
TEST_F(NbAclFT, CreateAclForListPartEmpty) {
    NbAcl acl = NbAcl::CreateAclFor(list<string>{"entry1", "entry2", "", "entry3", ""});
    EXPECT_TRUE(acl.GetOwner().empty());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetWrite());
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetAdmin());
}

// ACLテンプレート生成(R/W/Adminが同一リストの ACL: 全て空文字)
TEST_F(NbAclFT, CreateAclForListAllEmpty) {
    NbAcl acl = NbAcl::CreateAclFor(list<string>{"", "", ""});
    EXPECT_TRUE(acl.GetOwner().empty());
    EXPECT_TRUE(acl.GetRead().empty());
    EXPECT_TRUE(acl.GetWrite().empty());
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    EXPECT_TRUE(acl.GetAdmin().empty());
}

// JSONデータ設定
TEST_F(NbAclFT, SetJson) {
    NbAcl acl;
    NbJsonObject json(kDefaultAclString);
    acl.SetJson(json);
    EXPECT_EQ("ownerid", acl.GetOwner());
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"write1","write2","write3"}, acl.GetWrite());
    FTUtil::CompareList(list<string>{"create1","create2"}, acl.GetCreate());
    FTUtil::CompareList(list<string>{"update1"}, acl.GetUpdate());
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3"}, acl.GetDelete());
    FTUtil::CompareList(list<string>{"adminid1","adminid2"}, acl.GetAdmin());

    // キーを削除
    json.Remove("w");
    json.Remove("u");
    acl.SetJson(json);
    EXPECT_EQ("ownerid", acl.GetOwner());
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetRead());
    EXPECT_TRUE(acl.GetWrite().empty());
    FTUtil::CompareList(list<string>{"create1","create2"}, acl.GetCreate());
    EXPECT_TRUE(acl.GetUpdate().empty());
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3"}, acl.GetDelete());
    FTUtil::CompareList(list<string>{"adminid1","adminid2"}, acl.GetAdmin());
}

// Read権限設定
TEST_F(NbAclFT, SetRead) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    acl.SetRead(list<string>{"entry1", "entry2", "", "entry3", "entry3", "", "entry2", });

    EXPECT_EQ("ownerid", acl.GetOwner());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"write1","write2","write3"}, acl.GetWrite());
    FTUtil::CompareList(list<string>{"create1","create2"}, acl.GetCreate());
    FTUtil::CompareList(list<string>{"update1"}, acl.GetUpdate());
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3"}, acl.GetDelete());
    FTUtil::CompareList(list<string>{"adminid1","adminid2"}, acl.GetAdmin());

    acl.SetRead(list<string>{});
    EXPECT_TRUE(acl.GetRead().empty());
}

// Write権限設定
TEST_F(NbAclFT, SetWrite) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    acl.SetWrite(list<string>{"entry1", "entry2", "", "entry3", "entry3", "", "entry2", });

    EXPECT_EQ("ownerid", acl.GetOwner());
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetWrite());
    FTUtil::CompareList(list<string>{"create1","create2"}, acl.GetCreate());
    FTUtil::CompareList(list<string>{"update1"}, acl.GetUpdate());
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3"}, acl.GetDelete());
    FTUtil::CompareList(list<string>{"adminid1","adminid2"}, acl.GetAdmin());

    acl.SetWrite(list<string>{});
    EXPECT_TRUE(acl.GetWrite().empty());
}

// Create権限設定
TEST_F(NbAclFT, SetCreate) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    acl.SetCreate(list<string>{"entry1", "entry2", "", "entry3", "entry3", "", "entry2", });

    EXPECT_EQ("ownerid", acl.GetOwner());
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"write1","write2","write3"}, acl.GetWrite());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetCreate());
    FTUtil::CompareList(list<string>{"update1"}, acl.GetUpdate());
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3"}, acl.GetDelete());
    FTUtil::CompareList(list<string>{"adminid1","adminid2"}, acl.GetAdmin());

    acl.SetCreate(list<string>{});
    EXPECT_TRUE(acl.GetCreate().empty());
}

// Update権限設定
TEST_F(NbAclFT, SetUpdate) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    acl.SetUpdate(list<string>{"entry1", "entry2", "", "entry3", "entry3", "", "entry2", });

    EXPECT_EQ("ownerid", acl.GetOwner());
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"write1","write2","write3"}, acl.GetWrite());
    FTUtil::CompareList(list<string>{"create1","create2"}, acl.GetCreate());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetUpdate());
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3"}, acl.GetDelete());
    FTUtil::CompareList(list<string>{"adminid1","adminid2"}, acl.GetAdmin());

    acl.SetUpdate(list<string>{});
    EXPECT_TRUE(acl.GetUpdate().empty());
}

// Delete権限設定
TEST_F(NbAclFT, SetDelete) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    acl.SetDelete(list<string>{"entry1", "entry2", "", "entry3", "entry3", "", "entry2", });

    EXPECT_EQ("ownerid", acl.GetOwner());
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"write1","write2","write3"}, acl.GetWrite());
    FTUtil::CompareList(list<string>{"create1","create2"}, acl.GetCreate());
    FTUtil::CompareList(list<string>{"update1"}, acl.GetUpdate());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetDelete());
    FTUtil::CompareList(list<string>{"adminid1","adminid2"}, acl.GetAdmin());

    acl.SetDelete(list<string>{});
    EXPECT_TRUE(acl.GetDelete().empty());
}

// Admin権限設定
TEST_F(NbAclFT, SetAdmin) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    acl.SetAdmin(list<string>{"entry1", "entry2", "", "entry3", "entry3", "", "entry2", });

    EXPECT_EQ("ownerid", acl.GetOwner());
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetRead());
    FTUtil::CompareList(list<string>{"write1","write2","write3"}, acl.GetWrite());
    FTUtil::CompareList(list<string>{"create1","create2"}, acl.GetCreate());
    FTUtil::CompareList(list<string>{"update1"}, acl.GetUpdate());
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3"}, acl.GetDelete());
    FTUtil::CompareList(list<string>{"entry1", "entry2", "entry3"}, acl.GetAdmin());

    acl.SetAdmin(list<string>{});
    EXPECT_TRUE(acl.GetAdmin().empty());
}

// Read権限操作
TEST_F(NbAclFT, ReadOperation) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    // 追加
    acl.AddEntry(NbAclPermission::READ, "entry");
    FTUtil::CompareList(list<string>{"g:authenticated", "entry"}, acl.GetRead());

    // 空文字
    acl.AddEntry(NbAclPermission::READ, "");
    FTUtil::CompareList(list<string>{"g:authenticated", "entry"}, acl.GetRead());

    // 重複
    acl.AddEntry(NbAclPermission::READ, "entry");
    FTUtil::CompareList(list<string>{"g:authenticated", "entry"}, acl.GetRead());

    // 削除
    acl.RemoveEntry(NbAclPermission::READ, "entry");
    FTUtil::CompareList(list<string>{"g:authenticated"}, acl.GetRead());
}

// Write権限操作
TEST_F(NbAclFT, WriteOperation) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    // 追加
    acl.AddEntry(NbAclPermission::WRITE, "entry");
    FTUtil::CompareList(list<string>{"write1","write2","write3", "entry"}, acl.GetWrite());

    // 空文字
    acl.AddEntry(NbAclPermission::WRITE, "");
    FTUtil::CompareList(list<string>{"write1","write2","write3", "entry"}, acl.GetWrite());

    // 重複
    acl.AddEntry(NbAclPermission::WRITE, "entry");
    FTUtil::CompareList(list<string>{"write1","write2","write3", "entry"}, acl.GetWrite());

    // 削除
    acl.RemoveEntry(NbAclPermission::WRITE, "entry");
    FTUtil::CompareList(list<string>{"write1","write2","write3"}, acl.GetWrite());
}

// Create権限操作
TEST_F(NbAclFT, CreateOperation) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    // 追加
    acl.AddEntry(NbAclPermission::CREATE, "entry");
    FTUtil::CompareList(list<string>{"create1","create2", "entry"}, acl.GetCreate());

    // 空文字
    acl.AddEntry(NbAclPermission::CREATE, "");
    FTUtil::CompareList(list<string>{"create1","create2", "entry"}, acl.GetCreate());

    // 重複
    acl.AddEntry(NbAclPermission::CREATE, "entry");
    FTUtil::CompareList(list<string>{"create1","create2", "entry"}, acl.GetCreate());

    // 削除
    acl.RemoveEntry(NbAclPermission::CREATE, "entry");
    FTUtil::CompareList(list<string>{"create1","create2"}, acl.GetCreate());
}

// Update権限操作
TEST_F(NbAclFT, UpdateOperation) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    // 追加
    acl.AddEntry(NbAclPermission::UPDATE, "entry");
    FTUtil::CompareList(list<string>{"update1", "entry"}, acl.GetUpdate());

    // 空文字
    acl.AddEntry(NbAclPermission::UPDATE, "");
    FTUtil::CompareList(list<string>{"update1", "entry"}, acl.GetUpdate());

    // 重複
    acl.AddEntry(NbAclPermission::UPDATE, "entry");
    FTUtil::CompareList(list<string>{"update1", "entry"}, acl.GetUpdate());

    // 削除
    acl.RemoveEntry(NbAclPermission::UPDATE, "entry");
    FTUtil::CompareList(list<string>{"update1"}, acl.GetUpdate());
}

// Delete権限操作
TEST_F(NbAclFT, DeleteOperation) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    // 追加
    acl.AddEntry(NbAclPermission::DELETE, "entry");
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3", "entry"}, acl.GetDelete());

    // 空文字
    acl.AddEntry(NbAclPermission::DELETE, "");
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3", "entry"}, acl.GetDelete());

    // 重複
    acl.AddEntry(NbAclPermission::DELETE, "entry");
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3", "entry"}, acl.GetDelete());

    // 削除
    acl.RemoveEntry(NbAclPermission::DELETE, "entry");
    FTUtil::CompareList(list<string>{"delete1","delete2","delete3"}, acl.GetDelete());
}

// Admin権限操作
TEST_F(NbAclFT, AdminOperation) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    // 追加
    acl.AddEntry(NbAclPermission::ADMIN, "entry");
    FTUtil::CompareList(list<string>{"adminid1","adminid2", "entry"}, acl.GetAdmin());

    // 空文字
    acl.AddEntry(NbAclPermission::ADMIN, "");
    FTUtil::CompareList(list<string>{"adminid1","adminid2", "entry"}, acl.GetAdmin());

    // 重複
    acl.AddEntry(NbAclPermission::ADMIN, "entry");
    FTUtil::CompareList(list<string>{"adminid1","adminid2", "entry"}, acl.GetAdmin());

    // 削除
    acl.RemoveEntry(NbAclPermission::ADMIN, "entry");
    FTUtil::CompareList(list<string>{"adminid1","adminid2"}, acl.GetAdmin());
}

// Owner操作
TEST_F(NbAclFT, OwnerOperation) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    // 変更
    acl.SetOwner("ownerId_change");
    EXPECT_EQ(string("ownerId_change"), acl.GetOwner());    

    // 変更（空）
    acl.SetOwner("");
    EXPECT_TRUE(acl.GetOwner().empty());
}

// Json変換
TEST_F(NbAclFT, ConvertJson) {
    NbJsonObject json(kDefaultAclString);
    NbAcl acl(json);

    // JSONオブジェクト
    EXPECT_EQ(json, acl.ToJsonObject());
    // JSON文字列
    EXPECT_EQ(json.ToJsonString(), acl.ToJsonString());
}

// オブジェクト作成（権限なし、未ログイン）
TEST_F(NbAclFT, CreateObjectNoACL) {
    NbAcl acl;

    NbObject object = NbObjectBucket(service_, kObjectBucketName).NewObject();
    object.SetAcl(acl);

    NbResult<NbObject> result = object.Save(true);
    EXPECT_TRUE(result.IsSuccess());
    FTUtil::CompareAcl(acl, result.GetSuccessData().GetAcl());
}

// オブジェクト作成（r,w,c,u,dにグループ名を設定、未ログイン）
TEST_F(NbAclFT, CreateObjectGroup) {
    NbAcl acl;
    acl.AddEntry(NbAclPermission::READ, "g:anonymous");
    acl.AddEntry(NbAclPermission::WRITE, "g:authenticated");
    acl.AddEntry(NbAclPermission::CREATE, "g:anonymous");
    acl.AddEntry(NbAclPermission::UPDATE, "g:authenticated");
    acl.AddEntry(NbAclPermission::DELETE, "g:anonymous");

    NbObject object = NbObjectBucket(service_, kObjectBucketName).NewObject();
    object.SetAcl(acl);

    NbResult<NbObject> result = object.Save(true);
    EXPECT_TRUE(result.IsSuccess());
    FTUtil::CompareAcl(acl, result.GetSuccessData().GetAcl());
}

// オブジェクト作成（r,w,c,u,dにユーザIDを設定、未ログイン）
TEST_F(NbAclFT, CreateObjectUserId) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(login.IsSuccess());
    string userId = login.GetSuccessData().GetUserId();
    ASSERT_TRUE(NbUser::Logout(service_).IsSuccess());

    NbAcl acl;
    acl.AddEntry(NbAclPermission::READ, userId);
    acl.AddEntry(NbAclPermission::WRITE, userId);
    acl.AddEntry(NbAclPermission::CREATE, userId);
    acl.AddEntry(NbAclPermission::UPDATE, userId);
    acl.AddEntry(NbAclPermission::DELETE, userId);

    NbObject object = NbObjectBucket(service_, kObjectBucketName).NewObject();
    object.SetAcl(acl);

    NbResult<NbObject> result = object.Save(true);
    EXPECT_TRUE(result.IsSuccess());
    FTUtil::CompareAcl(acl, result.GetSuccessData().GetAcl());
}

// オブジェクト作成（r,w,c,u,dに複数設定、未ログイン）
TEST_F(NbAclFT, CreateObjectMultiple) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(login.IsSuccess());
    string userId = login.GetSuccessData().GetUserId();
    ASSERT_TRUE(NbUser::Logout(service_).IsSuccess());

    NbAcl acl;
    acl.AddEntry(NbAclPermission::READ, "g:anonymous");
    acl.AddEntry(NbAclPermission::READ, "g:test");
    acl.AddEntry(NbAclPermission::WRITE, "g:authenticated");
    acl.AddEntry(NbAclPermission::WRITE, userId);
    acl.AddEntry(NbAclPermission::CREATE, userId);
    acl.AddEntry(NbAclPermission::CREATE, "g:anonymous");
    acl.AddEntry(NbAclPermission::UPDATE, "g:authenticated");
    acl.AddEntry(NbAclPermission::UPDATE, "g:test");
    acl.AddEntry(NbAclPermission::DELETE, "g:anonymous");
    acl.AddEntry(NbAclPermission::DELETE, userId);

    NbObject object = NbObjectBucket(service_, kObjectBucketName).NewObject();
    object.SetAcl(acl);

    NbResult<NbObject> result = object.Save(true);
    EXPECT_TRUE(result.IsSuccess());
    FTUtil::CompareAcl(acl, result.GetSuccessData().GetAcl());
}

// オブジェクト作成（ownerにユーザIDを設定、未ログイン）
TEST_F(NbAclFT, CreateObjectOwner) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(login.IsSuccess());
    string userId = login.GetSuccessData().GetUserId();
    ASSERT_TRUE(NbUser::Logout(service_).IsSuccess());

    NbAcl acl;
    acl.SetOwner(userId);

    NbObject object = NbObjectBucket(service_, kObjectBucketName).NewObject();
    object.SetAcl(acl);

    NbResult<NbObject> result = object.Save(true);
    EXPECT_TRUE(result.IsSuccess());
    FTUtil::CompareAcl(acl, result.GetSuccessData().GetAcl());
}

// オブジェクト作成（adminにグループを設定、未ログイン）
TEST_F(NbAclFT, CreateObjectAdminGroup) {
    NbAcl acl;
    acl.AddEntry(NbAclPermission::ADMIN, "g:anonymous");

    NbObject object = NbObjectBucket(service_, kObjectBucketName).NewObject();
    object.SetAcl(acl);

    NbResult<NbObject> result = object.Save(true);
    EXPECT_TRUE(result.IsSuccess());
    FTUtil::CompareAcl(acl, result.GetSuccessData().GetAcl());
}

// オブジェクト作成（adminにユーザIDを設定、未ログイン）
TEST_F(NbAclFT, CreateObjectAdminuserId) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(login.IsSuccess());
    string userId = login.GetSuccessData().GetUserId();
    ASSERT_TRUE(NbUser::Logout(service_).IsSuccess());

    NbAcl acl;
    acl.AddEntry(NbAclPermission::ADMIN, userId);

    NbObject object = NbObjectBucket(service_, kObjectBucketName).NewObject();
    object.SetAcl(acl);

    NbResult<NbObject> result = object.Save(true);
    EXPECT_TRUE(result.IsSuccess());
    FTUtil::CompareAcl(acl, result.GetSuccessData().GetAcl());
}

// オブジェクト作成（adminに複数設定、未ログイン）
TEST_F(NbAclFT, CreateObjectAdminMultiple) {
    NbAcl acl;
    acl.AddEntry(NbAclPermission::ADMIN, "g:anonymous");
    acl.AddEntry(NbAclPermission::ADMIN, "g:test");

    NbObject object = NbObjectBucket(service_, kObjectBucketName).NewObject();
    object.SetAcl(acl);

    NbResult<NbObject> result = object.Save(true);
    EXPECT_TRUE(result.IsSuccess());
    FTUtil::CompareAcl(acl, result.GetSuccessData().GetAcl());
}

// オブジェクト参照（権限なし)
TEST_F(NbAclFT, ReadObjectNoACL) {
    NbAcl acl;
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(acl);
    NbResult<NbObject> save = object.Save(true);
    EXPECT_TRUE(save.IsSuccess());
    FTUtil::CompareAcl(acl, save.GetSuccessData().GetAcl());

    NbResult<NbObject> get_object = object_bucket.GetObject(object.GetObjectId());
    ASSERT_TRUE(get_object.IsRestError());
    EXPECT_EQ(403, get_object.GetRestError().status_code);
    
}

// オブジェクト参照（誰でも参照可能)
TEST_F(NbAclFT, ReadObjectAnonymous) {
    NbAcl acl;
    acl.AddEntry(NbAclPermission::READ, "g:anonymous");
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(acl);
    NbResult<NbObject> save = object.Save(true);
    EXPECT_TRUE(save.IsSuccess());
    FTUtil::CompareAcl(acl, save.GetSuccessData().GetAcl());

    // 未ログインで参照
    NbResult<NbObject> get_object = object_bucket.GetObject(object.GetObjectId());
    ASSERT_TRUE(get_object.IsSuccess());

    // ログインして参照
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(login.IsSuccess());
    get_object = object_bucket.GetObject(object.GetObjectId());
    ASSERT_TRUE(get_object.IsSuccess());    
}

// オブジェクト更新（権限なし)
TEST_F(NbAclFT, UpdateObjectNoACL) {
    NbAcl acl;
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(acl);
    NbResult<NbObject> save = object.Save(true);
    EXPECT_TRUE(save.IsSuccess());
    FTUtil::CompareAcl(acl, save.GetSuccessData().GetAcl());

    NbResult<NbObject> update_object = object.Save();
    ASSERT_TRUE(update_object.IsRestError());
    EXPECT_EQ(403, update_object.GetRestError().status_code);
}

// オブジェクト更新（認証ユーザのみ更新可能)
TEST_F(NbAclFT, UpdateObjectAuthenticated) {
    NbAcl acl;
    acl.AddEntry(NbAclPermission::UPDATE, "g:authenticated");
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(acl);
    NbResult<NbObject> save = object.Save(true);
    EXPECT_TRUE(save.IsSuccess());
    FTUtil::CompareAcl(acl, save.GetSuccessData().GetAcl());

    // 未ログインで更新
    NbResult<NbObject> update_object = object.Save();
    ASSERT_TRUE(update_object.IsRestError());
    EXPECT_EQ(403, update_object.GetRestError().status_code);

    // ログインして
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(login.IsSuccess());
    update_object = object.Save();
    ASSERT_TRUE(update_object.IsSuccess());
}

// オブジェクト削除（権限なし)
TEST_F(NbAclFT, DeleteObjectNoACL) {
    NbAcl acl;
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(acl);
    NbResult<NbObject> save = object.Save(true);
    EXPECT_TRUE(save.IsSuccess());
    FTUtil::CompareAcl(acl, save.GetSuccessData().GetAcl());

    NbResult<NbObject> delete_object = object.DeleteObject();
    ASSERT_TRUE(delete_object.IsRestError());
    EXPECT_EQ(403, delete_object.GetRestError().status_code);
}

// オブジェクト削除（ユーザのみ削除可能)
TEST_F(NbAclFT, DeleteObjectUser) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(login.IsSuccess());
    string user_id = login.GetSuccessData().GetUserId();
    ASSERT_TRUE(NbUser::Logout(service_).IsSuccess());    

    NbAcl acl;
    acl.AddEntry(NbAclPermission::DELETE, user_id);
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(acl);
    NbResult<NbObject> save = object.Save(true);
    EXPECT_TRUE(save.IsSuccess());
    FTUtil::CompareAcl(acl, save.GetSuccessData().GetAcl());

    // 未ログインで更新
    NbResult<NbObject> delete_object = object.DeleteObject();
    ASSERT_TRUE(delete_object.IsRestError());
    EXPECT_EQ(403, delete_object.GetRestError().status_code);

    // ログイン状態で削除
    login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(login.IsSuccess());
    delete_object = object.DeleteObject();
    ASSERT_TRUE(delete_object.IsSuccess());
}

// オブジェクト ACL変更（権限なし)
TEST_F(NbAclFT, ObjectAdminNoACL) {
    NbAcl acl;
    acl.AddEntry(NbAclPermission::WRITE, "g:anonymous");
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(acl);
    NbResult<NbObject> save = object.Save(true);
    EXPECT_TRUE(save.IsSuccess());
    FTUtil::CompareAcl(acl, save.GetSuccessData().GetAcl());

    acl.AddEntry(NbAclPermission::DELETE, "g:anonymous");
    object.SetAcl(acl);
    NbResult<NbObject> update_object = object.Save();
    ASSERT_TRUE(update_object.IsRestError());
    EXPECT_EQ(403, update_object.GetRestError().status_code);
}

// オブジェクト ACL変更（誰でもACL変更可能)
TEST_F(NbAclFT, ObjectAdminAnonymous) {
    NbAcl acl;
    acl.AddEntry(NbAclPermission::WRITE, "g:anonymous");
    acl.AddEntry(NbAclPermission::ADMIN, "g:anonymous");
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(acl);
    NbResult<NbObject> save = object.Save(true);
    EXPECT_TRUE(save.IsSuccess());
    FTUtil::CompareAcl(acl, save.GetSuccessData().GetAcl());

    // 未ログインで更新
    acl.AddEntry(NbAclPermission::DELETE, "g:anonymous");
    object.SetAcl(acl);
    NbResult<NbObject> update_object = object.Save();
    ASSERT_TRUE(update_object.IsSuccess());

    // ログイン状態で更新
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(login.IsSuccess());
    acl.AddEntry(NbAclPermission::UPDATE, "g:anonymous");
    object.SetAcl(acl);
    update_object = object.Save();
    ASSERT_TRUE(update_object.IsSuccess());
}

// オブジェクト オーナ情報
TEST_F(NbAclFT, ObjectOwner) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    ASSERT_TRUE(login.IsSuccess());
    string user_id = login.GetSuccessData().GetUserId();

    NbAcl acl;
    acl.SetOwner(user_id);
    NbObjectBucket object_bucket(service_, kObjectBucketName);
    NbObject object = object_bucket.NewObject();
    object.SetAcl(acl);
    NbResult<NbObject> save = object.Save(true);
    EXPECT_TRUE(save.IsSuccess());
    FTUtil::CompareAcl(acl, save.GetSuccessData().GetAcl());    

    // 別ユーザにスイッチ
    ASSERT_TRUE(NbUser::LoginWithUsername(service_, kUserNameB, kPasswordB).IsSuccess());

    NbResult<NbObject> get_object = object_bucket.GetObject(object.GetObjectId());
    ASSERT_TRUE(get_object.IsRestError());
    EXPECT_EQ(403, get_object.GetRestError().status_code);
    
    NbResult<NbObject> update_object = object.Save();
    ASSERT_TRUE(update_object.IsRestError());
    EXPECT_EQ(403, update_object.GetRestError().status_code);

    NbResult<NbObject> delete_object = object.DeleteObject();
    ASSERT_TRUE(delete_object.IsRestError());
    EXPECT_EQ(403, delete_object.GetRestError().status_code);
    
    // オーナユーザにスイッチ
    ASSERT_TRUE(NbUser::LoginWithUsername(service_, kUserName, kPassword).IsSuccess());

    get_object = object_bucket.GetObject(object.GetObjectId());
    ASSERT_TRUE(get_object.IsSuccess());

    update_object = object.Save();
    ASSERT_TRUE(update_object.IsSuccess());

    delete_object = object.DeleteObject();
    ASSERT_TRUE(delete_object.IsSuccess());
}
} //namespace necbaas
