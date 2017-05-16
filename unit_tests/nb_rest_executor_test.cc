#include "gtest/gtest.h"
#include "necbaas/internal/nb_rest_executor.h"
#include <curlpp/Options.hpp>
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::vector;

static const string kEmpty{""};
static const string kUrl{"http://localhost/api/1/tenantid/path"};
static const std::list<string> kReqHeaders{"X-HEADER-KEY1: abcdef", "X-HEADER-KEY2: 123456"};
static const vector<string> kRespHeaders = {
    "HTTP/1.1 200 OK\r\n",
    "Content-Type: application/json\r\n",
    "X-Content-Length: 20\r\n",
    "\r\n"};
static const char kBody[] = "1234567890abcdefghij";
static const string kProxy{"https://proxy.com/proxy:8080"};

static string MakeFilePath(string file_name) {
    string file_path = __FILE__;
    file_path.erase(file_path.rfind("/") + 1);
    file_path += "file/" + file_name;
    return file_path;
}

class NbRestExecutorTest : public NbRestExecutor {
public:
    void GetOpt(curlpp::OptionBase & option) const {
        try {
            curlpp_easy_.getOpt(option);
        } catch (curlpp::RuntimeError &ex) {
            std::cout << "exception!!" << std::endl;
            throw ex;
        }
    }

    NbResultCode error_trigger_{NbResultCode::NB_OK};
    vector<string> response_headers_{kRespHeaders};
    char *response_body_ = (char *)kBody;
    bool with_response_{false};

protected:
    void Execute() override {
        if (error_trigger_ == NbResultCode::NB_ERROR_CURL_RUNTIME && !with_response_) {
            throw curlpp::LibcurlRuntimeError(string("runtime exception"), (CURLcode)10);
        } else if (error_trigger_ == NbResultCode::NB_ERROR_CURL_LOGIC) {
            throw curlpp::LibcurlLogicError(string("logic exception"), (CURLcode)20);
        } else if (error_trigger_ == NbResultCode::NB_ERROR_CURL_FATAL) {
            throw std::exception();
        }

        curlpp::Options::HeaderFunction header_writer;
        GetOpt(header_writer);

        for (auto header : response_headers_) { 
             
            (header_writer.getValue())((char *)header.c_str(), 1, header.size());
        }

        curlpp::Options::WriteFunction writer;
        GetOpt(writer);
        (writer.getValue())((char *)response_body_, 1, strlen(response_body_));

        if (error_trigger_ == NbResultCode::NB_ERROR_CURL_RUNTIME && with_response_) {
            throw curlpp::LibcurlRuntimeError(string("runtime exception"), (CURLcode)10);
        }
        return;
    }    
};

static void CheckCurlOptionCommon(const NbRestExecutorTest &executor, const NbHttpRequest &request, int req_timeout = 60) {
    curlpp::Options::Url url;
    executor.GetOpt(url);
    EXPECT_EQ(request.GetUrl(), url.getValue());

    //コールバックはスキップ

    curlpp::Options::Proxy proxy;
    executor.GetOpt(proxy);
    EXPECT_EQ(request.GetProxy(), proxy.getValue());

    curlpp::Options::Timeout timeout;
    executor.GetOpt(timeout);
    EXPECT_EQ(req_timeout < 0 ? 60 : req_timeout, timeout.getValue());

    curlpp::Options::NoSignal signal;
    executor.GetOpt(signal);
    EXPECT_TRUE(signal.getValue());

    curlpp::Options::Verbose verbose;
    executor.GetOpt(verbose);
    if (NbLogger::IsDebugLogEnabled()) {
        EXPECT_TRUE(verbose.getValue());
    } else {
        EXPECT_FALSE(verbose.getValue());
    }
}

