/*
 * Copyright (C) 2017 NEC Corporation
 */

#include "necbaas/nb_api_gateway.h"
#include "necbaas/internal/nb_utility.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::multimap;
using std::shared_ptr;

// コンストラクタ
NbApiGateway::NbApiGateway(const shared_ptr<NbService> &service, const string &api_name,
                           NbHttpRequestMethod http_method, const string &subpath)
        : service_(service), api_name_(api_name), http_method_(http_method), subpath_(subpath) {}

// デストラクタ
NbApiGateway::~NbApiGateway() {}

NbResult<NbHttpResponse> NbApiGateway::ExecuteCustomApi(const std::string &body) {
    NBLOG(TRACE) << __func__;

    NbResult<NbHttpResponse> result;

    if (api_name_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_API_NAME);
        NBLOG(ERROR) << "Api name is empty.";
        return result;
    }

    //Content-Typeのチェック
    if (!CheckContentType(body)) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_CONTENT_TYPE);
        NBLOG(ERROR) << "Content-Type is empty.";
        return result;
    }

    return service_->ExecuteRequest(
        [this, &body](NbHttpRequestFactory &request_factory) -> NbHttpRequest {
            switch (http_method_) {
                case NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET:
                    request_factory.Get(kApigwUrl)
                                   .Params(parameters_)
                                   .Headers(headers_);
                    break;
                case NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST:
                    request_factory.Post(kApigwUrl)
                                   .Headers(headers_)
                                   .Body(body);
                    AppendContentType(body, &request_factory);
                    break;
                case NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT:
                    request_factory.Put(kApigwUrl)
                                   .Headers(headers_)
                                   .Body(body);
                    AppendContentType(body, &request_factory);
                    break;
                case NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE:
                    request_factory.Delete(kApigwUrl)
                                   .Params(parameters_)
                                   .Headers(headers_);
                    break;
            }
            return  request_factory.AppendPath("/" + api_name_ + subpath_)
                                   .Build();
        }, timeout_);
}

NbResult<NbHttpResponse> NbApiGateway::ExecuteCustomApi(const std::vector<char> &body) {
    string body_string(body.begin(), body.end());
    return ExecuteCustomApi(body_string);
}

bool NbApiGateway::CheckContentType(const string &body) {
    if (http_method_ == NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST ||
        http_method_ == NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT) {
        return (body.empty() || !content_type_.empty());
    }

    return true;
}

void NbApiGateway::AppendContentType(const string &body, NbHttpRequestFactory *request_factory) {
    if (!body.empty()) {
        // content_type_が空でないことはチェック済の想定
        request_factory->AppendHeader(kHeaderContentType, content_type_);
    }
}

int NbApiGateway::GetTimeout() const {
    return timeout_;
}

void NbApiGateway::SetTimeout(int timeout) {
    timeout_ = timeout;
}

const multimap<string, string> &NbApiGateway::GetHeaders() const {
    return headers_;
}

void NbApiGateway::AddHeader(const string &name, const string &value) {
    if (name.empty() || value.empty()) {
        return;
    }

    if (IsReservedHeaderName(name)) {
        // SDKで自動付与するヘッダは無視する
        return ;
    }

    if (NbUtility::CompareCaseInsensitiveString(name, kHeaderUserAgent)) {
        // User-Agentは、大文字小文字を意識し、上書き更新
        headers_.erase(kHeaderUserAgent);
        headers_.insert(std::make_pair(kHeaderUserAgent, value));
    } else {
        headers_.insert(std::make_pair(name, value));
    }
}

int NbApiGateway::RemoveHeader(const string &name) {
    return (int)headers_.erase(name);
}

void NbApiGateway::ClearHeaders() {
    headers_.clear();
}

const multimap<string, string> &NbApiGateway::GetParameters() const {
    return parameters_;
}

void NbApiGateway::AddParameter(const string &name, const string &value) {
    if (name.empty() || value.empty()) {
        return;
    }
    parameters_.insert(std::make_pair(name, value));
}

int NbApiGateway::RemoveParameter(const string &name) {
    return (int)parameters_.erase(name);
}

void NbApiGateway::ClearParameters() {
    parameters_.clear();
}

const string &NbApiGateway::GetContentType() const {
    return content_type_;
}

void NbApiGateway::SetContentType(const string &content_type) {
    content_type_ = content_type;
}

bool NbApiGateway::IsReservedHeaderName(const string &name) const { 
    // SDKで自動付与するヘッダかどうかの判別
    return (NbUtility::CompareCaseInsensitiveString(name, kHeaderContentType) ||
            NbUtility::CompareCaseInsensitiveString(name, kHeaderContentLength) ||
            NbUtility::CompareCaseInsensitiveString(name, kHeaderAppId) ||
            NbUtility::CompareCaseInsensitiveString(name, kHeaderAppKey) ||
            NbUtility::CompareCaseInsensitiveString(name, kHeaderSessionToken) ||
            NbUtility::CompareCaseInsensitiveString(name, kHeaderHost));
}
} //namespace necbaas
