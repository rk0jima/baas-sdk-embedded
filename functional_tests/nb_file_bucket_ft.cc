#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_file_bucket.h"
#include "necbaas/nb_user.h"
#include "necbaas/nb_acl.h"
#include "necbaas/internal/nb_utility.h"

namespace necbaas {

using std::string;
using std::vector;
using std::list;
using std::shared_ptr;

class NbFileBucketFT : public ::testing::Test {
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
        FTUtil::DeleteAllFile();
        std::remove(kDownloadFile.c_str());
    }

    // サービスインスタンスは使いまわす
    static shared_ptr<NbService> service_;
};

shared_ptr<NbService> NbFileBucketFT::service_;

// ファイルの新規アップロード.未ログイン
// ACL指定なし、キャッシュ禁止
TEST_F(NbFileBucketFT, UploadNewFileNoLogin) {
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();
    expected_acl.SetAdmin(list<string>());
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));

    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(kFileName, response.GetFileName());
    EXPECT_EQ(kContentType, response.GetContentType());
    FTUtil::CompareAcl(expected_acl, response.GetAcl());
    EXPECT_EQ(20, response.GetLength());
    EXPECT_NE(0, response.GetCreatedTime().tm_year);
    EXPECT_NE(0, response.GetUpdatedTime().tm_year);
    EXPECT_FALSE(response.GetMetaETag().empty());
    EXPECT_FALSE(response.GetFileETag().empty());
    EXPECT_TRUE(response.IsCacheDisabled());
}

// ファイルの新規アップロード.未ログイン
// ACL指定あり、キャッシュ許可
TEST_F(NbFileBucketFT, UploadNewFileNoLoginWithAcl) {
    NbAcl expected_acl = NbAcl::CreateAclForAuthenticated();
    expected_acl.SetOwner("TestUser");
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));

    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, expected_acl);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(kFileName, response.GetFileName());
    EXPECT_EQ(kContentType, response.GetContentType());
    FTUtil::CompareAcl(expected_acl, response.GetAcl());
    EXPECT_EQ(20, response.GetLength());
    EXPECT_NE(0, response.GetCreatedTime().tm_year);
    EXPECT_NE(0, response.GetUpdatedTime().tm_year);
    EXPECT_FALSE(response.GetMetaETag().empty());
    EXPECT_FALSE(response.GetFileETag().empty());
    EXPECT_FALSE(response.IsCacheDisabled());
}

// ファイルの新規アップロード.未ログイン
// ACL指定あり（Owner未設定）、キャッシュ許可
TEST_F(NbFileBucketFT, UploadNewFileNoLoginNoOwner) {
    NbAcl expected_acl = NbAcl::CreateAclForAuthenticated();
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));

    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, expected_acl);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(kFileName, response.GetFileName());
    EXPECT_EQ(kContentType, response.GetContentType());
    FTUtil::CompareAcl(expected_acl, response.GetAcl());
    EXPECT_EQ(20, response.GetLength());
    EXPECT_NE(0, response.GetCreatedTime().tm_year);
    EXPECT_NE(0, response.GetUpdatedTime().tm_year);
    EXPECT_FALSE(response.GetMetaETag().empty());
    EXPECT_FALSE(response.GetFileETag().empty());
    EXPECT_FALSE(response.IsCacheDisabled());
}

// ファイルの新規アップロード.ログイン済み
// ACL指定なし、キャッシュ禁止
TEST_F(NbFileBucketFT, UploadNewFileLogin) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    EXPECT_TRUE(NbUser::IsLoggedIn(service_));
    NbAcl expected_acl;
    expected_acl.SetOwner(login.GetSuccessData().GetUserId());

    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(kFileName, response.GetFileName());
    EXPECT_EQ(kContentType, response.GetContentType());
    FTUtil::CompareAcl(expected_acl, response.GetAcl());
    EXPECT_EQ(20, response.GetLength());
    EXPECT_NE(0, response.GetCreatedTime().tm_year);
    EXPECT_NE(0, response.GetUpdatedTime().tm_year);
    EXPECT_FALSE(response.GetMetaETag().empty());
    EXPECT_FALSE(response.GetFileETag().empty());
    EXPECT_TRUE(response.IsCacheDisabled());
}

// ファイルの新規アップロード.ログイン済み
// ACL指定あり、キャッシュ許可
TEST_F(NbFileBucketFT, UploadNewFileLoginWithAcl) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    EXPECT_TRUE(NbUser::IsLoggedIn(service_));
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();
    expected_acl.SetOwner(login.GetSuccessData().GetUserId());

    NbFileBucket file_bucket(service_, kFileBucketName); 

    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, expected_acl);
 
    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(kFileName, response.GetFileName());
    EXPECT_EQ(kContentType, response.GetContentType());
    FTUtil::CompareAcl(expected_acl, response.GetAcl());
    EXPECT_EQ(20, response.GetLength());
    EXPECT_NE(0, response.GetCreatedTime().tm_year);
    EXPECT_NE(0, response.GetUpdatedTime().tm_year);
    EXPECT_FALSE(response.GetMetaETag().empty());
    EXPECT_FALSE(response.GetFileETag().empty());
    EXPECT_FALSE(response.IsCacheDisabled());
}

// ファイルの新規アップロード.ログイン済み
// ACL指定あり（Owner未設定）、キャッシュ許可
TEST_F(NbFileBucketFT, UploadNewFileLoginNoOwner) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    EXPECT_TRUE(NbUser::IsLoggedIn(service_));
    NbAcl expected_acl = NbAcl::CreateAclForAuthenticated();

    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, expected_acl);

    // ownerがない場合は自動設定される
    expected_acl.SetOwner(login.GetSuccessData().GetUserId());

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(kFileName, response.GetFileName());
    EXPECT_EQ(kContentType, response.GetContentType());
    FTUtil::CompareAcl(expected_acl, response.GetAcl());
    EXPECT_EQ(20, response.GetLength());
    EXPECT_NE(0, response.GetCreatedTime().tm_year);
    EXPECT_NE(0, response.GetUpdatedTime().tm_year);
    EXPECT_FALSE(response.GetMetaETag().empty());
    EXPECT_FALSE(response.GetFileETag().empty());
    EXPECT_FALSE(response.IsCacheDisabled());
}

