/*
 * Copyright (C) 2017 NEC Corporation
 */

#include "necbaas/internal/nb_rest_executor.h"
#include <curlpp/Options.hpp>
#include "necbaas/internal/nb_logger.h"
#include "necbaas/internal/nb_http_file_upload_handler.h"
#include "necbaas/internal/nb_http_file_download_handler.h"
#include "necbaas/internal/nb_utility.h"

namespace necbaas {

using std::string;

NbRestExecutor::NbRestExecutor() {}
NbRestExecutor::~NbRestExecutor() {}

NbResult<NbHttpResponse> NbRestExecutor::ExecuteFileUpload(const NbHttpRequest &request, const string &file_path,
                                                           int timeout) {
    NBLOG(TRACE) << "Execute file upload: " << file_path;
    request.Dump();

    auto file_size = NbUtility::GetFileSize(file_path);

    NbHttpFileUploadHandler http_handler(file_path);
    if (http_handler.IsError()) {
        //ファイルオープン失敗
        NBLOG(ERROR) << "File open error.";
        return MakeResult(http_handler, NbResultCode::NB_ERROR_OPEN_FILE);
    }

    try {
        SetOptCommon(request, http_handler, timeout);

        // 送信ファイル読み出しコールバック登録
        using namespace std::placeholders;
        curlpp::types::ReadFunctionFunctor data_reader =
            std::bind(&NbHttpFileUploadHandler::ReadCallback, &http_handler, _1, _2, _3);
        curlpp_easy_.setOpt(new curlpp::Options::ReadFunction(data_reader));

        // データ受信コールバック登録
        curlpp::types::WriteFunctionFunctor data_writer =
            std::bind(&NbHttpFileUploadHandler::WriteCallback, &http_handler, _1, _2, _3);
        curlpp_easy_.setOpt(new curlpp::Options::WriteFunction(data_writer));

        // create headers
        auto request_headers = request.GetHeaders();

        // アップロードはPUT・POSTのみ
        switch (request.GetMethod()) {
            case NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT:
                curlpp_easy_.setOpt(new curlpp::Options::Upload(true));

                request_headers.push_back(kHeaderContentLength + ": " + std::to_string(file_size));
                break;

            case NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST:
                // new file upload
                curlpp_easy_.setOpt(new curlpp::Options::Post(true));

                request_headers.push_back(kHeaderContentLength + ": " + std::to_string(file_size));
                break;

            default: {
                NBLOG(ERROR) << "Unexpected request type";
                return MakeResult(http_handler, NbResultCode::NB_FATAL);
            }
        }

        // HTTPヘッダ登録
        curlpp_easy_.setOpt(new curlpp::Options::HttpHeader(request_headers));

        // HTTPリクエスト実行
        Execute();
    }
    catch (const curlpp::LibcurlRuntimeError &ex) {
        int code = static_cast<int>(ex.whatCode());
        NBLOG(ERROR) << "LibcurlRuntimeError error detected code:" << code;
        return MakeResult(http_handler, NbResultCode::NB_ERROR_CURL_RUNTIME);
    }
    catch (const curlpp::LibcurlLogicError &ex) {
        int code = static_cast<int>(ex.whatCode());
        NBLOG(ERROR) << "LibcurlLogicError error detected code:" << code;
        return MakeResult(http_handler, NbResultCode::NB_ERROR_CURL_LOGIC);
    }
    catch (...) {
        NBLOG(ERROR) << "unexpected error detected";
        return MakeResult(http_handler, NbResultCode::NB_ERROR_CURL_FATAL);
    }

    return MakeResult(http_handler, NbResultCode::NB_OK);
}

NbResult<NbHttpResponse> NbRestExecutor::ExecuteFileDownload(const NbHttpRequest &request, const string &file_path,
                                                             int timeout) {
    NBLOG(TRACE) << "Execute file download: " << file_path;
    request.Dump();

    NbHttpFileDownloadHandler http_handler(file_path);
    if (http_handler.IsError()) {
        //ファイルオープン失敗
        NBLOG(ERROR) << "File open error.";
        return MakeResult(http_handler, NbResultCode::NB_ERROR_OPEN_FILE);
    }

    try {
        SetOptCommon(request, http_handler, timeout);

        // 受信ファイル書込みコールバック登録
        using namespace std::placeholders;
        curlpp::types::WriteFunctionFunctor data_writer =
            std::bind(&NbHttpFileDownloadHandler::WriteCallback, &http_handler, _1, _2, _3);
        curlpp_easy_.setOpt(new curlpp::Options::WriteFunction(data_writer));

        // ファイルダウンロードはGETのみ
        switch (request.GetMethod()) {
            case NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET:
                curlpp_easy_.setOpt(new curlpp::Options::HttpGet(true));
                break;

            default: {
                NBLOG(ERROR) << "Unexpected request type";
                return MakeResult(http_handler, NbResultCode::NB_FATAL);
            }
        }

        // HTTPヘッダ登録
        curlpp_easy_.setOpt(new curlpp::Options::HttpHeader(request.GetHeaders()));

        // HTTPリクエスト実行
        Execute();
    }
    catch (const curlpp::LibcurlRuntimeError &ex) {
        int code = static_cast<int>(ex.whatCode());
        NBLOG(ERROR) << "LibcurlRuntimeError error detected code:" << code;
        return MakeResult(http_handler, NbResultCode::NB_ERROR_CURL_RUNTIME);
    }
    catch (const curlpp::LibcurlLogicError &ex) {
        int code = static_cast<int>(ex.whatCode());
        NBLOG(ERROR) << "LibcurlLogicError error detected code:" << code;
        return MakeResult(http_handler, NbResultCode::NB_ERROR_CURL_LOGIC);
    }
    catch (...) {
        NBLOG(ERROR) << "unexpected error detected";
        return MakeResult(http_handler, NbResultCode::NB_ERROR_CURL_FATAL);
    }

    NbResult<NbHttpResponse> response = MakeResult(http_handler, NbResultCode::NB_OK);

    //ダウンロードしたファイルサイズの検証
    if (response.IsSuccess() && !ValidateFileSize(response.GetSuccessData(), file_path)) {
        NBLOG(ERROR) << "A file size is different.";
        response.SetResultCode(NbResultCode::NB_ERROR_FILE_DOWNLOAD);
    }

    return response;
}

bool NbRestExecutor::ValidateFileSize(const NbHttpResponse &response, const string &file_path) const {
    int header_value = 0;

    auto &headers = response.GetHeaders();
    auto x_content_length = headers.find(kHeaderXContentLength);
    if (x_content_length == headers.end()) {
        // X-Content-Lengthヘッダは必須
        NBLOG(ERROR) << "There is no X-Content-Length.";
        return false;
    }
    header_value = std::atoi(x_content_length->second.c_str());

    int file_size = NbUtility::GetFileSize(file_path);

    return (header_value == file_size);
}

NbResult<NbHttpResponse> NbRestExecutor::ExecuteRequest(const NbHttpRequest &request, int timeout) {
    NBLOG(TRACE) << "Execute request.";
    request.Dump();

    NbHttpHandler http_handler;

    try {
        SetOptCommon(request, http_handler, timeout);

        // データ受信コールバック登録
        using namespace std::placeholders;
        curlpp::types::WriteFunctionFunctor data_writer =
            std::bind(&NbHttpHandler::WriteCallback, &http_handler, _1, _2, _3);
        curlpp_easy_.setOpt(new curlpp::Options::WriteFunction(data_writer));

        switch (request.GetMethod()) {
            case NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET:
                curlpp_easy_.setOpt(new curlpp::Options::HttpGet(true));
                break;
            case NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT:
                curlpp_easy_.setOpt(new curlpp::Options::CustomRequest("PUT"));
                curlpp_easy_.setOpt(new curlpp::Options::PostFields(request.GetBody()));
                curlpp_easy_.setOpt(new curlpp::Options::PostFieldSize(request.GetBody().length()));
                break;
            case NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST:
                curlpp_easy_.setOpt(new curlpp::Options::Post(true));
                curlpp_easy_.setOpt(new curlpp::Options::PostFields(request.GetBody()));
                curlpp_easy_.setOpt(new curlpp::Options::PostFieldSize(request.GetBody().length()));
                break;
            case NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE:
                curlpp_easy_.setOpt(new curlpp::Options::CustomRequest("DELETE"));
                break;
        }

        // HTTPヘッダ登録
        curlpp_easy_.setOpt(new curlpp::Options::HttpHeader(request.GetHeaders()));

        // HTTPリクエスト実行
        Execute();
    }
    catch (const curlpp::LibcurlRuntimeError &ex) {
        int code = static_cast<int>(ex.whatCode());
        NBLOG(ERROR) << "LibcurlRuntimeError error detected code:" << code;
        return MakeResult(http_handler, NbResultCode::NB_ERROR_CURL_RUNTIME);
    }
    catch (const curlpp::LibcurlLogicError &ex) {
        int code = static_cast<int>(ex.whatCode());
        NBLOG(ERROR) << "LibcurlLogicError error detected code:" << code;
        return MakeResult(http_handler, NbResultCode::NB_ERROR_CURL_LOGIC);
    }
    catch (...) {
        NBLOG(ERROR) << "unexpected error detected";
        return MakeResult(http_handler, NbResultCode::NB_ERROR_CURL_FATAL);
    }

    return MakeResult(http_handler, NbResultCode::NB_OK);
}

void NbRestExecutor::SetOptCommon(const NbHttpRequest &request, NbHttpHandler &http_handler, int timeout) {
    // CURLオプションリセット
    curlpp_easy_.reset();

    // URL
    curlpp_easy_.setOpt(new curlpp::Options::Url(request.GetUrl()));

    // HTTPヘッダ受信コールバック登録
    using namespace std::placeholders;
    curlpp::types::WriteFunctionFunctor header_writer =
        std::bind(&NbHttpHandler::WriteHeaderCallback, &http_handler, _1, _2, _3);
    curlpp_easy_.setOpt(new curlpp::Options::HeaderFunction(header_writer));

    // Proxy設定
    // 空文字でも設定する(明示的にProxy未使用)
    curlpp_easy_.setOpt(new curlpp::Options::Proxy(request.GetProxy()));

    // タイムアウト設定
    curlpp_easy_.setOpt(new curlpp::Options::Timeout(timeout < 0 ? kRestTimeoutDefault : timeout));
    // タイムアウト用にシグナルは使用しない
    curlpp_easy_.setOpt(new curlpp::Options::NoSignal(true));

    // CURLログ設定
    curlpp_easy_.setOpt(new curlpp::Options::Verbose(NbLogger::IsDebugLogEnabled()));
}

NbResult<NbHttpResponse> NbRestExecutor::MakeResult(NbHttpHandler &http_handler, NbResultCode result_code) {
    NbResult<NbHttpResponse> result(result_code);

    NbHttpResponse response = http_handler.Parse();

    // エラーが発生していてもステータスコードを受信している場合はRestErrorに上書きする
    // ただし、ステータスコードが300未満の場合はFatalエラーを優先する。
    if (result.IsFatalError() && response.GetStatusCode() < 300) {
        NBLOG(ERROR) << "Execute HTTP request error.";
        return result;
    }

    response.Dump();
    result.SetSuccessData(response);

    // ステータスコード取得失敗の場合は、不正レスポンス
    if (result.GetSuccessData().GetStatusCode() == 0) {
        result.SetResultCode(NbResultCode::NB_ERROR_INCORRECT_RESPONSE);
        NBLOG(ERROR) << "There is no status code.";
        return result;
    }

    // 200台以外は、RestErrorに設定
    if (result.GetSuccessData().GetStatusCode() / 100 != 2) {
        result.SetResultCode(NbResultCode::NB_ERROR_RESPONSE);
        NbRestError rest_error;
        rest_error.status_code = result.GetSuccessData().GetStatusCode();
        rest_error.reason.assign(result.GetSuccessData().GetBody().begin(), result.GetSuccessData().GetBody().end());
        result.SetRestError(std::move(rest_error));
    }

    return result;
}

void NbRestExecutor::Execute() {
    // for test
    curlpp_easy_.perform();
}
}  // namespace necbaas