static void CheckCurlOption(const NbRestExecutorTest &executor, const NbHttpRequest &request, int req_timeout = 60) {
    CheckCurlOptionCommon(executor, request, req_timeout);

    switch (request.GetMethod()) {
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET: {
            curlpp::Options::HttpGet get;
            executor.GetOpt(get);
            EXPECT_TRUE(get.getValue());

            curlpp::Options::PostFields post_fields;
            executor.GetOpt(post_fields);
            EXPECT_THROW(post_fields.getValue(), curlpp::UnsetOption);

            curlpp::Options::PostFieldSize post_field_size;
            executor.GetOpt(post_field_size);
            EXPECT_THROW(post_field_size.getValue(), curlpp::UnsetOption);
            break;
        }
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT: {
            curlpp::Options::CustomRequest custom;
            executor.GetOpt(custom);
            EXPECT_EQ(string("PUT"), custom.getValue());

            curlpp::Options::PostFields post_fields;
            executor.GetOpt(post_fields);
            EXPECT_EQ(request.GetBody(), post_fields.getValue());

            curlpp::Options::PostFieldSize post_field_size;
            executor.GetOpt(post_field_size);
            EXPECT_EQ(request.GetBody().size(), post_field_size.getValue());
            break;
        }
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST: {
            curlpp::Options::Post post;
            executor.GetOpt(post);
            EXPECT_TRUE(post.getValue());

            curlpp::Options::PostFields post_fields;
            executor.GetOpt(post_fields);
            EXPECT_EQ(request.GetBody(), post_fields.getValue());

            curlpp::Options::PostFieldSize post_field_size;
            executor.GetOpt(post_field_size);
            EXPECT_EQ(request.GetBody().size(), post_field_size.getValue());
            break;
        }
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE: {
            curlpp::Options::CustomRequest custom;
            executor.GetOpt(custom);
            EXPECT_EQ(string("DELETE"), custom.getValue());

            curlpp::Options::PostFields post_fields;
            executor.GetOpt(post_fields);
            EXPECT_THROW(post_fields.getValue(), curlpp::UnsetOption);

            curlpp::Options::PostFieldSize post_field_size;
            executor.GetOpt(post_field_size);
            EXPECT_THROW(post_field_size.getValue(), curlpp::UnsetOption);
            break;
        }
    }

    curlpp::Options::HttpHeader headers;
    executor.GetOpt(headers);
    EXPECT_EQ(request.GetHeaders(), headers.getValue());
}

//NbRestExecutor::ExecuteRequest(GET, bodyなし、Proxyなし、タイムアウトデフォルト)
TEST(NbRestExecutor, ExecuteRequestGet) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOption(executor, request);
}

//NbRestExecutor::ExecuteRequest(GET, bodyあり、Proxyあり、タイムアウト0)
TEST(NbRestExecutor, ExecuteRequestGet2) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kBody, kProxy);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request, 0);

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOption(executor, request, 0);
}

//NbRestExecutor::ExecuteRequest(PUT, bodyなし、タイムアウト不正)
TEST(NbRestExecutor, ExecuteRequestPut1) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request, -1);

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOption(executor, request, -1);
}

//NbRestExecutor::ExecuteRequest(PUT, bodyあり, ログあり)
TEST(NbRestExecutor, ExecuteRequestPut2) {
    NbLogger::SetDebugLogEnabled(true);
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kReqHeaders, kBody, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOption(executor, request);
}

//NbRestExecutor::ExecuteRequest(POST, bodyなし, ログなし)
TEST(NbRestExecutor, ExecuteRequestPost1) {
    NbLogger::SetDebugLogEnabled(false);
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOption(executor, request);
}

//NbRestExecutor::ExecuteRequest(POST, bodyあり)
TEST(NbRestExecutor, ExecuteRequestPost2) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kReqHeaders, kBody, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOption(executor, request);
}

//NbRestExecutor::ExecuteRequest(DELETE, bodyなし)
TEST(NbRestExecutor, ExecuteRequestDelete1) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOption(executor, request);
}

//NbRestExecutor::ExecuteRequest(DELETE, bodyあり)
TEST(NbRestExecutor, ExecuteRequestDelete2) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, kReqHeaders, kBody, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOption(executor, request);
}

//NbRestExecutor::ExecuteRequest(ステータスコードなし)
TEST(NbRestExecutor, ExecuteRequestStatuCodeNone) {
    NbRestExecutorTest executor;

    //ヘッダ情報上書き
    executor.response_headers_ = {
        "Content-Type: application/json\r\n",
        "Content-Length: 20\r\n",
        "\r\n"};

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INCORRECT_RESPONSE, result.GetResultCode());
}

//NbRestExecutor::ExecuteRequest(ステータスコード200台)
TEST(NbRestExecutor, ExecuteRequestStatusCode299) {
    NbRestExecutorTest executor;

    //ヘッダ情報上書き
    executor.response_headers_ = {
        "HTTP/1.1 299 OK\r\n",
        "Content-Type: application/json\r\n",
        "Content-Length: 20\r\n",
        "\r\n"};

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOption(executor, request);
}

