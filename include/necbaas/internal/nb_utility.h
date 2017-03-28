/*
 * COPYRIGHT (C) 2017 NEC CORPORATION
 *
 * ALL RIGHTS RESERVED BY NEC CORPORATION,  THIS PROGRAM
 * MUST BE USED SOLELY FOR THE PURPOSE FOR WHICH IT WAS
 * FURNISHED BY NEC CORPORATION,  NO PART OF THIS PROGRAM
 * MAY BE REPRODUCED OR DISCLOSED TO OTHERS,  IN ANY FORM
 * WITHOUT THE PRIOR WRITTEN PERMISSION OF NEC CORPORATION.
 *
 * NEC CONFIDENTIAL AND PROPRIETARY
 */

#ifndef NECBAAS_NBUTILITY_H
#define NECBAAS_NBUTILITY_H

#include <string>
#include <ctime>
#include <json/json.h>
#include "necbaas/nb_json_type.h"

namespace necbaas {
namespace NbUtility {

// ユーティリティ関数群

/**
 * 日時データの文字列からstd::tm型への変換.
 * @param[in]   time_string       日時文字列
 * @return      変換結果
 */
extern std::tm DateStringToTm(const std::string &time_string);

/**
 * 日時データのstd::tm型から文字列への変換.
 * @param[in]   time              日時文字列
 * @return      変換結果
 */
extern std::string TmToDateString(const std::tm &time);

/**
 * ファイルサイズ取得.
 * @param[in]   file_name       ファイル名
 * @return      ファイルサイズ
 */
extern int GetFileSize(const std::string &file_name);

/**
 * Jsonタイプ変換.
 * @param[in]   json_type       Json::ValueType
 * @return      NbJsonType
 */
extern NbJsonType ConvertJsonType(Json::ValueType json_type);

/**
 * 文字列比較(大文字小文字区別なし).
 * @param[in]   str1       文字列１
 * @param[in]   str2       文字列２
 * @return      比較結果  
 */
extern bool CompareCaseInsensitiveString(std::string str1, std::string str2);
} //namespace NbUtility
} //namespace necbaas

#endif //NECBAAS_NBUTILITY_H
