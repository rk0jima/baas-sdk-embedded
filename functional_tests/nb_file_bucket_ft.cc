#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_file_bucket.h"
#include "necbaas/nb_user.h"
#include "necbaas/nb_acl.h"

namespace necbaas {

using std::string;
using std::vector;
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

// ファイルの新規アップロード.ログイン済み
// ACL指定なし、キャッシュ禁止
TEST_F(NbFileBucketFT, UploadNewFile) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();
    expected_acl.SetOwner(login.GetSuccessData().GetUserId());

    NbFileBucket file_bucket(service_, kFileBucketName); 

    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, expected_acl, true);
 
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

// ファイルの更新アップロード.ログイン済み
// 情報個別指定
TEST_F(NbFileBucketFT, UploadUpdateFile) {
    NbResult<NbUser> login = NbUser::LoginWithUsername(service_, kUserName, kPassword);
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

// ファイル一覧の取得
// 公開状態指定あり、削除マークあり
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

    NbResult<std::vector<NbFileMetadata>> result = file_bucket.GetFiles(true, true);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    std::vector<NbFileMetadata> response = result.GetSuccessData();

    ASSERT_EQ(2, response.size());
    EXPECT_TRUE(string("UploadC") == response[0].GetFileName() ||
                string("UploadD") == response[0].GetFileName()) << "filename = " << response[0].GetFileName();
    EXPECT_TRUE(string("UploadC") == response[1].GetFileName() ||
                string("UploadD") == response[1].GetFileName()) << "filename = " << response[1].GetFileName();
}

// ファイルの削除
// 削除マーク指定あり
TEST_F(NbFileBucketFT, DeleteFile) {
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile), kContentType, true);

    NbResult<NbFileMetadata> result = file_bucket.DeleteFile(newfile.GetSuccessData(), true);

    // 戻り値確認
    ASSERT_TRUE(result.IsSuccess());
    NbFileMetadata response = result.GetSuccessData();

    EXPECT_EQ(kFileName, response.GetFileName());
    EXPECT_TRUE(response.IsDeleted());
}
} //namespace necbaas