//NbRestExecutor::ExecuteRequest(ステータスコード300)
TEST(NbRestExecutor, ExecuteRequestStatusCode300) {
    NbRestExecutorTest executor;

    //ヘッダ情報上書き
    executor.response_headers_ = {
        "HTTP/1.1 300 OK\r\n",
        "Content-Type: application/json\r\n",
        "Content-Length: 20\r\n",
        "\r\n"};

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsRestError());
    EXPECT_EQ(300, result.GetRestError().status_code);
    EXPECT_EQ(kBody, result.GetRestError().reason);
    CheckCurlOption(executor, request);
}

//NbRestExecutor::ExecuteRequest(curlpp::LibcurlRuntimeError)
TEST(NbRestExecutor, ExecuteRequestLibcurlRuntimeError) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_RUNTIME;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, result.GetResultCode());
}

//NbRestExecutor::ExecuteRequest(curlpp::LibcurlRuntimeError:レスポンスあり(400 Bad Request))
TEST(NbRestExecutor, ExecuteRequestLibcurlRuntimeError400) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_RUNTIME;
    executor.with_response_ = true;

    //ヘッダ情報上書き
    executor.response_headers_ = {
        "HTTP/1.1 400 Bad Request\r\n",
        "Content-Type: application/json\r\n",
        "Content-Length: 20\r\n",
        "\r\n"};

    executor.response_body_ = (char *)"{\"error\":\"File size over\"}";

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsRestError());
    EXPECT_EQ(400, result.GetRestError().status_code);
    EXPECT_EQ(executor.response_body_, result.GetRestError().reason);
}

//NbRestExecutor::ExecuteRequest(curlpp::LibcurlRuntimeError:レスポンスあり(200 OK))
TEST(NbRestExecutor, ExecuteRequestLibcurlRuntimeError200) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_RUNTIME;
    executor.with_response_ = true;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, result.GetResultCode());
}

//NbRestExecutor::ExecuteRequest(curlpp::LibcurlLogicError)
TEST(NbRestExecutor, ExecuteRequestLibcurlLogicError) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_LOGIC;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_LOGIC, result.GetResultCode());
}

//NbRestExecutor::ExecuteRequest(exception)
TEST(NbRestExecutor, ExecuteRequestException) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_FATAL;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteRequest(request);

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_FATAL, result.GetResultCode());
}

static void CheckCurlOptionUpload(const NbRestExecutorTest &executor, const NbHttpRequest &request, int req_timeout = 60) {
    CheckCurlOptionCommon(executor, request, req_timeout);

    switch (request.GetMethod()) {
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT: {
            curlpp::Options::Upload upload;
            executor.GetOpt(upload);
            EXPECT_TRUE(upload.getValue());

            curlpp::Options::Post post;
            executor.GetOpt(post);
            EXPECT_THROW(post.getValue(), curlpp::UnsetOption);

            curlpp::Options::PostFields post_fields;
            executor.GetOpt(post_fields);
            EXPECT_THROW(post_fields.getValue(), curlpp::UnsetOption);

            curlpp::Options::PostFieldSize post_field_size;
            executor.GetOpt(post_field_size);
            EXPECT_THROW(post_field_size.getValue(), curlpp::UnsetOption);
            break;
        }
        case NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST: {
            curlpp::Options::Post post;
            executor.GetOpt(post);
            EXPECT_TRUE(post.getValue());

            curlpp::Options::Upload upload;
            executor.GetOpt(upload);
            EXPECT_THROW(upload.getValue(), curlpp::UnsetOption);

            curlpp::Options::PostFields post_fields;
            executor.GetOpt(post_fields);
            EXPECT_THROW(post_fields.getValue(), curlpp::UnsetOption);

            curlpp::Options::PostFieldSize post_field_size;
            executor.GetOpt(post_field_size);
            EXPECT_THROW(post_field_size.getValue(), curlpp::UnsetOption);
            break;
        }
    }

    std::list<string> request_headers = request.GetHeaders();
    request_headers.push_back("Content-Length: 20");

    curlpp::Options::HttpHeader headers;
    executor.GetOpt(headers);
    EXPECT_EQ(request_headers, headers.getValue());
}

