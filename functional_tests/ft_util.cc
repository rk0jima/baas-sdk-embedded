#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_service.h"
#include "necbaas/nb_file_bucket.h"

// テストユーティリティ&メイン関数

namespace necbaas {
namespace FTUtil {

using std::string;
using std::vector;
using std::shared_ptr;

static shared_ptr<NbService> master_service;

// テストバケットの全オブジェクトを削除する
void DeleteAllObject() {
    NbResult<NbHttpResponse> rest_result = master_service->ExecuteRequest(
        [](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Delete("/objects")
                          .AppendPath("/" + kObjectBucketName)
                          .Build();
        }, 60);
    EXPECT_TRUE(rest_result.IsSuccess());
}

// テストバケットの全ファイルを削除する
void DeleteAllFile() {
    NbFileBucket file_bucket(master_service, kFileBucketName);
    NbResult<vector<NbFileMetadata>> file_lists = file_bucket.GetFiles(false, true);
    ASSERT_TRUE(file_lists.IsSuccess());
    for (auto meta : file_lists.GetSuccessData()) {
        ASSERT_TRUE(file_bucket.DeleteFile(meta.GetFileName(), kEmpty, kEmpty).IsSuccess());
    }
}

// ファイル公開
void PublishFile(const string &bucket_name, const string &file_name) {
    NbResult<NbHttpResponse> rest_result = master_service->ExecuteRequest(
        [&](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Put("/files")
                          .AppendPath("/" + bucket_name + "/" + file_name + "/publish")
                          .Build();
        }, 60);
    EXPECT_TRUE(rest_result.IsSuccess());
}

// ファイルパスの作成
string MakeFilePath(const string &file_name) {
    // ビルド環境のfunctinal_test/filesをデフォルトとする
    // kFileDir が設定されていれば kFileDir + "files/" を使用
    string file_path;

    if (kFileDir.empty()) {
        file_path = __FILE__;
        file_path.erase(file_path.rfind("/") + 1);
        file_path += "files/" + file_name;
    } else {
        file_path = kFileDir + "files/" + file_name;
    }

    return file_path;
}

// ACLの比較
void CompareAcl(const NbAcl &acl1, const NbAcl &acl2) {
    EXPECT_EQ(acl1.GetCreate(), acl2.GetCreate());
    EXPECT_EQ(acl1.GetDelete(), acl2.GetDelete());
    EXPECT_EQ(acl1.GetRead(), acl2.GetRead());
    EXPECT_EQ(acl1.GetUpdate(), acl2.GetUpdate());
    EXPECT_EQ(acl1.GetWrite(), acl2.GetWrite());
    EXPECT_EQ(acl1.GetAdmin(), acl2.GetAdmin());
    EXPECT_EQ(acl1.GetOwner(), acl2.GetOwner());
}

// NbObjectの比較
void CompareObject(const NbObject &obj1, const NbObject &obj2) {
    EXPECT_EQ(obj1.GetBucketName(), obj2.GetBucketName());
    EXPECT_EQ(obj1.GetObjectId(), obj2.GetObjectId());
    CompareTime(obj1.GetCreatedTime(), obj2.GetCreatedTime());
    CompareTime(obj1.GetUpdatedTime(), obj2.GetUpdatedTime());
    CompareAcl(obj1.GetAcl(), obj2.GetAcl());
    EXPECT_EQ(obj1.IsDeleteMark(), obj2.IsDeleteMark());
    EXPECT_EQ(obj1, obj2);
}

// ファイルの中身の比較
void CompareFiledata(const string &file1, const string &file2, int size) {
    std::unique_ptr<char> buffer(new char[size]);
    std::ifstream file_stream;
    file_stream.open(file1, std::ios::in | std::ios::binary);
    file_stream.read(buffer.get(), size);
    string file1_data(buffer.get(), size);

    file_stream.open(file2, std::ios::in | std::ios::binary);
    file_stream.read(buffer.get(), size);
    string file2_data(buffer.get(), size);

    EXPECT_EQ(file1_data, file2_data);
}

// std::tmの比較
void CompareTime(const std::tm &tm1, const std::tm &tm2, bool equal) {
    if (equal) {
        EXPECT_EQ(tm1.tm_sec, tm2.tm_sec);
        EXPECT_EQ(tm1.tm_min, tm2.tm_min);
        EXPECT_EQ(tm1.tm_hour, tm2.tm_hour);
        EXPECT_EQ(tm1.tm_mday, tm2.tm_mday);
        EXPECT_EQ(tm1.tm_mon, tm2.tm_mon);
        EXPECT_EQ(tm1.tm_year, tm2.tm_year);
        EXPECT_EQ(tm1.tm_wday, tm2.tm_wday);
        EXPECT_EQ(tm1.tm_yday, tm2.tm_yday);
    } else {
        EXPECT_FALSE(tm1.tm_sec == tm2.tm_sec &&
                     tm1.tm_min == tm2.tm_min &&
                     tm1.tm_hour == tm2.tm_hour &&
                     tm1.tm_mday == tm2.tm_mday &&
                     tm1.tm_mon == tm2.tm_mon &&
                     tm1.tm_year == tm2.tm_year &&
                     tm1.tm_wday == tm2.tm_wday &&
                     tm1.tm_yday == tm2.tm_yday);
    }
}

// std::tm型データのインクリメント
std::tm IncrementTime(std::tm tm) {
    std::time_t time = std::mktime(&tm);
    ++time;
    std::tm *new_time = std::gmtime(&time);
    return *new_time;
}

} //namespace FTUtil
} //namespace necbaas

// メイン
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    using namespace necbaas;
    FTUtil::master_service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kMasterKey, kProxy);

    return RUN_ALL_TESTS();
}