// ファイルの新規アップロード.ファイルサイズ0KB
TEST_F(NbFileBucketFT, UploadNewFileSize0) {
    NbAcl expected_acl = NbAcl::CreateAclForAuthenticated();

    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath("0.txt"), kContentType, expected_acl);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();
    EXPECT_EQ(0, response.GetLength());
}

// ファイルの新規アップロード.ファイル名日本語
TEST_F(NbFileBucketFT, UploadNewFileNameJapanese) {
    NbAcl expected_acl = NbAcl::CreateAclForAuthenticated();

    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile("日本語.txt", FTUtil::MakeFilePath(kUploadFile), kContentType, expected_acl);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();
    EXPECT_EQ(string("日本語.txt"), response.GetFileName());
}

// ファイルの新規アップロード.不正なファイル名
TEST_F(NbFileBucketFT, UploadNewFileInvalidFilename) {
    string file_name{"UploadFileA"};
    while (file_name.size() <= 900 ) {
        file_name += "UploadFileA";
    }
    NbAcl expected_acl = NbAcl::CreateAclForAuthenticated();

    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(file_name, FTUtil::MakeFilePath(kUploadFile), kContentType, expected_acl);

    // 戻り値確認
    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(400, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));
}

// ファイルの新規アップロード.ファイル名重複
TEST_F(NbFileBucketFT, UploadNewFileConflictDuplicateFilename) {
    NbAcl expected_acl = NbAcl::CreateAclForAuthenticated();

    // 新規アップロード
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, expected_acl);
    ASSERT_TRUE(result.IsSuccess());

    // 同じファイル名で新規アップロード
    result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, expected_acl);

    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(409, result.GetRestError().status_code);
    NbJsonObject response(result.GetRestError().reason);
    EXPECT_EQ(string("duplicate_filename"), response.GetString("reasonCode"));
    EXPECT_EQ(string("Duplicate File Name"), response.GetString("detail"));
}

// ファイルの新規アップロード.論理削除データと同名
TEST_F(NbFileBucketFT, UploadNewFileConflictDeletedMark) {
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();

    // 新規アップロード
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, expected_acl);
    ASSERT_TRUE(result.IsSuccess());

    // DeleteMark
    result = file_bucket.DeleteFile(result.GetSuccessData(), true);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_TRUE(result.GetSuccessData().IsDeleted());

    // 同じファイル名で新規アップロード
    result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile2), kContentType, expected_acl);

    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();
    EXPECT_EQ(30, response.GetLength());
    EXPECT_FALSE(response.IsDeleted());
}

// ファイルの新規アップロード.バケットのcontentACLにcreate権限がない
TEST_F(NbFileBucketFT, UploadNewFilePermissionBucketContentAclCreateNone) {
    string content_acl = R"({"r":["g:anonymous"],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("file", kFileBucketName, "", content_acl);

    // 新規アップロード
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType);

    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(403, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));

    FTUtil::CreateBucket("file", kFileBucketName);
}

// ファイルの新規アップロード.バケットのcontentACLにcreate権限がない(AppKeyにマスターキーを使用する)
TEST_F(NbFileBucketFT, UploadNewFileMasterKeyAclNone) {
    string content_acl = R"({"r":["g:anonymous"],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("file", kFileBucketName, "", content_acl);

    shared_ptr<NbService> master_service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);    

    // 新規アップロード
    NbFileBucket file_bucket(master_service, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType);

    ASSERT_TRUE(result.IsSuccess());

    FTUtil::CreateBucket("file", kFileBucketName);
}

// ファイルの新規アップロード.バケットのcontentACLにcreate権限がある(AppKeyにマスターキーを使用する)
TEST_F(NbFileBucketFT, UploadNewFileMasterKeyAclExists) {
    shared_ptr<NbService> master_service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);

    // 新規アップロード
    NbFileBucket file_bucket(master_service, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType);

    ASSERT_TRUE(result.IsSuccess());
}

// ファイルの新規アップロード.ファイル名が空文字
TEST_F(NbFileBucketFT, UploadNewFileFilenameEmpty) {
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kEmpty, FTUtil::MakeFilePath(kUploadFile), kContentType, true);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

// ファイルの新規アップロード.ファイルパスが空文字
TEST_F(NbFileBucketFT, UploadNewFileFilepathEmpty) {
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, kEmpty, kContentType, true);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

// ファイルの新規アップロード.content-Typeが空文字
TEST_F(NbFileBucketFT, UploadNewFileContentTypeEmpty) {
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kEmpty, true);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

// ファイルの新規アップロード.ファイルオープンエラー
TEST_F(NbFileBucketFT, UploadNewFileOpenError) {
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath("invalidFileName"), kContentType, true);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_OPEN_FILE, result.GetResultCode());
}

// ファイルの新規アップロード.バケット名が空文字
TEST_F(NbFileBucketFT, UploadNewFileBucketNameEmpty) {
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();
    NbFileBucket file_bucket(service_, kEmpty);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

static NbJsonObject FileMetadataToJsonObject(const NbFileMetadata &meta) {
    NbJsonObject json;
    json["filename"] = meta.GetFileName();
    json["contentType"] = meta.GetContentType();
    json["metaETag"] = meta.GetMetaETag();
    json["fileETag"] = meta.GetFileETag();
    return json;
}

// ファイルの更新アップロード.未ログイン
// メタデータによる情報一括指定
TEST_F(NbFileBucketFT, UploadUpdateFileWithMetadata) {
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType,
                                                                 expected_acl, true);
    NbFileMetadata meta = newfile.GetSuccessData();

    // 更新時間を変化させるためsleep
    sleep(1);

    NbJsonObject meta_json = FileMetadataToJsonObject(meta);
    meta_json["contentType"] = "text/html";
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(FTUtil::MakeFilePath(kUploadFile2), NbFileMetadata(kFileBucketName, meta_json));

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(kFileName, response.GetFileName());
    EXPECT_EQ("text/html", response.GetContentType());
    FTUtil::CompareAcl(expected_acl, response.GetAcl());
    EXPECT_EQ(30, response.GetLength());
    FTUtil::CompareTime(meta.GetCreatedTime(), response.GetCreatedTime(), true);
    FTUtil::CompareTime(meta.GetUpdatedTime(), response.GetUpdatedTime(), false);
    EXPECT_NE(meta.GetMetaETag(), response.GetMetaETag());
    EXPECT_NE(meta.GetFileETag(), response.GetFileETag());
    EXPECT_EQ(meta.IsCacheDisabled(), response.IsCacheDisabled());
}

