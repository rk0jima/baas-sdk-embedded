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

#include "necbaas/nb_http_response.h"
#include "necbaas/internal/nb_logger.h"
#include "necbaas/internal/nb_constants.h"

namespace necbaas {

using std::string;
using std::vector;
using std::multimap;

NbHttpResponse::NbHttpResponse() {};

NbHttpResponse::NbHttpResponse(int status_code, const string &reason_phrase,
                               const multimap<string, string> &headers,
                               const vector<char> &body) 
        : status_code_(status_code), reason_phrase_(reason_phrase), headers_(headers), body_(body) {}

NbHttpResponse::~NbHttpResponse() {};

int NbHttpResponse::GetStatusCode() const {
    return status_code_;
}

const string &NbHttpResponse::GetReasonPhrase() const {
    return reason_phrase_;
}

const multimap<string, string> &NbHttpResponse::GetHeaders() const {
    return headers_;
}

const vector<char> &NbHttpResponse::GetBody() const {
    return body_;
}

void NbHttpResponse::Dump() const {
    if (!NbLogger::IsRestLogEnabled()) {
        //RESTログ有効時のみ実行
        return;
    }

    NBLOG(INFO) << "[response][status-code] " << status_code_;
    NBLOG(INFO) << "[response][reason-phrase] " << reason_phrase_;

    auto &headers = GetHeaders();
    for (auto i: GetHeaders()) {
        NBLOG(INFO) << "[response][header] " << i.first << ": " << i.second;
    }
    
    const vector<string> text_types{"application/json",
                                    "text/plain",
                                    "text/html",
                                    "text/xml"};

    auto content_type = headers.find(kHeaderContentType);
    if (content_type == headers.end()) {
        return;
    }

    for (auto text_type : text_types) {
        if (content_type->second.compare(0, text_type.size(), text_type) == 0) {
            //テキスト形式のみBody部を出力
            NBLOG(INFO) << "[response][body] " << string(GetBody().begin(), GetBody().end());
            break; 
        }
    }
}
} //namespace necbaas

