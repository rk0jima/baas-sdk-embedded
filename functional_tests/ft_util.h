#ifndef NECBAAS_FTUTIL_H
#define NECBAAS_FTUTIL_H

#include <string>
#include <vector>
#include <ctime>
#include "necbaas/nb_json_object.h"
#include "necbaas/nb_object.h"
#include "necbaas/nb_acl.h"

// テストユーティリティヘッダ

namespace necbaas {
namespace FTUtil {
// 全オブジェクト削除
void DeleteAllObject();

// テストバケットの全ファイルを削除する
void DeleteAllFile();

// ファイル公開
void PublishFile(const std::string &bucket_name, const std::string &file_name);

// ファイルパスの作成
std::string MakeFilePath(const std::string &file_name);

// ACLの比較
void CompareAcl(const NbAcl &acl1, const NbAcl &acl2);

// NbObjectの比較
void CompareObject(const NbObject &obj1, const NbObject &obj2);

// ファイルの中身の比較
void CompareFiledata(const std::string &file1, const std::string &file2, int size);

// std::tmの比較
void CompareTime(const std::tm &tm1, const std::tm &tm2, bool equal = true);

// std::tm型データのインクリメント
std::tm IncrementTime(std::tm tm);

} //namespace FTUtil
} //namespace necbaas
#endif //NECBAAS_FTUTIL_H
