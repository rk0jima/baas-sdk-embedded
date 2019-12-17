/*
 * Copyright (C) 2017-2019 NEC Corporation
 */

#include "necbaas/nb_service.h"
#include <curlpp/cURLpp.hpp>
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::shared_ptr;

static std::mutex mutex_curl;

// サービス生成
shared_ptr<NbService> NbService::CreateService(const string &endpoint_url, const string &tenant_id,
                                               const string &app_id, const string &app_key, const string &proxy) {
    NBLOG(TRACE) << __func__;
    return shared_ptr<NbService>(new NbService(endpoint_url, tenant_id, app_id, app_key, proxy));
}

void NbService::SetDebugLogEnabled(bool flag) {
    NbLogger::SetDebugLogEnabled(flag);
}

void NbService::SetErrorLogEnabled(bool flag) {
    NbLogger::SetErrorLogEnabled(flag);
}

void NbService::SetRestLogEnabled(bool flag) {
    NbLogger::SetRestLogEnabled(flag);
}

// コンストラクタ
NbService::NbService(const string &endpoint_url, const string &tenant_id, const string &app_id,
                     const string &app_key, const string &proxy)
        : endpoint_url_(endpoint_url), tenant_id_(tenant_id), app_id_(app_id), app_key_(app_key), proxy_(proxy), http_options_() {
    // curl_global_init()がスレッドセーフでないため、排他する
    std::lock_guard<std::mutex> lock(mutex_curl);
    curlpp::initialize();
}

// デストラクタ
NbService::~NbService() {
    // curl_global_cleanup()がスレッドセーフでないため、排他する
    std::lock_guard<std::mutex> lock(mutex_curl);
    curlpp::terminate();
}

// Getter
const string &NbService::GetAppId() const {
    return app_id_;
}

const string &NbService::GetAppKey() const {
    return app_key_;
}

const string &NbService::GetEndpointUrl() const {
    return endpoint_url_;
}

const string &NbService::GetTenantId() const {
    return tenant_id_;
}

const string &NbService::GetProxy() const {
    return proxy_;
}

NbSessionToken NbService::GetSessionToken() {
    std::lock_guard<std::mutex> lock(session_token_mutex_);
    return session_token_;
}
void NbService::SetSessionToken(const NbSessionToken &token) {
    std::lock_guard<std::mutex> lock(session_token_mutex_);
    session_token_ = token;
}

void NbService::ClearSessionToken() {
    std::lock_guard<std::mutex> lock(session_token_mutex_);
    session_token_.ClearSessionToken();
}

void NbService::SetHttpOptions(const NbHttpOptions &options) {
    http_options_ = options;
}

NbRestExecutor *NbService::PopRestExecutor() {
    return rest_executor_pool_.PopRestExecutor();
}

void NbService::PushRestExecutor(NbRestExecutor *executor) {
    rest_executor_pool_.PushRestExecutor(executor);
}

NbHttpRequestFactory NbService::GetHttpRequestFactory() {
    std::lock_guard<std::mutex> lock(session_token_mutex_);
    return NbHttpRequestFactory(endpoint_url_, tenant_id_, app_id_, app_key_,
                                session_token_.GetSessionToken(), proxy_, http_options_);
}

NbResult<NbHttpResponse> NbService::ExecuteCommon(
    std::function<NbHttpRequest(NbHttpRequestFactory &)> create_request,
    std::function<NbResult<NbHttpResponse>(NbRestExecutor *, const NbHttpRequest &)> executor_method) {
                                                
    NbResult<NbHttpResponse> result;
    //HTTPリクエスト作成
    NbHttpRequestFactory request_factory = GetHttpRequestFactory();
    if (request_factory.IsError()) {
        //request構築エラー
        result.SetResultCode(request_factory.GetError());
        return result;
    }

    //呼び元のリクエスト作成関数を実行
    NbHttpRequest request = create_request(request_factory);

    //リクエスト実行
    NbRestExecutor *executor = PopRestExecutor();
    if (!executor) {
        // 同時接続数オーバー
        result.SetResultCode(NbResultCode::NB_ERROR_CONNECTION_OVER);
        return result;
    }
    result = executor_method(executor, request);
    PushRestExecutor(executor);
    return result;
}

NbResult<NbHttpResponse> NbService::ExecuteRequest(std::function<NbHttpRequest(NbHttpRequestFactory &)> create_request, int timeout) {
    return ExecuteCommon(create_request, 
        [timeout](NbRestExecutor *executor, const NbHttpRequest &request) {
            return executor->ExecuteRequest(request, timeout);
        });
}

NbResult<NbHttpResponse> NbService::ExecuteFileDownload(std::function<NbHttpRequest(NbHttpRequestFactory &)> create_request,
                                                        const std::string &file_path, int timeout) {
    return ExecuteCommon(create_request,
        [&file_path, timeout](NbRestExecutor *executor, const NbHttpRequest &request) {
            return executor->ExecuteFileDownload(request, file_path, timeout);
        });
}

NbResult<NbHttpResponse> NbService::ExecuteFileUpload(std::function<NbHttpRequest(NbHttpRequestFactory &)> create_request,
                                                      const std::string &file_path, int timeout) {
    return ExecuteCommon(create_request,
        [&file_path, timeout](NbRestExecutor *executor, const NbHttpRequest &request) {
            return executor->ExecuteFileUpload(request, file_path, timeout);
        });
}
} //namespace necbaas
