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

#include "necbaas/internal/nb_http_request_factory.h"
#include <curlpp/cURLpp.hpp>
#include "necbaas/internal/nb_constants.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::list;
using std::multimap;

NbHttpRequestFactory::NbHttpRequestFactory(const string &end_point_url, const string &tenant_id, const string &app_id,
                                           const string &app_key, const string &sessnon_token, const string &proxy)
    : end_point_url_(end_point_url),
      tenant_id_(tenant_id),
      app_id_(app_id),
      app_key_(app_key),
      sessnon_token_(sessnon_token),
      proxy_(proxy) {
    CheckParameter();
}

NbHttpRequestFactory::~NbHttpRequestFactory() {}

NbHttpRequestFactory &NbHttpRequestFactory::Get(const string &api_uri) {
    request_method_ = NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET;
    path_ = api_uri;
    return *this;
}

NbHttpRequestFactory &NbHttpRequestFactory::Post(const string &api_uri) {
    request_method_ = NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST;
    path_ = api_uri;
    return *this;
}

NbHttpRequestFactory &NbHttpRequestFactory::Put(const string &api_uri) {
    request_method_ = NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT;
    path_ = api_uri;
    return *this;
}

NbHttpRequestFactory &NbHttpRequestFactory::Delete(const string &api_uri) {
    request_method_ = NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE;
    path_ = api_uri;
    return *this;
}

NbHttpRequestFactory &NbHttpRequestFactory::AppendPath(const string &path) {
    path_.append(path);
    return *this;
}

NbHttpRequestFactory &NbHttpRequestFactory::Params(const multimap<string, string> &params) {
    request_params_ = params;
    return *this;
}

NbHttpRequestFactory &NbHttpRequestFactory::AppendParam(const string &key, const string &value) {
    if (!key.empty() && !value.empty()) {
        request_params_.insert(std::make_pair(key, value));
    }
    return *this;
}

NbHttpRequestFactory &NbHttpRequestFactory::Headers(const multimap<string, string> &headers) {
    headers_ = headers;
    return *this;
}

NbHttpRequestFactory &NbHttpRequestFactory::AppendHeader(const string &key, const string &value) {
    if (!key.empty() && !value.empty()) {
        headers_.insert(std::make_pair(key, value));
    }
    return *this;
}

NbHttpRequestFactory &NbHttpRequestFactory::Body(const string &body) {
    body_ = body;
    return *this;
}

NbHttpRequestFactory &NbHttpRequestFactory::SessionNone() {
    session_none_ = true;
    return *this;
}

NbHttpRequest NbHttpRequestFactory::Build() {
    auto url_params = CreateRequestParams();
    // end_point_url_の最後の'/'
    if (*end_point_url_.rbegin() != '/') {
        end_point_url_ += "/";
    }
    auto url = end_point_url_ + kPathApiVersion + "/" + tenant_id_ + path_ + url_params;

    // Set common headers
    headers_.insert(std::make_pair(kHeaderAppId, app_id_));
    headers_.insert(std::make_pair(kHeaderAppKey, app_key_));

    // User-Agent
    if (headers_.count(kHeaderUserAgent) == 0) {
        headers_.insert(std::make_pair(kHeaderUserAgent, kHeaderUserAgentDefault));
    }

    if (!session_none_ && !sessnon_token_.empty()) {
        headers_.insert(std::make_pair(kHeaderSessionToken, sessnon_token_));
    }

    auto header_list = list<string>();
    for (auto itor : headers_) {
        if (!itor.first.empty() && !itor.second.empty()) {
            auto line = itor.first + ": " + itor.second;
            header_list.push_back(line);
        }
    }

    return NbHttpRequest(url, request_method_, header_list, body_, proxy_);
}

NbResultCode NbHttpRequestFactory::GetError() const { return error_; }

bool NbHttpRequestFactory::IsError() const { return (error_ != NbResultCode::NB_OK); }

NbResultCode NbHttpRequestFactory::CheckParameter() {
    if (end_point_url_.empty()) {
        NBLOG(ERROR) << "End point URL is empty.";
        error_ = NbResultCode::NB_ERROR_ENDPOINT_URL;
    } else if (tenant_id_.empty()) {
        NBLOG(ERROR) << "Tenant ID is empty.";
        error_ = NbResultCode::NB_ERROR_TENANT_ID;
    } else if (app_id_.empty()) {
        NBLOG(ERROR) << "App ID is empty.";
        error_ = NbResultCode::NB_ERROR_APP_ID;
    } else if (app_key_.empty()) {
        NBLOG(ERROR) << "App Key is empty.";
        error_ = NbResultCode::NB_ERROR_APP_KEY;
    }
}

string NbHttpRequestFactory::CreateRequestParams() {
    string converted_params;
    if (request_params_.empty()) {
        return converted_params;
    }

    converted_params = "?";
    string escaped_first;
    string escaped_second;
    for (auto itor = request_params_.begin(); itor != request_params_.end(); ++itor) {
        if (!itor->first.empty() && !itor->second.empty()) {
            escaped_first = curlpp::escape(itor->first);
            escaped_second = curlpp::escape(itor->second);
            converted_params += escaped_first + "=" + escaped_second + "&";
        }
    }
    // remove an excess '&' or '?'
    converted_params.erase(--converted_params.end());

    return converted_params;
}
}  // namespace necbaas