// ファイルの更新アップロード.ログイン済み
// 情報個別指定
TEST_F(NbFileBucketFT, UploadUpdateFileWithString) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    EXPECT_TRUE(NbUser::IsLoggedIn(service_));
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();
    expected_acl.SetOwner(login.GetSuccessData().GetUserId());
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType,
                                                                 expected_acl, true);
    NbFileMetadata meta = newfile.GetSuccessData();

    // 更新時間を変化させるためsleep
    sleep(1);

    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, FTUtil::MakeFilePath(kUploadFile2), "application/json",
                                                                   meta.GetMetaETag(), meta.GetFileETag());

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(kFileName, response.GetFileName());
    EXPECT_EQ("application/json", response.GetContentType());
    FTUtil::CompareAcl(expected_acl, response.GetAcl());
    EXPECT_EQ(30, response.GetLength());
    FTUtil::CompareTime(meta.GetCreatedTime(), response.GetCreatedTime(), true);
    FTUtil::CompareTime(meta.GetUpdatedTime(), response.GetUpdatedTime(), false);
    EXPECT_NE(meta.GetMetaETag(), response.GetMetaETag());
    EXPECT_NE(meta.GetFileETag(), response.GetFileETag());
    EXPECT_EQ(meta.IsCacheDisabled(), response.IsCacheDisabled());
}

// ファイルの更新アップロード.パラメータを未設定(Content-Type,MetaEtag,FileEtag)
TEST_F(NbFileBucketFT, UploadUpdateFileNoOptionParams) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    EXPECT_TRUE(NbUser::IsLoggedIn(service_));
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();
    expected_acl.SetOwner(login.GetSuccessData().GetUserId());
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType,
                                                                 expected_acl, true);
    NbFileMetadata meta = newfile.GetSuccessData();

    // 更新時間を変化させるためsleep
    sleep(1);

    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, FTUtil::MakeFilePath(kUploadFile2), kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(kFileName, response.GetFileName());
    EXPECT_EQ(kContentType, response.GetContentType());
    FTUtil::CompareAcl(expected_acl, response.GetAcl());
    EXPECT_EQ(30, response.GetLength());
    FTUtil::CompareTime(meta.GetCreatedTime(), response.GetCreatedTime(), true);
    FTUtil::CompareTime(meta.GetUpdatedTime(), response.GetUpdatedTime(), false);
    EXPECT_NE(meta.GetMetaETag(), response.GetMetaETag());
    EXPECT_NE(meta.GetFileETag(), response.GetFileETag());
    EXPECT_EQ(meta.IsCacheDisabled(), response.IsCacheDisabled());
}

// ファイルの更新アップロード.ファイルサイズ0KB
TEST_F(NbFileBucketFT, UploadUpdateFileSize0) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType);
    NbFileMetadata meta = newfile.GetSuccessData();

    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, FTUtil::MakeFilePath("0.txt"), "application/json",
                                                                   meta.GetMetaETag(), meta.GetFileETag());
    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(0, response.GetLength());
}

// ファイルの更新アップロード.ファイル名日本語
TEST_F(NbFileBucketFT, UploadUpdateFileFilenameJapanese) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile("日本語.txt", FTUtil::MakeFilePath(kUploadFile), kContentType);
    NbFileMetadata meta = newfile.GetSuccessData();

    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile("日本語.txt", FTUtil::MakeFilePath(kUploadFile2), "application/json",
                                                                   meta.GetMetaETag(), meta.GetFileETag());
    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ("日本語.txt", response.GetFileName());
    EXPECT_EQ(30, response.GetLength());
}

// ファイルの更新アップロード.論理削除データ
TEST_F(NbFileBucketFT, UploadUpdateFileDeletedMark) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType);
    NbFileMetadata meta = newfile.GetSuccessData();

    // DeleteMark
    NbResult<NbFileMetadata> result = file_bucket.DeleteFile(meta, true);
    ASSERT_TRUE(result.IsSuccess());
    meta = result.GetSuccessData();
    EXPECT_TRUE(meta.IsDeleted());

    result = file_bucket.UploadUpdateFile(kFileName, FTUtil::MakeFilePath(kUploadFile2), "application/json",
                                          meta.GetMetaETag(), meta.GetFileETag());
    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(kFileName, response.GetFileName());
    EXPECT_EQ(30, response.GetLength());
    EXPECT_FALSE(result.GetSuccessData().IsDeleted());
}

// ファイルの更新アップロード.Etag不一致
TEST_F(NbFileBucketFT, UploadUpdateFileEtagMismatch) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType);
    NbFileMetadata meta = newfile.GetSuccessData();

    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, FTUtil::MakeFilePath(kUploadFile2), "application/json",
                                          "etag", meta.GetFileETag());
    // 戻り値確認
    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(409, result.GetRestError().status_code);
    NbJsonObject response(result.GetRestError().reason);
    EXPECT_EQ(string("etag_mismatch"), response.GetString("reasonCode"));
}

// ファイルの更新アップロード.バケットのcontentACLにupdate権限がない
TEST_F(NbFileBucketFT, UploadUpdateFileEtagPermissionBucketContentAclUpdateNone) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType);
    NbFileMetadata meta = newfile.GetSuccessData();

    // ACL更新
    string content_acl = R"({"r":["g:anonymous"],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("file", kFileBucketName, "", content_acl);

    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, FTUtil::MakeFilePath(kUploadFile2), "application/json",
                                                                   "etag", meta.GetFileETag());
    // 戻り値確認
    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(403, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));

    FTUtil::CreateBucket("file", kFileBucketName);
}

// ファイルの更新アップロード.ファイルなし
TEST_F(NbFileBucketFT, UploadUpdateFileNotFound) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, FTUtil::MakeFilePath(kUploadFile2), kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));
}

