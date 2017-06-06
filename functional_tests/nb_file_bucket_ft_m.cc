#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_file_bucket.h"
#include "necbaas/nb_user.h"
#include "necbaas/nb_acl.h"

namespace necbaas {

using std::string;
using std::vector;
using std::list;
using std::shared_ptr;

class NbFileBucketManual : public ::testing::Test {
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

shared_ptr<NbService> NbFileBucketManual::service_;

// ファイルCRUD
TEST_F(NbFileBucketManual, FileStorageCRUD) {
    NbAcl expected_acl = NbAcl::CreateAclForAnonymous();
    EXPECT_FALSE(NbUser::IsLoggedIn(service_));

    NbFileBucket file_bucket(service_, kFileBucketName);
    NbResult<NbFileMetadata> upload = file_bucket.UploadNewFile(kFileName, FTUtil::MakeFilePath(kUploadFile),
                                                                kContentType, expected_acl, true);
    ASSERT_TRUE(upload.IsSuccess());

    NbResult<NbFileMetadata> update = file_bucket.UploadUpdateFile(FTUtil::MakeFilePath(kUploadFile2), upload.GetSuccessData());
    ASSERT_TRUE(update.IsSuccess());

    NbResult<int> download = file_bucket.DownloadFile(kFileName, kDownloadFile);
    ASSERT_TRUE(download.IsSuccess());

    NbResult<std::vector<NbFileMetadata>> getfiles = file_bucket.GetFiles(true, true);
    ASSERT_TRUE(getfiles.IsSuccess());

    NbResult<NbFileMetadata> del = file_bucket.DeleteFile(update.GetSuccessData(), true);
    ASSERT_TRUE(del.IsSuccess());
}


// ファイル新規作成・ファイルサイズオーバー
TEST_F(NbFileBucketManual, FileSizeError) {
    FTUtil::CreateTestFile("testfile.txt", 1024*1024*100+1);
    NbFileBucket file_bucket(service_, kFileBucketName);

    NbResult<NbFileMetadata> newfile = file_bucket.UploadNewFile(kFileName, "testfile.txt", kContentType, true);
    std::remove("testfile.txt");

    ASSERT_TRUE(newfile.IsRestError());
    EXPECT_EQ(400, newfile.GetRestError().status_code);
}
} //namespace necbaas
