#include "gtest/gtest.h"
#include "necbaas/nb_acl.h"

namespace necbaas {

using std::string;

const string kJsonAclString{R"({"owner":"ownerid","r":["g:authenticated"],"w":["write1","write2","write3"],"c":["create1"],"u":["update1"],"d":["delete1"],"admin":["adminid"]})"};

const string kJsonAclString2{R"({"owner":"ownerid",
                            "r":["read3","read1", "read2","read1"],
                            "w":["write3","write1","write2","write1"],
                            "c":["create3","create1","create2","create1"],
                            "u":["update3","update1","update2","update1"],
                            "d":["delete3","delete1","delete2","delete1"],
                            "admin":["admin3","admin1","admin2","admin1"]})"};

//NbAcl::NbAcl(デフォルトコンストラクタ)
TEST(NbAcl, NbAcl) {
    NbAcl acl;
    EXPECT_TRUE(acl.GetCreate().empty());
    EXPECT_TRUE(acl.GetDelete().empty());
    EXPECT_TRUE(acl.GetRead().empty());
    EXPECT_TRUE(acl.GetUpdate().empty());
    EXPECT_TRUE(acl.GetWrite().empty());
    EXPECT_TRUE(acl.GetAdmin().empty());
    EXPECT_TRUE(acl.GetOwner().empty());
}

//NbAcl::NbAcl(JSON)
TEST(NbAcl, NbAclJson) {
    NbJsonObject json(kJsonAclString);
    NbAcl acl(json);
    EXPECT_EQ(1, acl.GetCreate().size());
    EXPECT_EQ("create1", acl.GetCreate().front());
    EXPECT_EQ(1, acl.GetDelete().size());
    EXPECT_EQ("delete1", acl.GetDelete().front());
    EXPECT_EQ(1, acl.GetRead().size());
    EXPECT_EQ("g:authenticated", acl.GetRead().front());
    EXPECT_EQ(1, acl.GetUpdate().size());
    EXPECT_EQ("update1", acl.GetUpdate().front());
    EXPECT_EQ(3, acl.GetWrite().size());
    auto itr = acl.GetWrite().begin();
    EXPECT_EQ("write1", *itr);
    EXPECT_EQ("write2", *(++itr));
    EXPECT_EQ("write3", *(++itr));
    EXPECT_EQ(1, acl.GetAdmin().size());
    EXPECT_EQ("adminid", acl.GetAdmin().front());
    EXPECT_EQ("ownerid", acl.GetOwner());
}

//NbAcl::SetJson
TEST(NbAcl, SetJson) {
    NbJsonObject json(kJsonAclString2);
    NbAcl acl;
    acl.SetJson(json);
    EXPECT_EQ(3, acl.GetCreate().size());
    auto itr = acl.GetCreate().begin();
    EXPECT_EQ("create1", *itr);
    EXPECT_EQ("create2", *(++itr));
    EXPECT_EQ("create3", *(++itr));
    EXPECT_EQ(3, acl.GetDelete().size());
    itr = acl.GetDelete().begin();
    EXPECT_EQ("delete1", *itr);
    EXPECT_EQ("delete2", *(++itr));
    EXPECT_EQ("delete3", *(++itr));
    EXPECT_EQ(3, acl.GetRead().size());
    itr = acl.GetRead().begin();
    EXPECT_EQ("read1", *itr);
    EXPECT_EQ("read2", *(++itr));
    EXPECT_EQ("read3", *(++itr));
    EXPECT_EQ(3, acl.GetUpdate().size());
    itr = acl.GetUpdate().begin();
    EXPECT_EQ("update1", *itr);
    EXPECT_EQ("update2", *(++itr));
    EXPECT_EQ("update3", *(++itr));
    EXPECT_EQ(3, acl.GetWrite().size());
    itr = acl.GetWrite().begin();
    EXPECT_EQ("write1", *itr);
    EXPECT_EQ("write2", *(++itr));
    EXPECT_EQ("write3", *(++itr));
    EXPECT_EQ(3, acl.GetAdmin().size());
    itr = acl.GetAdmin().begin();
    EXPECT_EQ("admin1", *itr);
    EXPECT_EQ("admin2", *(++itr));
    EXPECT_EQ("admin3", *(++itr));
    EXPECT_EQ("ownerid", acl.GetOwner());
}

