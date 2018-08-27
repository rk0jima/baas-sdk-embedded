/*
 * Copyright (C) 2017 NEC Corporation
 */

#include "necbaas/internal/nb_http_request.h"
#include "necbaas/internal/nb_logger.h"
#include "necbaas/internal/nb_constants.h"

namespace necbaas {

using std::string;
using std::list;
using std::vector;

NbHttpRequest::NbHttpRequest(const string &url, const NbHttpRequestMethod &method, const list<string> &headers,
                             const string &body, const string &proxy)
    : url_(url), method_(method), headers_(headers), body_(body), proxy_(proxy) {}

NbHttpRequest::~NbHttpRequest() {}

const string &NbHttpRequest::GetUrl() const { return url_; }

const NbHttpRequestMethod &NbHttpRequest::GetMethod() const { return method_; }

const list<string> &NbHttpRequest::GetHeaders() const { return headers_; }

const string &NbHttpRequest::GetBody() const { return body_; }

const string &NbHttpRequest::GetProxy() const { return proxy_; }

void NbHttpRequest::Dump() const {
    if (!NbLogger::IsRestLogEnabled()) {
        // RESTログ有効時のみ実行
        return;
    }

    string method;
    switch (method_) {
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET:
            method = "GET";
            break;
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT:
            method = "PUT";
            break;
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST:
            method = "POST";
            break;
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE:
            method = "DELETE";
            break;
    }
    NBLOG(INFO) << "[request][method] " << method;
    NBLOG(INFO) << "[request][url] " << url_;

    auto &headers = GetHeaders();
    string content_type;
    for (auto i : GetHeaders()) {
        NBLOG(INFO) << "[request][header] " << i;
        if (i.compare(0, string(kHeaderContentType).size(), string(kHeaderContentType)) == 0) {
            content_type = i;
        }
    }

    if (!content_type.empty()) {
        const vector<string> text_types{"application/json", "text/plain", "text/html", "text/xml"};
        for (auto text_type : text_types) {
            if (content_type.find(text_type) != string::npos) {
                //テキスト形式のみBody部を出力
                NBLOG(INFO) << "[request][body] " << string(GetBody().begin(), GetBody().end());
                break;
            }
        }
    }
}
}  // namespace necbaas
