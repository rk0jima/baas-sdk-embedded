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

#include "necbaas/internal/nb_utility.h"
#include <fstream>
#include "necbaas/internal/nb_logger.h"

namespace necbaas {
namespace NbUtility {

using std::string;

std::tm DateStringToTm(const string &time) {
    std::tm out_tm;
    string time_string = time;  // strptimeで使用するためコピーする

    char *ret = strptime(time_string.c_str(), "%Y-%m-%dT%T", &out_tm);

    // "YYYY-mm-ddTHH:MM:SS"形式で時刻情報が変換できればOKとする
    if (!ret || (ret - time_string.c_str() != 19)) {
        //失敗した場合は初期化
        NBLOG(ERROR) << "strptime fail.";
        std::memset(&out_tm, 0, sizeof(out_tm));
    }

    return out_tm;
}

string TmToDateString(const std::tm &time) {
    string time_string{};
    char time_char[25];

    if (time.tm_year == 0) {
        return time_string;
    }

    strftime(time_char, 25, "%Y-%m-%dT%T.000Z", &time);
    time_string = time_char;
    return time_string;
}

int GetFileSize(const std::string &file_name) {
    std::ifstream file_stream(file_name, std::ios::ate | std::ios::binary);
    if (!file_stream) {
        // ファイルオープンエラー
        NBLOG(ERROR) << "File open error.";
        return 0;
    }
    file_stream.seekg(0, std::fstream::end);
    auto eof_pos = file_stream.tellg();
    file_stream.seekg(0, std::fstream::beg);
    auto beg_pos = file_stream.tellg();
    return (int)(eof_pos - beg_pos);
}

NbJsonType ConvertJsonType(Json::ValueType json_type) {
    switch (json_type) {
        case Json::intValue:
        case Json::uintValue:
        case Json::realValue:
            return NbJsonType::NB_JSON_NUMBER;
        case Json::stringValue:
            return NbJsonType::NB_JSON_STRING;
        case Json::booleanValue:
            return NbJsonType::NB_JSON_BOOLEAN;
        case Json::arrayValue:
            return NbJsonType::NB_JSON_ARRAY;
        case Json::objectValue:
            return NbJsonType::NB_JSON_OBJECT;
        case Json::nullValue:
        default:
            return NbJsonType::NB_JSON_NULL;
    }
}
}  // namespace NbUtility
}  // namespace necbaas