static void AclSetterGetterTest(NbAcl &acl,
                                std::function<const std::list<string>&()> getter,
                                std::function<void(const std::list<string>&)> setter,
                                NbAclPermission permission) {
    //Entry一括設定
    std::list<string> data{"id5", "id1", "id3"};
    setter(data);
    EXPECT_EQ(3, getter().size());
    auto itr = getter().begin();
    EXPECT_EQ("id1", *itr);
    EXPECT_EQ("id3", *(++itr));
    EXPECT_EQ("id5", *(++itr));

    //Entry追加
    acl.AddEntry(permission, "id4");
    EXPECT_EQ(4, getter().size());
    itr = getter().begin();
    EXPECT_EQ("id1", *itr);
    EXPECT_EQ("id3", *(++itr));
    EXPECT_EQ("id4", *(++itr));
    EXPECT_EQ("id5", *(++itr));

    //Entry重複
    acl.AddEntry(permission, "id1");
    EXPECT_EQ(4, getter().size());
    itr = getter().begin();
    EXPECT_EQ("id1", *itr);
    EXPECT_EQ("id3", *(++itr));
    EXPECT_EQ("id4", *(++itr));
    EXPECT_EQ("id5", *(++itr));
    
    //Entry空
    acl.AddEntry(permission, "");
    EXPECT_EQ(4, getter().size());
    itr = getter().begin();
    EXPECT_EQ("id1", *itr);
    EXPECT_EQ("id3", *(++itr));
    EXPECT_EQ("id4", *(++itr));
    EXPECT_EQ("id5", *(++itr));

    //Entry削除
    acl.RemoveEntry(permission, "id3");
    EXPECT_EQ(3, getter().size());
    itr = getter().begin();
    EXPECT_EQ("id1", *itr);
    EXPECT_EQ("id4", *(++itr));
    EXPECT_EQ("id5", *(++itr));

    //Entry削除(該当なし)
    acl.RemoveEntry(permission, "id6");
    EXPECT_EQ(3, getter().size());
    itr = getter().begin();
    EXPECT_EQ("id1", *itr);
    EXPECT_EQ("id4", *(++itr));
    EXPECT_EQ("id5", *(++itr));    

    //Entry削除(空)
    acl.RemoveEntry(permission, "");
    EXPECT_EQ(3, getter().size());
    itr = getter().begin();
    EXPECT_EQ("id1", *itr);
    EXPECT_EQ("id4", *(++itr));
    EXPECT_EQ("id5", *(++itr));

    //Entry全削除
    data.clear();
    setter(data);
    EXPECT_EQ(0, getter().size());
}

//NbAcl::Create権限操作
TEST(NbAcl, Create) {
    NbAcl acl;
    auto getter = std::bind(&NbAcl::GetCreate, &acl);
    auto setter = std::bind(&NbAcl::SetCreate, &acl, std::placeholders::_1);
    AclSetterGetterTest(acl, getter, setter, NbAclPermission::CREATE);
}

//NbAcl::Delete権限操作
TEST(NbAcl, Delete) {
    NbAcl acl;
    auto getter = std::bind(&NbAcl::GetDelete, &acl);
    auto setter = std::bind(&NbAcl::SetDelete, &acl, std::placeholders::_1);
    AclSetterGetterTest(acl, getter, setter, NbAclPermission::DELETE);
}

//NbAcl::Read権限操作
TEST(NbAcl, Read) {
    NbAcl acl;
    auto getter = std::bind(&NbAcl::GetRead, &acl);
    auto setter = std::bind(&NbAcl::SetRead, &acl, std::placeholders::_1);
    AclSetterGetterTest(acl, getter, setter, NbAclPermission::READ);
}

//NbAcl::Update権限操作
TEST(NbAcl, Update) {
    NbAcl acl;
    auto getter = std::bind(&NbAcl::GetUpdate, &acl);
    auto setter = std::bind(&NbAcl::SetUpdate, &acl, std::placeholders::_1);
    AclSetterGetterTest(acl, getter, setter, NbAclPermission::UPDATE);
}

//NbAcl::Write権限操作
TEST(NbAcl, Write) {
    NbAcl acl;
    auto getter = std::bind(&NbAcl::GetWrite, &acl);
    auto setter = std::bind(&NbAcl::SetWrite, &acl, std::placeholders::_1);
    AclSetterGetterTest(acl, getter, setter, NbAclPermission::WRITE);
}

//NbAcl::Admin権限操作
TEST(NbAcl, Admin) {
    NbAcl acl;
    auto getter = std::bind(&NbAcl::GetAdmin, &acl);
    auto setter = std::bind(&NbAcl::SetAdmin, &acl, std::placeholders::_1);
    AclSetterGetterTest(acl, getter, setter, NbAclPermission::ADMIN);
}

//NbAcl::Owner操作
TEST(NbAcl, Owner) {
    NbAcl acl;
    acl.SetOwner("ownner1");
    EXPECT_EQ(string("ownner1"), acl.GetOwner());

    acl.SetOwner("");
    EXPECT_EQ(string(""), acl.GetOwner());
}

//NbAcl::ToJsonString(ToJsonObjectを含む)
TEST(NbAcl, ToJsonString) {
    NbAcl acl;
    NbJsonObject json(R"({"owner":"","r":[],"w":[],"c":[],"u":[],"d":[],"admin":[]})");
    EXPECT_EQ(json.ToJsonString(), acl.ToJsonString());

    json = NbJsonObject(kJsonAclString);
    acl.SetJson(json);
    EXPECT_EQ(json.ToJsonString(), acl.ToJsonString());
}