// ファイルの更新アップロード.マスターキー（ACLなし）
TEST_F(NbFileBucketFT, UploadUpdateFileMasterKeyAclNone) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType);
    NbFileMetadata meta = newfile.GetSuccessData();

    // ACL更新
    string content_acl = R"({"r":["g:anonymous"],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("file", kFileBucketName, "", content_acl);

    shared_ptr<NbService> master_service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
    NbFileBucket file_bucket_master(master_service, kFileBucketName);

    NbResult<NbFileMetadata> result = file_bucket_master.UploadUpdateFile(kFileName, FTUtil::MakeFilePath(kUploadFile2), "application/json",
                                                                          meta.GetMetaETag() , meta.GetFileETag());
    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());

    FTUtil::CreateBucket("file", kFileBucketName);
}

// ファイルの更新アップロード.マスターキー（ACLあり）
TEST_F(NbFileBucketFT, UploadUpdateFileMasterKeyAclExist) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType);
    NbFileMetadata meta = newfile.GetSuccessData();

    shared_ptr<NbService> master_service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
    NbFileBucket file_bucket_master(master_service, kFileBucketName);

    NbResult<NbFileMetadata> result = file_bucket_master.UploadUpdateFile(kFileName, FTUtil::MakeFilePath(kUploadFile2), "application/json",
                                                                          meta.GetMetaETag() , meta.GetFileETag());
    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
}

// ファイルの更新アップロード.ファイル名が空文字
TEST_F(NbFileBucketFT, UploadUpdateFileFilenameEmpty) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kEmpty, FTUtil::MakeFilePath(kUploadFile2), kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

// ファイルの更新アップロード.ファイルパスが空文字
TEST_F(NbFileBucketFT, UploadUpdateFileFilepathEmpty) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, kEmpty, kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

// ファイルの更新アップロード.ファイルオープンエラー
TEST_F(NbFileBucketFT, UploadUpdateFileOpenError) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, FTUtil::MakeFilePath("invalidFileName"), kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_OPEN_FILE, result.GetResultCode());
}

// ファイルの更新アップロード.バケット名が空文字
TEST_F(NbFileBucketFT, UploadUpdateFileBucketNameEmpty) {
    NbFileBucket file_bucket(service_, kEmpty);
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, FTUtil::MakeFilePath(kUploadFile2), kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

// ファイルのダウンロード(ダウンロード成功)
TEST_F(NbFileBucketFT, DownloadFile) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);

    NbResult<int> result = file_bucket.DownloadFile(kFileName, kDownloadFile);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    int response = result.GetSuccessData();

    EXPECT_EQ(newfile.GetSuccessData().GetLength(), response);

    FTUtil::CompareFiledata(FTUtil::MakeFilePath(kUploadFile), kDownloadFile, response);
}

// ファイルのダウンロード(ファイルサイズ0)
TEST_F(NbFileBucketFT, DownloadFileSize0) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath("0.txt"), kContentType, true);

    NbResult<int> result = file_bucket.DownloadFile(kFileName, kDownloadFile);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    int response = result.GetSuccessData();

    EXPECT_EQ(newfile.GetSuccessData().GetLength(), response);

    std::ifstream file_stream(kDownloadFile, std::ios::ate | std::ios::binary);
    EXPECT_TRUE(file_stream);
    file_stream.close();
    FTUtil::CompareFiledata(FTUtil::MakeFilePath("0.txt"), kDownloadFile, response);
}

// ファイルのダウンロード(ファイル名日本語)
TEST_F(NbFileBucketFT, DownloadFileFilenameJapanese) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile("日本語.txt", FTUtil::MakeFilePath(kUploadFile), kContentType, true);

    NbResult<int> result = file_bucket.DownloadFile("日本語.txt", "日本語.txt");

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    int response = result.GetSuccessData();

    EXPECT_EQ(newfile.GetSuccessData().GetLength(), response);

    FTUtil::CompareFiledata(FTUtil::MakeFilePath(kUploadFile), "日本語.txt", response);

    std::remove("日本語.txt");
}

// ファイルのダウンロード(ファイル存在)
TEST_F(NbFileBucketFT, DownloadFileOverwrite) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);
    NbResult<int> result = file_bucket.DownloadFile(kFileName, kDownloadFile);
    ASSERT_TRUE(result.IsSuccess());
    FTUtil::CompareFiledata(FTUtil::MakeFilePath(kUploadFile), kDownloadFile, result.GetSuccessData());
    newfile = file_bucket.UploadUpdateFile(kFileName, FTUtil::MakeFilePath(kUploadFile2), kEmpty, kEmpty, kEmpty);

    result = file_bucket.DownloadFile(kFileName, kDownloadFile);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    int response = result.GetSuccessData();

    EXPECT_EQ(newfile.GetSuccessData().GetLength(), response);

    FTUtil::CompareFiledata(FTUtil::MakeFilePath(kUploadFile2), kDownloadFile, response);
}

// ファイルのダウンロード(ファイル名不正)
TEST_F(NbFileBucketFT, DownloadFileNotFound) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<int> result = file_bucket.DownloadFile(kFileName, kDownloadFile);

    // 戻り値確認
    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));
}

// ファイルのダウンロード(バケットのcontentACLにread権限がない)
TEST_F(NbFileBucketFT, DownloadFilePermissionBucketContentAclReadNone) {
    string content_acl = R"({"r":[],"w":["g:anonymous"],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("file", kFileBucketName, "", content_acl);

    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);

    NbResult<int> result = file_bucket.DownloadFile(kFileName, kDownloadFile);

    // 戻り値確認
    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(403, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));

    FTUtil::CreateBucket("file", kFileBucketName);
}

// ファイルのダウンロード(ファイルのACLにread権限がない)
TEST_F(NbFileBucketFT, DownloadFilePermissionFileAclReadNone) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    
    NbAcl acl;
    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, acl);

    NbResult<int> result = file_bucket.DownloadFile(kFileName, kDownloadFile);

    // 戻り値確認
    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(403, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));
}

// ファイルのダウンロード.ファイル名が空文字
TEST_F(NbFileBucketFT, DownloadFileFilenameEmpty) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<int> result = file_bucket.DownloadFile(kEmpty, kDownloadFile);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

// ファイルのダウンロード.ファイルパスが空文字
TEST_F(NbFileBucketFT, DownloadFileFilepathEmpty) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<int> result = file_bucket.DownloadFile(kFileName, kEmpty);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