//NbRestExecutor::ExecuteFileUpload(PUT, Proxyなし、タイムアウトデフォルト)
TEST(NbRestExecutor, ExecuteFileUploadPut) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileUpload(request, MakeFilePath("upload.dat"));

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOptionUpload(executor, request);
}

//NbRestExecutor::ExecuteFileUpload(Post, Proxyあり、タイムアウト10)
TEST(NbRestExecutor, ExecuteFileUploadPost) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, kReqHeaders, kEmpty, kProxy);

    NbResult<NbHttpResponse> result = executor.ExecuteFileUpload(request, MakeFilePath("upload.dat"), 10);

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOptionUpload(executor, request, 10);
}

//NbRestExecutor::ExecuteFileUpload(Get)
TEST(NbRestExecutor, ExecuteFileUploadGet) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileUpload(request, MakeFilePath("upload.dat"));

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_FATAL, result.GetResultCode());
}

//NbRestExecutor::ExecuteFileUpload(ファイルオープンエラー)
TEST(NbRestExecutor, ExecuteFileUploadFileOpenError) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileUpload(request, kEmpty);

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_OPEN_FILE, result.GetResultCode());
}

//NbRestExecutor::ExecuteFileUpload(curlpp::LibcurlRuntimeError)
TEST(NbRestExecutor, ExecuteFileUploadLibcurlRuntimeError) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_RUNTIME;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileUpload(request, MakeFilePath("upload.dat"));

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, result.GetResultCode());
}

//NbRestExecutor::ExecuteFileUpload(curlpp::LibcurlRuntimeError:レスポンスあり(400 Bad Request))
TEST(NbRestExecutor, ExecuteFileUploadLibcurlRuntimeError400) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_RUNTIME;
    executor.with_response_ = true;

    //ヘッダ情報上書き
    executor.response_headers_ = {
        "HTTP/1.1 400 Bad Request\r\n",
        "Content-Type: application/json\r\n",
        "Content-Length: 20\r\n",
        "\r\n"};

    executor.response_body_ = (char *)"{\"error\":\"File size over\"}";

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileUpload(request, MakeFilePath("upload.dat"));

    EXPECT_TRUE(result.IsRestError());
    EXPECT_EQ(400, result.GetRestError().status_code);
    EXPECT_EQ(executor.response_body_, result.GetRestError().reason);
}

//NbRestExecutor::ExecuteFileUpload(curlpp::LibcurlRuntimeError:レスポンスあり(200 OK))
TEST(NbRestExecutor, ExecuteFileUploadLibcurlRuntimeError200) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_RUNTIME;
    executor.with_response_ = true;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileUpload(request, MakeFilePath("upload.dat"));

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, result.GetResultCode());
}

//NbRestExecutor::ExecuteFileUpload(curlpp::LibcurlLogicError)
TEST(NbRestExecutor, ExecuteFileUploadLibcurlLogicError) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_LOGIC;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileUpload(request, MakeFilePath("upload.dat"));

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_LOGIC, result.GetResultCode());
}

//NbRestExecutor::ExecuteFileUpload(exception)
TEST(NbRestExecutor, ExecuteFileUploadException) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_FATAL;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileUpload(request, MakeFilePath("upload.dat"));

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_FATAL, result.GetResultCode());
}

static void CreateDownloadFile() {
    std::ofstream file_stream;
    file_stream.open("download.dat", std::ios::out | std::ios::binary | std::ios::trunc);
    char buffer[] = "abcdefghij1234567890";
    file_stream.write(buffer, 20);
}

class NbRestExecutorDownloadTest : public ::testing::Test {
  protected:
    virtual void SetUp() {
       std::ofstream file_stream;
        file_stream.open("download.dat", std::ios::out | std::ios::binary | std::ios::trunc);
        char buffer[] = "abcdefghij1234567890";
        file_stream.write(buffer, 20);
    }

    virtual void TearDown() {
        std::remove("download.dat");
    }
};

//NbRestExecutor::ExecuteFileDownload(GET, Proxyなし、タイムアウトデフォルト)
TEST_F(NbRestExecutorDownloadTest, ExecuteFileDownload) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileDownload(request, string("download.dat"));

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOption(executor, request);
}

//NbRestExecutor::ExecuteFileDownload(GET, Proxyあり、タイムアウト10)
TEST_F(NbRestExecutorDownloadTest, ExecuteFileDownload2) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kProxy);

    NbResult<NbHttpResponse> result = executor.ExecuteFileDownload(request, string("download.dat"), 10);

    EXPECT_TRUE(result.IsSuccess());
    CheckCurlOption(executor, request, 10);
}

