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

#include "necbaas/internal/nb_http_handler.h"
#include <thread>
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::vector;

static const string kStatusCodeLineMarker = "HTTP/1.1 ";
static const auto kSizeStatusLineMarker = kStatusCodeLineMarker.size();
static const int kStatusCodeSize = 3;

NbHttpHandler::NbHttpHandler() {}
NbHttpHandler::~NbHttpHandler() {}

size_t NbHttpHandler::ReadCallback(void *buffer, size_t size, size_t nmemb) {
    //無処理
    return 0;
}

size_t NbHttpHandler::WriteHeaderCallback(void *buffer, size_t size, size_t nmemb) {
    size_t write_size = size * nmemb;
    string header((char *)buffer, write_size);
    response_headers_.push_back(header);

    return write_size;
}

size_t NbHttpHandler::WriteCallback(char *buffer, size_t size, size_t nmemb) {
    size_t write_size = size * nmemb;
    auto before_size = response_body_.size();
    response_body_.resize(before_size + write_size);
    std::copy(buffer, buffer + write_size, response_body_.begin() + before_size);

    return write_size;
}

NbHttpResponse NbHttpHandler::Parse() {
    int status_code{0};
    string reason_phrase{};

    //レスポンスの１行目からステータスコードとリーズンフレーズを取得
    auto header_line = ParseStatusLine(&status_code, &reason_phrase);

    std::multimap<std::string, std::string> headers{};
    //２行目以降はヘッダ情報
    for (; header_line != response_headers_.end(); ++header_line) {
        auto index = header_line->find_first_of(':');
        if (index != string::npos) {
            // assume normal header
            // key
            string header_key = header_line->substr(0, index);
            // value
            string header_value = header_line->substr(index + 1);
            // remove \r\n at the tail
            if (header_value.length() >= 2) {
                header_value = header_value.substr(0, header_value.length() - 2);
            } else {
                //異常値(改行コード(CRLF)は必須)
                header_value.clear();
            }
            // 前方スペースを削除
            string::size_type not_space = header_value.find_first_not_of(' ');
            if (not_space == string::npos) {
                header_value.clear();
            } else if (not_space > 0) {
                header_value = header_value.substr(not_space);
            }

            headers.insert(std::make_pair(header_key, header_value));
        }
    }

    // HTTPレスポンス設定
    NbHttpResponse http_response(status_code, reason_phrase, headers, std::move(response_body_));
    return http_response;
}

vector<string>::iterator NbHttpHandler::SearchStatusLine() {
    //ステータスラインを後方検索
    // PROXYを使用する場合、HTTP CONNECTの情報も取得してしまうため
    auto it = response_headers_.end();

    if (response_headers_.empty()) {
        return it;
    }
    
    for (auto header_line = response_headers_.end() - 1; ; --header_line) {
        if ((header_line->length() > kSizeStatusLineMarker + kStatusCodeSize) &&
            (header_line->substr(0, kSizeStatusLineMarker) == kStatusCodeLineMarker)) {
            it = header_line;
            break;
        }

        // デクリメント前に脱出条件評価
        if (header_line == response_headers_.begin()) {
            break;
        }
    }

    return it;
}

vector<string>::iterator NbHttpHandler::ParseStatusLine(int *status_code, string *reason_phrase) {
    vector<string>::iterator status_line = SearchStatusLine();

    if (status_line == response_headers_.end()) {
        NBLOG(ERROR) << "There is no status line.";
        return status_line;
    }

    string statusCodeStr = status_line->substr(kSizeStatusLineMarker, kStatusCodeSize);
    if (status_code) {
        *status_code = std::atoi(statusCodeStr.c_str());
    }

    if (reason_phrase && (status_line->length() > kSizeStatusLineMarker + kStatusCodeSize + 1) &&
        ((*status_line)[kSizeStatusLineMarker + kStatusCodeSize] == ' ')) {
        *reason_phrase = status_line->substr(kSizeStatusLineMarker + kStatusCodeSize + 1);
        // remove \r\n at the tail
        if (reason_phrase->length() >= 2) {
            *reason_phrase = reason_phrase->substr(0, reason_phrase->length() - 2);
        } else {
            // 異常値(改行コード(CRLF)は必須)
            // ただし、ステータスコードの取得は成功とする
            reason_phrase->clear();
        }
    }

    return ++status_line;
}
}  // namespace necbaas