// ファイルのダウンロード.ファイルオープンエラー
TEST_F(NbFileBucketFT, DownloadFileOpenError) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<int> result = file_bucket.DownloadFile(kFileName, "/invalid/dir/file.txt");

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_OPEN_FILE, result.GetResultCode());
}

// ファイルのダウンロード.バケット名が空文字
TEST_F(NbFileBucketFT, DownloadFileBucketNameEmpty) {
    NbFileBucket file_bucket(service_, kEmpty);
    NbResult<int> result = file_bucket.DownloadFile(kFileName, kDownloadFile);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

// ファイル一覧の取得
TEST_F(NbFileBucketFT, GetFiles) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    // 公開なし、削除なし
    NbResult<NbFileMetadata> fileA = file_bucket.UploadNewFile("UploadA", FTUtil::MakeFilePath(kUploadFile), kContentType);
    // 公開なし、削除あり
    NbResult<NbFileMetadata> fileB = file_bucket.UploadNewFile("UploadB", FTUtil::MakeFilePath(kUploadFile), kContentType);
    // 公開あり、削除なし
    NbResult<NbFileMetadata> fileC = file_bucket.UploadNewFile("UploadC", FTUtil::MakeFilePath(kUploadFile), kContentType);
    // 公開あり、削除あり
    NbResult<NbFileMetadata> fileD = file_bucket.UploadNewFile("UploadD", FTUtil::MakeFilePath(kUploadFile), kContentType);
    file_bucket.DeleteFile(fileB.GetSuccessData(), true);
    file_bucket.DeleteFile(fileD.GetSuccessData(), true);
    FTUtil::PublishFile(kFileBucketName, "UploadC");
    FTUtil::PublishFile(kFileBucketName, "UploadD");

    // 公開状態指定のみ、削除マークありを含む
    NbResult<std::vector<NbFileMetadata>> result = file_bucket.GetFiles(true, true);

    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbFileMetadata> response = result.GetSuccessData();

    ASSERT_EQ(2, response.size());
    EXPECT_TRUE(string("UploadC") == response[0].GetFileName() ||
                string("UploadD") == response[0].GetFileName()) << "filename = " << response[0].GetFileName();
    EXPECT_TRUE(string("UploadC") == response[1].GetFileName() ||
                string("UploadD") == response[1].GetFileName()) << "filename = " << response[1].GetFileName();

    // 公開状態指定のみでない、削除マークありを含まない
    result = file_bucket.GetFiles();

    ASSERT_TRUE(result.IsSuccess());
    response = result.GetSuccessData();

    ASSERT_EQ(2, response.size());
    EXPECT_TRUE(string("UploadA") == response[0].GetFileName() ||
                string("UploadC") == response[0].GetFileName()) << "filename = " << response[0].GetFileName();
    EXPECT_TRUE(string("UploadA") == response[1].GetFileName() ||
                string("UploadC") == response[1].GetFileName()) << "filename = " << response[1].GetFileName();
}

// ファイル一覧の取得.ヒット0件
TEST_F(NbFileBucketFT, GetFilesNoData) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<std::vector<NbFileMetadata>> result = file_bucket.GetFiles();

    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbFileMetadata> response = result.GetSuccessData();

    ASSERT_EQ(0, response.size());
}

// ファイル一覧の取得.バケットのcontentACLにread権限がない
TEST_F(NbFileBucketFT, GetFilesPermissionBucketContentAclReadNone) {
    string content_acl = R"({"r":[],"w":["g:anonymous"],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("file", kFileBucketName, "", content_acl);

    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<std::vector<NbFileMetadata>> result = file_bucket.GetFiles(true, true);

    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(403, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));

    FTUtil::CreateBucket("file", kFileBucketName);
}

// ファイル一覧の取得.ファイルのACLにread権限がない
TEST_F(NbFileBucketFT, GetFilesPermissionFileAclReadNone) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    
    NbResult<NbFileMetadata> fileA = file_bucket.UploadNewFile("UploadA", FTUtil::MakeFilePath(kUploadFile), kContentType, NbAcl());
    NbResult<NbFileMetadata> fileB = file_bucket.UploadNewFile("UploadB", FTUtil::MakeFilePath(kUploadFile), kContentType);

    NbResult<std::vector<NbFileMetadata>> result = file_bucket.GetFiles();

    ASSERT_TRUE(result.IsSuccess());
    vector<NbFileMetadata> response = result.GetSuccessData();

    ASSERT_EQ(1, response.size());
    EXPECT_EQ(string("UploadB"), response[0].GetFileName());
}

// ファイル一覧の取得.バケットのcontentACLとファイルのACLにread権限がない(マスターキー使用)
TEST_F(NbFileBucketFT, GetFilesMasterKeyAclNone) {
    string content_acl = R"({"r":[],"w":["g:anonymous"],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("file", kFileBucketName, "", content_acl);

    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> fileA = file_bucket.UploadNewFile("UploadA", FTUtil::MakeFilePath(kUploadFile), kContentType, NbAcl());
    NbResult<NbFileMetadata> fileB = file_bucket.UploadNewFile("UploadB", FTUtil::MakeFilePath(kUploadFile), kContentType);

    shared_ptr<NbService> master_service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
    NbFileBucket file_bucket_master(master_service, kFileBucketName);
    NbResult<std::vector<NbFileMetadata>> result = file_bucket_master.GetFiles();

    ASSERT_TRUE(result.IsSuccess());
    vector<NbFileMetadata> response = result.GetSuccessData();

    ASSERT_EQ(2, response.size());

    FTUtil::CreateBucket("file", kFileBucketName);
}

// ファイル一覧の取得.バケットのcontentACLとファイルのACLにread権限がある(マスターキー使用)
TEST_F(NbFileBucketFT, GetFilesMasterKeyAclExists) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> fileA = file_bucket.UploadNewFile("UploadA", FTUtil::MakeFilePath(kUploadFile), kContentType);
    NbResult<NbFileMetadata> fileB = file_bucket.UploadNewFile("UploadB", FTUtil::MakeFilePath(kUploadFile), kContentType);

    shared_ptr<NbService> master_service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
    NbFileBucket file_bucket_master(master_service, kFileBucketName);
    NbResult<std::vector<NbFileMetadata>> result = file_bucket_master.GetFiles();

    ASSERT_TRUE(result.IsSuccess());
    vector<NbFileMetadata> response = result.GetSuccessData();

    ASSERT_EQ(2, response.size());
}