//NbRestExecutor::ExecuteFileDownload(メソッド不正)
TEST_F(NbRestExecutorDownloadTest, ExecuteFileDownloadInvalidMethod) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileDownload(request, string("download.dat"));

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_FATAL, result.GetResultCode());
}

//NbRestExecutor::ExecuteFileDownload(X-Content-Lengthヘッダなし)
TEST_F(NbRestExecutorDownloadTest, ExecuteFileDownloadXContentLengthNone) {
    NbRestExecutorTest executor;

    //ヘッダ情報上書き
    executor.response_headers_ = {
        "HTTP/1.1 200 OK\r\n",
        "Content-Type: application/json\r\n",
        "Content-Length: 20\r\n",
        "\r\n"};

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileDownload(request, string("download.dat"));

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_FILE_DOWNLOAD, result.GetResultCode());
}

//NbRestExecutor::ExecuteFileDownload(X-Content-Lengthファイルサイズ不正)
TEST_F(NbRestExecutorDownloadTest, ExecuteFileDownloadLengthError) {
    NbRestExecutorTest executor;

    //ヘッダ情報上書き
    executor.response_headers_ = {
        "HTTP/1.1 200 OK\r\n",
        "Content-Type: application/json\r\n",
        "X-Content-Length: 30\r\n",
        "\r\n"};

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileDownload(request, string("download.dat"));

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_FILE_DOWNLOAD, result.GetResultCode());
}

//NbRestExecutor::ExecuteFileDownload(ファイルオープンエラー)
TEST_F(NbRestExecutorDownloadTest, ExecuteFileDownloadFileOpenError) {
    NbRestExecutorTest executor;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileDownload(request, kEmpty);

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_OPEN_FILE, result.GetResultCode());
}

//NbRestExecutor::ExecuteFileDownload(curlpp::LibcurlRuntimeError)
TEST_F(NbRestExecutorDownloadTest, ExecuteFileDownloadLibcurlRuntimeError) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_RUNTIME;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileDownload(request, string("download.dat"));

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, result.GetResultCode());
}

//NbRestExecutor::ExecuteFileDownload(curlpp::LibcurlRuntimeError:レスポンスあり(400 Bad Request))
TEST_F(NbRestExecutorDownloadTest, ExecuteFileDownloadLibcurlRuntimeError400) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_RUNTIME;
    executor.with_response_ = true;

    //ヘッダ情報上書き
    executor.response_headers_ = {
        "HTTP/1.1 400 Bad Request\r\n",
        "Content-Type: application/json\r\n",
        "Content-Length: 20\r\n",
        "\r\n"};

    executor.response_body_ = (char *)"{\"error\":\"File size over\"}";

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileDownload(request, string("download.dat"));

    EXPECT_TRUE(result.IsRestError());
    EXPECT_EQ(400, result.GetRestError().status_code);
    EXPECT_EQ(executor.response_body_, result.GetRestError().reason);
}

//NbRestExecutor::ExecuteFileDownload(curlpp::LibcurlRuntimeError:レスポンスあり(200 OK))
TEST_F(NbRestExecutorDownloadTest, ExecuteFileDownloadLibcurlRuntimeError200) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_RUNTIME;
    executor.with_response_ = true;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileDownload(request, string("download.dat"));

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_RUNTIME, result.GetResultCode());
}

//NbRestExecutor::ExecuteFileDownload(curlpp::LibcurlLogicError)
TEST_F(NbRestExecutorDownloadTest, ExecuteFileDownloadLibcurlLogicError) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_LOGIC;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileDownload(request, string("download.dat"));

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_LOGIC, result.GetResultCode());
}

//NbRestExecutor::ExecuteFileDownload(exception)
TEST_F(NbRestExecutorDownloadTest, ExecuteFileDownloadException) {
    NbRestExecutorTest executor;
    executor.error_trigger_ = NbResultCode::NB_ERROR_CURL_FATAL;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, kReqHeaders, kEmpty, kEmpty);

    NbResult<NbHttpResponse> result = executor.ExecuteFileDownload(request, string("download.dat"));

    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CURL_FATAL, result.GetResultCode());
}
} //namespace necbaas
