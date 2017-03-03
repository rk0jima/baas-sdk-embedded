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

#include "necbaas/nb_api_gateway.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::multimap;
using std::shared_ptr;

// コンストラクタ
NbApiGateway::NbApiGateway() {}

NbApiGateway::NbApiGateway(const shared_ptr<NbService> &service, const string &api_name,
                           NbHttpRequestMethod http_method, const string &subpath)
        : service_(service), api_name_(api_name), http_method_(http_method), subpath_(subpath) {}

// デストラクタ
NbApiGateway::~NbApiGateway() {}


NbResult<NbHttpResponse> NbApiGateway::ExecuteCustomApi() {
    string body_string{};
    return ExecuteCustomApi(body_string);
}

NbResult<NbHttpResponse> NbApiGateway::ExecuteCustomApi(const std::string &body) {
    NBLOG(DEBUG) << __func__;

    NbResult<NbHttpResponse> result;

    if (api_name_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_API_NAME);
        NBLOG(ERROR) << "Api name is empty.";
        return result;
    }

    //HTTPリクエスト作成
    NbHttpRequestFactory request_factory = service_->GetHttpRequestFactory();
    if (request_factory.IsError()) {
        //request構築エラー
        result.SetResultCode(request_factory.GetError());
        return result;
    }

    switch (http_method_) {
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET:
            request_factory.Get(kApigwUrl)
                           .Headers(headers_);
            break;
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST:
            request_factory.Post(kApigwUrl)
                           .Headers(headers_)
                           .Body(body);
            if (!body.empty()) {
                if (content_type_.empty()) {
                    //エラー処理
                    result.SetResultCode(NbResultCode::NB_ERROR_CONTENT_TYPE);
                    NBLOG(ERROR) << "Conten-Type is empty.";
                    return result;
                }
                request_factory.AppendHeader(kHeaderContentType, content_type_);
            }
            break;
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT:
            request_factory.Put(kApigwUrl)
                           .Headers(headers_)
                           .Body(body);
            if (!body.empty()) {
                if (content_type_.empty()) {
                    //エラー処理
                    result.SetResultCode(NbResultCode::NB_ERROR_CONTENT_TYPE);
                    NBLOG(ERROR) << "Conten-Type is empty.";
                    return result;
                }
                request_factory.AppendHeader(kHeaderContentType, content_type_);
            }
            break;
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE:
            request_factory.Delete(kApigwUrl)
                           .Headers(headers_);
            break;
    }

    NbHttpRequest request = request_factory.AppendPath("/" + api_name_ + subpath_)
                                           .Params(parameters_)
                                           .Build();
    //リクエスト実行
    NbRestExecutor *executor = service_->PopRestExecutor();
    if (!executor) {
        // 同時接続数オーバー
        result.SetResultCode(NbResultCode::NB_ERROR_CONNECTION_OVER);
        return result;
    }

    result = executor->ExecuteJsonRequest(request, timeout_);
    service_->PushRestExecutor(executor);

    return result;
}

NbResult<NbHttpResponse> NbApiGateway::ExecuteCustomApi(const std::vector<char> &body) {
    string body_string(body.begin(), body.end());
    return ExecuteCustomApi(body_string);
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

    if (name != kHeaderContentType) {
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
} //namespace necbaas