// ファイル一覧の取得.バケット名が空文字
TEST_F(NbFileBucketFT, GetFilesBucketNameEmpty) {
    NbFileBucket file_bucket(service_, kEmpty);
    NbResult<std::vector<NbFileMetadata>> result = file_bucket.GetFiles();

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

// ファイルの削除
// 削除マーク指定なし
TEST_F(NbFileBucketFT, DeleteFileDeleteMarkNone) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);
    NbFileMetadata meta = newfile.GetSuccessData();
    NbResult<NbFileMetadata> result = file_bucket.DeleteFile(meta.GetFileName(), meta.GetMetaETag(), meta.GetFileETag());

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();
    EXPECT_TRUE(response.GetFileName().empty());

    EXPECT_TRUE(file_bucket.GetFiles(false ,true).GetSuccessData().empty());
}

// ファイルの削除
// 削除マーク指定あり
TEST_F(NbFileBucketFT, DeleteFileDeleteMark) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);

    NbResult<NbFileMetadata> result = file_bucket.DeleteFile(newfile.GetSuccessData(), true);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(kFileName, response.GetFileName());
    EXPECT_TRUE(response.IsDeleted());
}

// ファイルの削除.オプションパラメータ未設定
TEST_F(NbFileBucketFT, DeleteFileNoOptionParams) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);

    NbResult<NbFileMetadata> result = file_bucket.DeleteFile(kFileName, kEmpty, kEmpty);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();
    EXPECT_TRUE(response.GetFileName().empty());

    EXPECT_TRUE(file_bucket.GetFiles(false ,true).GetSuccessData().empty());
}

// ファイルの削除.ファイルロック中
TEST_F(NbFileBucketFT, DeleteFileLock) {
    FTUtil::CreateTestFile("testfile.txt", 10000000);
    NbFileBucket file_bucket(service_, kFileBucketName);

    std::thread upload_thread( [&file_bucket]() {
            file_bucket.SetTimeout(600);
            NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, "testfile.txt", kContentType, true);
        });

    struct timespec time{0, 10000000}; //10ms WAIT
    nanosleep(&time, nullptr);
    NbResult<NbFileMetadata> result = file_bucket.DeleteFile(kFileName, kEmpty, kEmpty);
    upload_thread.join();
    std::remove("testfile.txt");

    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(409, result.GetRestError().status_code);
    NbJsonObject response(result.GetRestError().reason);
    EXPECT_EQ(string("file_locked"), response.GetString("reasonCode"));
    EXPECT_EQ(string("File is locked"), response.GetString("detail"));
}

// ファイルの削除.FileEtag不一致
TEST_F(NbFileBucketFT, DeleteFileEtagMismatch) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);
    NbFileMetadata meta = newfile.GetSuccessData();
    NbResult<NbFileMetadata> result = file_bucket.DeleteFile(meta.GetFileName(), meta.GetMetaETag(), "etag");

    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(409, result.GetRestError().status_code);
    NbJsonObject response(result.GetRestError().reason);
    EXPECT_EQ(string("etag_mismatch"), response.GetString("reasonCode"));
}

// ファイルの削除(バケットのcontentACLにread権限がない)
TEST_F(NbFileBucketFT, DeleteFilePermissionBucketContentAclReadNone) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);

    string content_acl = R"({"r":["g:anonymous"],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("file", kFileBucketName, "", content_acl);

    NbFileMetadata meta = newfile.GetSuccessData();
    NbResult<NbFileMetadata> result = file_bucket.DeleteFile(meta.GetFileName(), meta.GetMetaETag(), meta.GetFileETag());

    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(403, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));

    FTUtil::CreateBucket("file", kFileBucketName);
}

// ファイルの削除(ファイルのcontentACLにread権限がない)
TEST_F(NbFileBucketFT, DeleteFilePermissionFileAclReadNone) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, NbAcl(), true);
    NbFileMetadata meta = newfile.GetSuccessData();
    NbResult<NbFileMetadata> result = file_bucket.DeleteFile(meta.GetFileName(), meta.GetMetaETag(), meta.GetFileETag());

    ASSERT_TRUE(result.IsRestError());
    EXPECT_EQ(403, result.GetRestError().status_code);
    EXPECT_TRUE(NbJsonObject(result.GetRestError().reason).IsMember("error"));
}

// ファイルの削除(バケットのcontentACLにread権限がない:マスターキー)
TEST_F(NbFileBucketFT, DeleteFileMasterKeyAclNone) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, NbAcl(), true);

    string content_acl = R"({"r":["g:anonymous"],"w":[],"c":[],"u":[],"d":[]})";
    FTUtil::CreateBucket("file", kFileBucketName, "", content_acl);

    NbFileMetadata meta = newfile.GetSuccessData();
    shared_ptr<NbService> master_service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
    NbFileBucket file_bucket_master(master_service, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket_master.DeleteFile(meta.GetFileName(), meta.GetMetaETag(), meta.GetFileETag());

    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();
    EXPECT_TRUE(response.GetFileName().empty());

    EXPECT_TRUE(file_bucket.GetFiles(false ,true).GetSuccessData().empty());

    FTUtil::CreateBucket("file", kFileBucketName);
}

// ファイルの削除(バケットのcontentACLにread権限がある:マスターキー)
TEST_F(NbFileBucketFT, DeleteFileMasterKeyAclExists) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);

    NbFileMetadata meta = newfile.GetSuccessData();
    shared_ptr<NbService> master_service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);
    NbFileBucket file_bucket_master(master_service, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket_master.DeleteFile(meta.GetFileName(), meta.GetMetaETag(), meta.GetFileETag());

    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();
    EXPECT_TRUE(response.GetFileName().empty());

    EXPECT_TRUE(file_bucket.GetFiles(false ,true).GetSuccessData().empty());
}

