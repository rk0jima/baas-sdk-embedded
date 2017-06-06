#ifndef NECBAAS_FTUTIL_H
#define NECBAAS_FTUTIL_H

#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <ctime>
#include "necbaas/nb_json_object.h"
#include "necbaas/nb_object.h"
#include "necbaas/nb_acl.h"

// テストユーティリティヘッダ

namespace necbaas {
namespace FTUtil {
// バケット初期化
void InitDefaultBucket();

// デフォルトユーザの作成
void CreateDefaultUser();
void CreateDefaultUser(std::shared_ptr<NbService> service);

// バケットを作成・更新する
void CreateBucket(const std::string &bucket_type, const std::string &bucket_name,
                  std::string acl = "", std::string content_acl = "");
void CreateBucket(std::shared_ptr<NbService> service, const std::string &bucket_type, const std::string &bucket_name,
                  std::string acl = "", std::string content_acl = "");

// 全オブジェクト削除
void DeleteAllObject();
void DeleteAllObject(std::shared_ptr<NbService> service);

// テストバケットの全ファイルを削除する
void DeleteAllFile();
void DeleteAllFile(std::shared_ptr<NbService> service);

// ファイル公開
void PublishFile(const std::string &bucket_name, const std::string &file_name);

// ファイルパスの作成
std::string MakeFilePath(const std::string &file_name);

// テスト用ファイルの作成
void CreateTestFile(const std::string &file_path, int file_size);

// key,valに一致するヘッダを検索する（keyの大文字小文字は問わない）
bool SearchHeader(const std::multimap<std::string, std::string> &map,
                  const std::string &key, const std::string &value);

// ACLの比較
void CompareAcl(const NbAcl &acl1, const NbAcl &acl2);

// NbObjectの比較
void CompareObject(const NbObject &obj1, const NbObject &obj2);

// ファイルの中身の比較
void CompareFiledata(const std::string &file1, const std::string &file2, int size);

// vectorの比較(順不同)
template <typename T>
void CompareList(std::vector<T> container1, std::vector<T> container2) {
    ASSERT_EQ(container1.size(), container2.size());
    std::list<T> list1, list2;
    for (int i = 0; i < container1.size(); ++i) {
        list1.push_back(container1[i]);
        list2.push_back(container2[i]);
    }
    list1.sort();
    list2.sort();
    EXPECT_EQ(list1, list2);    
}

// listの比較(順不同)
template <typename T>
void CompareList(std::list<T> container1, std::list<T> container2) {
    ASSERT_EQ(container1.size(), container2.size());
    container1.sort();
    container2.sort();
    EXPECT_EQ(container1, container2);
}

// std::tmの比較
void CompareTime(const std::tm &tm1, const std::tm &tm2, bool equal = true);

// std::tm型データのインクリメント
std::tm IncrementTime(std::tm tm);

// NbUserの比較
void CompareUser(const NbUser &user1, const NbUser &user2);

} //namespace FTUtil
} //namespace necbaas
#endif //NECBAAS_FTUTIL_H