//NbAcl::CreateAclForAnonymous
TEST(NbAcl, CreateAclForAnonymous) {
    NbAcl acl = NbAcl::CreateAclForAnonymous();

    EXPECT_EQ(1, acl.GetRead().size());
    EXPECT_EQ("g:anonymous", acl.GetRead().front());

    EXPECT_EQ(1, acl.GetWrite().size());
    EXPECT_EQ("g:anonymous", acl.GetWrite().front());

    EXPECT_EQ(1, acl.GetAdmin().size());
    EXPECT_EQ("g:anonymous", acl.GetAdmin().front());

    EXPECT_EQ(0, acl.GetCreate().size());
    EXPECT_EQ(0, acl.GetDelete().size());
    EXPECT_EQ(0, acl.GetUpdate().size());

    EXPECT_TRUE(acl.GetOwner().empty());
}

//NbAcl::CreateAclForAuthenticated
TEST(NbAcl, CreateAclForAuthenticated) {
    NbAcl acl = NbAcl::CreateAclForAuthenticated();

    EXPECT_EQ(1, acl.GetRead().size());
    EXPECT_EQ("g:authenticated", acl.GetRead().front());

    EXPECT_EQ(1, acl.GetWrite().size());
    EXPECT_EQ("g:authenticated", acl.GetWrite().front());

    EXPECT_EQ(1, acl.GetAdmin().size());
    EXPECT_EQ("g:authenticated", acl.GetAdmin().front());

    EXPECT_EQ(0, acl.GetCreate().size());
    EXPECT_EQ(0, acl.GetDelete().size());
    EXPECT_EQ(0, acl.GetUpdate().size());

    EXPECT_TRUE(acl.GetOwner().empty());
}

//NbAcl::CreateAclForUser
TEST(NbAcl, CreateAclForUser) {
    NbUser user;
    NbJsonObject json;
    json["_id"] = "ownerid";
    NbUserEntity user_entity(json);
    user.SetUserEntity(user_entity);
    
    NbAcl acl = NbAcl::CreateAclForUser(user);

    EXPECT_EQ(1, acl.GetRead().size());
    EXPECT_EQ("ownerid", acl.GetRead().front());

    EXPECT_EQ(1, acl.GetWrite().size());
    EXPECT_EQ("ownerid", acl.GetWrite().front());

    EXPECT_EQ(1, acl.GetAdmin().size());
    EXPECT_EQ("ownerid", acl.GetAdmin().front());

    EXPECT_EQ(0, acl.GetCreate().size());
    EXPECT_EQ(0, acl.GetDelete().size());
    EXPECT_EQ(0, acl.GetUpdate().size());

    EXPECT_EQ("ownerid", acl.GetOwner());
}

//NbAcl::CreateAclForUser(空)
TEST(NbAcl, CreateAclForUserEmpty) {
    NbUser user;
    NbJsonObject json;
    json["_id"] = "";
    NbUserEntity user_entity(json);
    user.SetUserEntity(user_entity);

    NbAcl acl = NbAcl::CreateAclForUser(user);

    EXPECT_EQ(0, acl.GetRead().size());
    EXPECT_EQ(0, acl.GetWrite().size());
    EXPECT_EQ(0, acl.GetAdmin().size());
    EXPECT_EQ(0, acl.GetCreate().size());
    EXPECT_EQ(0, acl.GetDelete().size());
    EXPECT_EQ(0, acl.GetUpdate().size());

    EXPECT_TRUE(acl.GetOwner().empty());
}

//NbAcl::CreateAclFor
TEST(NbAcl, CreateAclFor) {
    std::list<string> entries{"entry1", "entry2", "entry3"};
    NbAcl acl = NbAcl::CreateAclFor(entries);

    EXPECT_EQ(3, acl.GetRead().size());
    auto itr = acl.GetRead().begin();
    EXPECT_EQ("entry1", *itr);
    EXPECT_EQ("entry2", *(++itr));
    EXPECT_EQ("entry3", *(++itr));

    EXPECT_EQ(3, acl.GetWrite().size());
    itr = acl.GetWrite().begin();
    EXPECT_EQ("entry1", *itr);
    EXPECT_EQ("entry2", *(++itr));
    EXPECT_EQ("entry3", *(++itr));

    EXPECT_EQ(3, acl.GetAdmin().size());
    itr = acl.GetAdmin().begin();
    EXPECT_EQ("entry1", *itr);
    EXPECT_EQ("entry2", *(++itr));
    EXPECT_EQ("entry3", *(++itr));

    EXPECT_EQ(0, acl.GetCreate().size());
    EXPECT_EQ(0, acl.GetDelete().size());
    EXPECT_EQ(0, acl.GetUpdate().size());

    EXPECT_TRUE(acl.GetOwner().empty());
}

//NbAcl::CreateAclFor(空)
TEST(NbAcl, CreateAclForEmpty) {
    std::list<string> entries{};
    NbAcl acl = NbAcl::CreateAclFor(entries);

    EXPECT_EQ(0, acl.GetRead().size());
    EXPECT_EQ(0, acl.GetWrite().size());
    EXPECT_EQ(0, acl.GetAdmin().size());
    EXPECT_EQ(0, acl.GetCreate().size());
    EXPECT_EQ(0, acl.GetDelete().size());
    EXPECT_EQ(0, acl.GetUpdate().size());

    EXPECT_TRUE(acl.GetOwner().empty());
}
} //namespace necbaas