// ファイルの削除.ファイル名が空文字
TEST_F(NbFileBucketFT, DeleteFileFilenameEmpty) {
    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> result = file_bucket.DeleteFile(kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

// ファイルの削除.バケット名が空文字
TEST_F(NbFileBucketFT, DeleteFileBucketNameEmpty) {
    NbFileBucket file_bucket(service_, kEmpty);
    NbResult<NbFileMetadata> result = file_bucket.DeleteFile(kFileName, kEmpty, kEmpty);

    // 戻り値確認
    ASSERT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

// ファイルストレージ.マルチテナント
TEST_F(NbFileBucketFT, FileStorageMultiTenant) {
    shared_ptr<NbService> service_b = NbService::CreateService(kEndPointUrl, kTenantIdB, kAppIdB, kAppKeyB, kProxyB);
    NbFileBucket file_bucket_a(service_, kFileBucketName);
    NbFileBucket file_bucket_b(service_b, kFileBucketName);

    // ファイル新規アップロード(A)
    NbResult<NbFileMetadata> upload_a = file_bucket_a.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType);
    ASSERT_TRUE(upload_a.IsSuccess());

    NbResult<vector<NbFileMetadata>> getfiles_a = file_bucket_a.GetFiles();
    ASSERT_TRUE(getfiles_a.IsSuccess());
    vector<NbFileMetadata> files_a = getfiles_a.GetSuccessData();
    EXPECT_EQ(1, files_a.size());
    EXPECT_EQ(kFileName, files_a[0].GetFileName());

    NbResult<vector<NbFileMetadata>> getfiles_b = file_bucket_b.GetFiles();
    ASSERT_TRUE(getfiles_b.IsSuccess());
    vector<NbFileMetadata> files_b = getfiles_b.GetSuccessData();
    EXPECT_EQ(0, files_b.size());

    // ファイル新規アップロード(B)
    NbResult<NbFileMetadata> upload_b = file_bucket_b.UploadNewFile("file_name_b.txt", FTUtil::MakeFilePath(kUploadFile2), kContentType);
    ASSERT_TRUE(upload_b.IsSuccess());

    getfiles_a = file_bucket_a.GetFiles();
    ASSERT_TRUE(getfiles_a.IsSuccess());
    files_a = getfiles_a.GetSuccessData();
    EXPECT_EQ(1, files_a.size());
    EXPECT_EQ(kFileName, files_a[0].GetFileName());

    getfiles_b = file_bucket_b.GetFiles();
    ASSERT_TRUE(getfiles_b.IsSuccess());
    files_b = getfiles_b.GetSuccessData();
    EXPECT_EQ(1, files_b.size());
    EXPECT_EQ("file_name_b.txt", files_b[0].GetFileName());

    // ファイルダウンロード(A)
    NbResult<int> download_a = file_bucket_a.DownloadFile(kFileName, kDownloadFile);
    ASSERT_TRUE(download_a.IsSuccess());
    FTUtil::CompareFiledata(FTUtil::MakeFilePath(kUploadFile), kDownloadFile, download_a.GetSuccessData());

    // ファイルダウンロード(B)
    NbResult<int> download_b = file_bucket_b.DownloadFile("file_name_b.txt", kDownloadFile);
    ASSERT_TRUE(download_b.IsSuccess());
    FTUtil::CompareFiledata(FTUtil::MakeFilePath(kUploadFile2), kDownloadFile, download_b.GetSuccessData());

    // ファイル削除(A)
    NbResult<NbFileMetadata> delete_a = file_bucket_a.DeleteFile(kFileName, kEmpty, kEmpty);
    ASSERT_TRUE(delete_a.IsSuccess());

    getfiles_a = file_bucket_a.GetFiles();
    ASSERT_TRUE(getfiles_a.IsSuccess());
    files_a = getfiles_a.GetSuccessData();
    EXPECT_EQ(0, files_a.size());

    getfiles_b = file_bucket_b.GetFiles();
    ASSERT_TRUE(getfiles_b.IsSuccess());
    files_b = getfiles_b.GetSuccessData();
    EXPECT_EQ(1, files_b.size());
    EXPECT_EQ("file_name_b.txt", files_b[0].GetFileName());

    // ファイル削除(B)
    NbResult<NbFileMetadata> delete_b = file_bucket_b.DeleteFile("file_name_b.txt", kEmpty, kEmpty);
    ASSERT_TRUE(delete_b.IsSuccess());

    getfiles_a = file_bucket_a.GetFiles();
    ASSERT_TRUE(getfiles_a.IsSuccess());
    files_a = getfiles_a.GetSuccessData();
    EXPECT_EQ(0, files_a.size());

    getfiles_b = file_bucket_b.GetFiles();
    ASSERT_TRUE(getfiles_b.IsSuccess());
    files_b = getfiles_b.GetSuccessData();
    EXPECT_EQ(0, files_b.size());
}

// ファイルストレージ.繰返し
TEST_F(NbFileBucketFT, FileStorageRepeat) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    for (int i = 0; i < 3; i++) {
        // 新規アップロード
        NbResult<NbFileMetadata> upload = file_bucket.UploadNewFile(std::to_string(i) + kFileName,
                                                                    FTUtil::MakeFilePath(kUploadFile), kContentType);
        ASSERT_TRUE(upload.IsSuccess());

        // メタデータ一覧
        NbResult<vector<NbFileMetadata>> getfiles = file_bucket.GetFiles();
        ASSERT_TRUE(getfiles.IsSuccess());
        vector<NbFileMetadata> files = getfiles.GetSuccessData();
        EXPECT_EQ(i + 1, files.size());

        // ダウンロード
        NbResult<int> download = file_bucket.DownloadFile(std::to_string(i) + kFileName, kDownloadFile);
        ASSERT_TRUE(download.IsSuccess());
        FTUtil::CompareFiledata(FTUtil::MakeFilePath(kUploadFile), kDownloadFile, download.GetSuccessData());
    }

    for (int i = 0; i < 3; i++) {
        // 更新アップロード
        NbResult<NbFileMetadata> upload = file_bucket.UploadUpdateFile(std::to_string(i) + kFileName, FTUtil::MakeFilePath(kUploadFile2),
                                                                       kEmpty, kEmpty, kEmpty);
        ASSERT_TRUE(upload.IsSuccess());

        // メタデータ一覧
        NbResult<vector<NbFileMetadata>> getfiles = file_bucket.GetFiles();
        ASSERT_TRUE(getfiles.IsSuccess());
        vector<NbFileMetadata> files = getfiles.GetSuccessData();
        EXPECT_EQ(3 - i, files.size());

        // ファイル削除
        NbResult<NbFileMetadata> del = file_bucket.DeleteFile(std::to_string(i) + kFileName, kEmpty, kEmpty);
        ASSERT_TRUE(del.IsSuccess());
    }
}

// ファイルストレージ.性能.最大ファイル数
TEST_F(NbFileBucketFT, PerformanceFileNumSlowTest) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    for (int i = 0; i < 1000; i++) {
        // 新規アップロード
        NbResult<NbFileMetadata> upload = file_bucket.UploadNewFile("filename_" + std::to_string(i + 1) + ".txt",
                                                                    FTUtil::MakeFilePath(kUploadFile), kContentType);
        ASSERT_TRUE(upload.IsSuccess());
    }

    for (int i = 0; i < 1000; i++) {
        string download_file = "filename_" + std::to_string(i + 1) + ".txt";
        // ダウンロード
        NbResult<int> download = file_bucket.DownloadFile(download_file, download_file);
        ASSERT_TRUE(download.IsSuccess());
        FTUtil::CompareFiledata(FTUtil::MakeFilePath(kUploadFile), download_file, download.GetSuccessData());
    }

    // メタデータ一覧
    NbResult<vector<NbFileMetadata>> getfiles = file_bucket.GetFiles();
    ASSERT_TRUE(getfiles.IsSuccess());
    vector<NbFileMetadata> files = getfiles.GetSuccessData();
    EXPECT_EQ(1000, files.size());

    // ファイル名のリストを作成して比較する
    vector<string> expect_filename;
    vector<string> get_filename;
    for (int i = 0; i < 1000; i++) {
        expect_filename.push_back("filename_" + std::to_string(i + 1) + ".txt");
        get_filename.push_back(files[i].GetFileName());
    }
    FTUtil::CompareList(expect_filename, get_filename);

    for (int i = 0; i < 1000; i++) {
        string remove_file = "filename_" + std::to_string(i + 1) + ".txt";
        std::remove(remove_file.c_str());
    }
}

// ファイルストレージ.性能.最大ファイルサイズ
TEST_F(NbFileBucketFT, PerformanceFileSizeSlowTest) {
    int file_size = 1024*1024*100;
    FTUtil::CreateTestFile("testfile.txt", file_size);
    NbFileBucket file_bucket(service_, kFileBucketName);
    file_bucket.SetTimeout(1800);

    NbResult<NbFileMetadata> upload = file_bucket.UploadNewFile(kFileName, "testfile.txt", kContentType);
    ASSERT_TRUE(upload.IsSuccess());
    EXPECT_EQ(file_size, upload.GetSuccessData().GetLength());

    NbResult<int> download = file_bucket.DownloadFile(kFileName, "testfile.txt");
    ASSERT_TRUE(download.IsSuccess());
    EXPECT_EQ(file_size, download.GetSuccessData());
    EXPECT_EQ(file_size, NbUtility::GetFileSize("testfile.txt"));

    std::remove("testfile.txt");
}

// ファイルストレージ.性能.最大サイズファイルの保持
TEST_F(NbFileBucketFT, PerformanceMaxSizeNumSlowTest) {
    int file_size = 1024*1024*100;
    FTUtil::CreateTestFile("testfile.txt", file_size);
    NbFileBucket file_bucket(service_, kFileBucketName);
    file_bucket.SetTimeout(1800);

    for (int i = 0; i < 8; i++) {
        // 新規アップロード
        NbResult<NbFileMetadata> upload = file_bucket.UploadNewFile("filename_" + std::to_string(i + 1) + ".txt",
                                                                    "testfile.txt", kContentType);
        ASSERT_TRUE(upload.IsSuccess());
    }

    // メタデータ一覧
    NbResult<vector<NbFileMetadata>> getfiles = file_bucket.GetFiles();
    ASSERT_TRUE(getfiles.IsSuccess());
    vector<NbFileMetadata> files = getfiles.GetSuccessData();
    EXPECT_EQ(8, files.size());

    std::remove("testfile.txt");
}

// ファイルストレージ.性能.最大合計サイズ
TEST_F(NbFileBucketFT, PerformanceMaxTotalSizeSlowTest) {
    int file_size = 1024*1024*10;
    FTUtil::CreateTestFile("testfile.txt", file_size);
    NbFileBucket file_bucket(service_, kFileBucketName);
    file_bucket.SetTimeout(1800);

    for (int i = 0; i < 100; i++) {
        // 新規アップロード
        NbResult<NbFileMetadata> upload = file_bucket.UploadNewFile("filename_" + std::to_string(i + 1) + ".txt",
                                                                    "testfile.txt", kContentType);
        ASSERT_TRUE(upload.IsSuccess());
    }

    // メタデータ一覧
    NbResult<vector<NbFileMetadata>> getfiles = file_bucket.GetFiles();
    ASSERT_TRUE(getfiles.IsSuccess());
    vector<NbFileMetadata> files = getfiles.GetSuccessData();
    EXPECT_EQ(100, files.size());
    int total = 0;
    for (auto meta : files) {
        total += meta.GetLength();
    }
    EXPECT_EQ(file_size * 100, total);

    std::remove("testfile.txt");
}

// ファイルストレージ.性能.アップロード・ダウンロード時間
TEST_F(NbFileBucketFT, PerformanceTransferTime) {
    int file_size = 1024*1024*1;
    FTUtil::CreateTestFile("testfile.txt", file_size);
    NbFileBucket file_bucket(service_, kFileBucketName);

    // 接続確立用
    NbResult<vector<NbFileMetadata>> getfiles = file_bucket.GetFiles();

    // 新規アップロード
    auto start = std::chrono::system_clock::now();
    NbResult<NbFileMetadata> upload = file_bucket.UploadNewFile(kFileName, "testfile.txt", kContentType);
    auto end = std::chrono::system_clock::now();
    ASSERT_TRUE(upload.IsSuccess());
    EXPECT_GE(900, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    // ダウンロード
    start = std::chrono::system_clock::now();
    NbResult<int> download = file_bucket.DownloadFile(kFileName, kDownloadFile);
    end = std::chrono::system_clock::now();
    ASSERT_TRUE(download.IsSuccess());    
    EXPECT_GE(1100, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    std::remove("testfile.txt");
}
} //namespace necbaas
