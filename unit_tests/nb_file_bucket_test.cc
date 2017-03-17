#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "necbaas/nb_file_bucket.h"
#include "necbaas/internal/nb_utility.h"
#include "rest_api_mock.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

static const string kEmpty{""};

static const string kBucketName{"bucketName"};
static const string kFileName{"fileName"};
static const string kFilePath{"/dir/filePath"};
static const string kContentType{"text/plain"};

static const string kFileMetadata{R"({
    "_id": "533d31c43cbc3dd2d0b32cd1",
    "filename": "fileName",
    "contentType": "text/plain",
    "length": 12,
    "ACL": {
        "owner": "514af36644f9cb2eb8000002",
         "r": [ "g:authenticated" ],
         "w": [],
         "u": [],
         "d": [],
         "admin": [ "514af36644f9cb2eb8000003" ]
    },
    "createdAt": "2013-08-27T04:37:30.000Z",
    "updatedAt": "2013-08-27T04:37:30.000Z",
    "metaETag": "8c92c97e-01a7-11e4-9598-53792c688d1b",
    "fileETag": "8c92c97e-01a7-11e4-9598-53792c688d1c",
    "cacheDisabled": false
})"};

static const string kFileList{R"({
    "currentTime": "2014-11-18T07:34:22:242Z",
    "results": [
        {
            "_id": "533d31c43cbc3dd2d0b32cd1",
            "filename": "hello.txt",
            "contentType": "text/plain",
            "length": 12,
            "ACL": { "owner": "514af36644f9cb2eb8000002","r":["g:authenticated"],"w":[],"u":[],"d":[],"admin":["514af36644f9cb2eb8000003"]},
            "createdAt": "2013-08-27T04:37:30.000Z",
            "updatedAt": "2013-08-27T04:37:30.000Z",
            "metaETag": "8c92c97e-01a7-11e4-9598-53792c688d1b",
            "fileETag": "8c92c97e-01a7-11e4-9598-53792c688d1c",
            "cacheDisabled": false,
            "publicUrl": "http://publicurl.com/sample1"
        },
        {
            "_id": "533d31c43cbc3dd2d0b32cd2",
            "filename": "hello2.txt",
            "contentType": "text/plain",
            "length": 12,
            "ACL": { "owner": "514af36644f9cb2eb8000002","r":["g:authenticated"],"w":[],"u":[],"d":[],"admin":["514af36644f9cb2eb8000003"]},
            "createdAt": "2013-08-27T04:37:30.000Z",
            "updatedAt": "2013-08-27T04:37:30.000Z",
            "metaETag": "8c92c97e-01a7-11e4-9598-53792c688d1b",
            "fileETag": "8c92c97e-01a7-11e4-9598-53792c688d1c",
            "cacheDisabled": false,
            "publicUrl": "http://publicurl.com/sample2"
        }
    ]
})"};


// テスト名を変更するため、フィクスチャを継承
class NbFileBucketTest : public RestApiTest {};

static NbResult<NbHttpResponse> DownloadFileCommon(const NbHttpRequest &request, const string &file_path, int timeout) {
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    std::multimap<std::string, std::string> resp_headers{{"X-Content-Length", "12345"}};
    NbHttpResponse response(200, string("OK"), resp_headers, vector<char>());
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

static NbResult<NbHttpResponse> DownloadFileNorm(const NbHttpRequest &request, const string &file_path, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "/" + kFileName), request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(60, timeout);
    return DownloadFileCommon(request, file_path, timeout);
}

//NbFileBucketTest::DownloadFile(正常)
TEST_F(NbFileBucketTest, DownloadFile) {
    SetExpectDownload(&executor_, kFilePath, &DownloadFileNorm);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<int> result = file_bucket.DownloadFile(kFileName, kFilePath);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    int response = result.GetSuccessData();
    EXPECT_EQ(12345, response);
}

static NbResult<NbHttpResponse> DownloadFileEscape(const NbHttpRequest &request, const string &file_path, int timeout) {
    string file_name = "%E3%83%86%E3%82%B9%E3%83%88%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB";
    EXPECT_EQ(10, timeout);
    EXPECT_EQ(string("/files/" + kBucketName + "/" + file_name), request.GetUrl().substr(request.GetUrl().find("/files/")));
    return DownloadFileCommon(request, file_path, timeout);
}

//NbFileBucketTest::DownloadFile(ファイル名URLエンコード)
TEST_F(NbFileBucketTest, DownloadFileEscape) {
    SetExpectDownload(&executor_, kFilePath, &DownloadFileEscape);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    file_bucket.SetTimeout(10);
    EXPECT_EQ(10, file_bucket.GetTimeout());
    NbResult<int> result = file_bucket.DownloadFile(string("テストファイル"), kFilePath);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    int response = result.GetSuccessData();
    EXPECT_EQ(12345, response);
}

static NbResult<NbHttpResponse> DownloadFileRestError(const NbHttpRequest &request, const string &file_path, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "/" + kFileName), request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_ERROR_RESPONSE);
    string body_str{"Http Response Error"};

    NbRestError response{404, body_str};
    tmp_result.SetRestError(response);

    return tmp_result;
}

//NbFileBucketTest::DownloadFile(RESTエラー)
TEST_F(NbFileBucketTest, DownloadFileRestError) {
    SetExpectDownload(&executor_, kFilePath, &DownloadFileRestError);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<int> result = file_bucket.DownloadFile(kFileName, kFilePath);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_EQ(string("Http Response Error"), result.GetRestError().reason);

    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

static NbResult<NbHttpResponse> DownloadFileFatal(const NbHttpRequest &request, const string &file_path, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "/" + kFileName), request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);

    return tmp_result;
}

//NbFileBucketTest::DownloadFile(エラー)
TEST_F(NbFileBucketTest, DownloadFileFatal) {
    SetExpectDownload(&executor_, kFilePath, &DownloadFileFatal);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<int> result = file_bucket.DownloadFile(kFileName, kFilePath);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
}

//NbFileBucketTest::DownloadFile(ファイル名空)
TEST_F(NbFileBucketTest, DownloadFileNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<int> result = file_bucket.DownloadFile(kEmpty, kFilePath);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

//NbFileBucketTest::DownloadFile(ファイルパス空)
TEST_F(NbFileBucketTest, DownloadFilePathEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<int> result = file_bucket.DownloadFile(kFileName, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

//NbFileBucketTest::DownloadFile(バケット名空)
TEST_F(NbFileBucketTest, DownloadFileBucketNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kEmpty);
    NbResult<int> result = file_bucket.DownloadFile(kFileName, kFilePath);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

//NbFileBucketTest::DownloadFile(request_factory構築失敗)
TEST_F(NbFileBucketTest, DownloadFileRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<int> result = file_bucket.DownloadFile(kFileName, kFilePath);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
}

//NbFileBucketTest::DownloadFile(接続数オーバー)
TEST_F(NbFileBucketTest, DownloadFileConnectionOver) {
    SetExpectDownload(nullptr, kFilePath, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<int> result = file_bucket.DownloadFile(kFileName, kFilePath);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
}

static NbResult<NbHttpResponse> UploadNewFileCommon(const NbHttpRequest &request, const string &file_path, int timeout) {
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(kEmpty, request.GetBody());

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    std::vector<char> body(kFileMetadata.length());
    std::copy(kFileMetadata.c_str(), kFileMetadata.c_str() + kFileMetadata.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

static NbResult<NbHttpResponse> UploadNewFileNorm1(const NbHttpRequest &request, const string &file_path, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "/" + kFileName), request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(4, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("Content-Type:") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(1, check_header);
    EXPECT_EQ(60, timeout);
    return UploadNewFileCommon(request, file_path, timeout);
}

//NbFileBucketTest::UploadNewFile(ACLなし、cache_disable：なし)
TEST_F(NbFileBucketTest, UploadNewFileNorm1) {
    SetExpectUpload(&executor_, kFilePath, &UploadNewFileNorm1);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, kFilePath, kContentType);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbFileMetadata meta = result.GetSuccessData();
    EXPECT_EQ(kFileName, meta.GetFileName());
}

static NbResult<NbHttpResponse> UploadNewFileNorm2(const NbHttpRequest &request, const string &file_path, int timeout) {
    string file_name = "%E3%83%86%E3%82%B9%E3%83%88%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB";
    EXPECT_EQ(string("/files/" + kBucketName + "/" + file_name + "?cacheDisabled=true"), request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(5, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("Content-Type:") != std::string::npos) {
            ++check_header;
            continue;
        }
        if (header.find("X-ACL:") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(2, check_header);
    EXPECT_EQ(10, timeout);
    return UploadNewFileCommon(request, file_path, timeout);
}

//NbFileBucketTest::UploadNewFile(ACLあり、cache_disable：True, ファイル名URLエンコード)
TEST_F(NbFileBucketTest, UploadNewFileNorm2) {
    SetExpectUpload(&executor_, kFilePath, &UploadNewFileNorm2);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    file_bucket.SetTimeout(10);
    EXPECT_EQ(10, file_bucket.GetTimeout());
    NbAcl acl = NbAcl::CreateAclForAnonymous();
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile("テストファイル", kFilePath, kContentType, acl, true);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbFileMetadata meta = result.GetSuccessData();
    EXPECT_EQ(kFileName, meta.GetFileName());
}

static NbResult<NbHttpResponse> UploadNewFileRestError(const NbHttpRequest &request, const string &file_path, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "/" + kFileName), request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_ERROR_RESPONSE);
    string body_str{"Http Response Error"};

    NbRestError response{404, body_str};
    tmp_result.SetRestError(response);

    return tmp_result;
}

//NbFileBucketTest::UploadNewFile(RESTエラー)
TEST_F(NbFileBucketTest, UploadNewFileRestError) {
    SetExpectUpload(&executor_, kFilePath, &UploadNewFileRestError);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, kFilePath, kContentType);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_EQ(string("Http Response Error"), result.GetRestError().reason);

    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

static NbResult<NbHttpResponse> UploadNewFileFatal(const NbHttpRequest &request, const string &file_path, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "/" + kFileName), request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_POST, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);

    return tmp_result;
}

//NbFileBucketTest::UploadNewFile(エラー)
TEST_F(NbFileBucketTest, UploadNewFileFatal) {
    SetExpectUpload(&executor_, kFilePath, &UploadNewFileFatal);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, kFilePath, kContentType);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
}

//NbFileBucketTest::UploadNewFile(ファイル名空)
TEST_F(NbFileBucketTest, UploadNewFileNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kEmpty, kFilePath, kContentType);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

//NbFileBucketTest::UploadNewFile(ファイルパス空)
TEST_F(NbFileBucketTest, UploadNewFilePathEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, kEmpty, kContentType);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

//NbFileBucketTest::UploadNewFile(Conten-Type空)
TEST_F(NbFileBucketTest, UploadNewFileContentTypeEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, kFilePath, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

//NbFileBucketTest::UploadNewFile(バケット名空)
TEST_F(NbFileBucketTest, UploadNewFileBucketNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kEmpty);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, kFilePath, kContentType);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

//NbFileBucketTest::UploadNewFile(request_factory構築失敗)
TEST_F(NbFileBucketTest, UploadNewFileRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, kFilePath, kContentType);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
}

//NbFileBucketTest::UploadNewFile(接続数オーバー)
TEST_F(NbFileBucketTest, UploadNewFileConnectionOver) {
    SetExpectUpload(nullptr, kFilePath, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadNewFile(kFileName, kFilePath, kContentType);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
}

static NbResult<NbHttpResponse> UploadUpdateFileCommon(const NbHttpRequest &request, const string &file_path, int timeout) {
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    EXPECT_EQ(kEmpty, request.GetBody());

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    std::vector<char> body(kFileMetadata.length());
    std::copy(kFileMetadata.c_str(), kFileMetadata.c_str() + kFileMetadata.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

static NbResult<NbHttpResponse> UploadUpdateFileNorm1(const NbHttpRequest &request, const string &file_path, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "/" + kFileName + "?fileETag=8c92c97e-01a7-11e4-9598-53792c688d1c&metaETag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
              request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(4, request.GetHeaders().size());
    int check_header = 0;
    for (auto header : request.GetHeaders()) {
        if (header.find("Content-Type:") != std::string::npos) {
            ++check_header;
            continue;
        }
    }
    EXPECT_EQ(1, check_header);
    EXPECT_EQ(60, timeout);
    return UploadUpdateFileCommon(request, file_path, timeout);
}

//NbFileBucketTest::UploadUpdateFile(metadata指定)
TEST_F(NbFileBucketTest, UploadUpdateFileNorm1) {
    SetExpectUpload(&executor_, kFilePath, &UploadUpdateFileNorm1);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbFileMetadata in_meta(kBucketName, NbJsonObject(kFileMetadata));
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFilePath, in_meta);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbFileMetadata meta = result.GetSuccessData();
    EXPECT_EQ(kFileName, meta.GetFileName());
}

static NbResult<NbHttpResponse> UploadUpdateFileNorm2(const NbHttpRequest &request, const string &file_path, int timeout) {
    string file_name = "%E3%83%86%E3%82%B9%E3%83%88%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB";
    EXPECT_EQ(string("/files/" + kBucketName + "/" + file_name),
              request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(10, timeout);
    return UploadUpdateFileCommon(request, file_path, timeout);
}

//NbFileBucketTest::UploadUpdateFile(Content-Type, Etag空)
TEST_F(NbFileBucketTest, UploadUpdateFileNorm2) {
    SetExpectUpload(&executor_, kFilePath, &UploadUpdateFileNorm2);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    file_bucket.SetTimeout(10);
    EXPECT_EQ(10, file_bucket.GetTimeout());
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile("テストファイル", kFilePath, kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbFileMetadata meta = result.GetSuccessData();
    EXPECT_EQ(kFileName, meta.GetFileName());
}

static NbResult<NbHttpResponse> UploadUpdateFileRestError(const NbHttpRequest &request, const string &file_path, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "/" + kFileName + "?fileETag=8c92c97e-01a7-11e4-9598-53792c688d1c&metaETag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
              request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_ERROR_RESPONSE);
    string body_str{"Http Response Error"};

    NbRestError response{404, body_str};
    tmp_result.SetRestError(response);

    return tmp_result;
}

//NbFileBucketTest::UploadUpdateFile(RESTエラー)
TEST_F(NbFileBucketTest, UploadUpdateFileRestError) {
    SetExpectUpload(&executor_, kFilePath, &UploadUpdateFileRestError);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbFileMetadata in_meta(kBucketName, NbJsonObject(kFileMetadata));
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFilePath, in_meta);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_EQ(string("Http Response Error"), result.GetRestError().reason);

    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

static NbResult<NbHttpResponse> UploadUpdateFileFatal(const NbHttpRequest &request, const string &file_path, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "/" + kFileName + "?fileETag=8c92c97e-01a7-11e4-9598-53792c688d1c&metaETag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
              request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_PUT, request.GetMethod());
    EXPECT_EQ(4, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);

    return tmp_result;
}

//NbFileBucketTest::UploadUpdateFile(エラー)
TEST_F(NbFileBucketTest, UploadUpdateFileFatal) {
    SetExpectUpload(&executor_, kFilePath, &UploadUpdateFileFatal);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbFileMetadata in_meta(kBucketName, NbJsonObject(kFileMetadata));
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFilePath, in_meta);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
}

//NbFileBucketTest::UploadUpdateFile(ファイル名空)
TEST_F(NbFileBucketTest, UploadUpdateFileNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kEmpty, kFilePath, kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

//NbFileBucketTest::UploadUpdateFile(ファイルパス空)
TEST_F(NbFileBucketTest, UploadUpdateFilePathEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, kEmpty, kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

//NbFileBucketTest::UploadUpdateFile(バケット名空)
TEST_F(NbFileBucketTest, UploadUpdateFileBucketNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kEmpty);
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, kFilePath, kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

//NbFileBucketTest::UploadUpdateFile(request_factory構築失敗)
TEST_F(NbFileBucketTest, UploadUpdateFileRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, kFilePath, kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
}

//NbFileBucketTest::UploadUpdateFile(接続数オーバー)
TEST_F(NbFileBucketTest, UploadUpdateFileConnectionOver) {
    SetExpectUpload(nullptr, kFilePath, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbFileMetadata> result = file_bucket.UploadUpdateFile(kFileName, kFilePath, kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
}

static NbResult<NbHttpResponse> DeleteFileCommon(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    std::vector<char> body{'{','}'};
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

static NbResult<NbHttpResponse> DeleteFileNorm1(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "/" + kFileName + "?deleteMark=1&fileETag=8c92c97e-01a7-11e4-9598-53792c688d1c&metaETag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
              request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(60, timeout);
    return DeleteFileCommon(request, timeout);
}

//NbFileBucketTest::DeleteFile(metadata指定)
TEST_F(NbFileBucketTest, DeleteFileNorm1) {
    SetExpect(&executor_, &DeleteFileNorm1);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbFileMetadata in_meta(kBucketName, NbJsonObject(kFileMetadata));
    NbResult<NbJsonObject> result = file_bucket.DeleteFile(in_meta, true);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbJsonObject response_json = result.GetSuccessData();
    EXPECT_TRUE(response_json.IsEmpty());
}

static NbResult<NbHttpResponse> DeleteFileNorm2(const NbHttpRequest &request, int timeout) {
    string file_name = "%E3%83%86%E3%82%B9%E3%83%88%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB";
    EXPECT_EQ(string("/files/" + kBucketName + "/" + file_name),
              request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(10, timeout);
    return DeleteFileCommon(request, timeout);
}

//NbFileBucketTest::DeleteFile(Content-Type, Etag空)
TEST_F(NbFileBucketTest, DeleteFileNorm2) {
    SetExpect(&executor_, &DeleteFileNorm2);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    file_bucket.SetTimeout(10);
    EXPECT_EQ(10, file_bucket.GetTimeout());
    NbResult<NbJsonObject> result = file_bucket.DeleteFile(string("テストファイル"), kEmpty, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    NbJsonObject response_json = result.GetSuccessData();
    EXPECT_TRUE(response_json.IsEmpty());
}

static NbResult<NbHttpResponse> DeleteFileRestError(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "/" + kFileName + "?deleteMark=1&fileETag=8c92c97e-01a7-11e4-9598-53792c688d1c&metaETag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
              request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_ERROR_RESPONSE);
    string body_str{"Http Response Error"};

    NbRestError response{404, body_str};
    tmp_result.SetRestError(response);

    return tmp_result;
}

//NbFileBucketTest::DeleteFile(RESTエラー)
TEST_F(NbFileBucketTest, DeleteFileRestError) {
    SetExpect(&executor_, &DeleteFileRestError);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbFileMetadata in_meta(kBucketName, NbJsonObject(kFileMetadata));
    NbResult<NbJsonObject> result = file_bucket.DeleteFile(in_meta, true);

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_EQ(string("Http Response Error"), result.GetRestError().reason);

    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

static NbResult<NbHttpResponse> DeleteFileFatal(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "/" + kFileName + "?deleteMark=1&fileETag=8c92c97e-01a7-11e4-9598-53792c688d1c&metaETag=8c92c97e-01a7-11e4-9598-53792c688d1b"),
              request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_DELETE, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);

    return tmp_result;
}

//NbFileBucketTest::DeleteFile(エラー)
TEST_F(NbFileBucketTest, DeleteFileFatal) {
    SetExpect(&executor_, &DeleteFileFatal);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbFileMetadata in_meta(kBucketName, NbJsonObject(kFileMetadata));
    NbResult<NbJsonObject> result = file_bucket.DeleteFile(in_meta, true);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
}

//NbFileBucketTest::DeleteFile(ファイル名空)
TEST_F(NbFileBucketTest, DeleteFileNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbJsonObject> result = file_bucket.DeleteFile(kEmpty, kEmpty, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_INVALID_ARGUMENT, result.GetResultCode());
}

//NbFileBucketTest::DeleteFile(バケット名空)
TEST_F(NbFileBucketTest, DeleteFileBucketNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kEmpty);
    NbResult<NbJsonObject> result = file_bucket.DeleteFile(kFileName, kEmpty, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

//NbFileBucketTest::DeleteFile(request_factory構築失敗)
TEST_F(NbFileBucketTest, DeleteFileRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbJsonObject> result = file_bucket.DeleteFile(kFileName, kEmpty, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
}

//NbFileBucketTest::DeleteFile(接続数オーバー)
TEST_F(NbFileBucketTest, DeleteFileConnectionOver) {
    SetExpect(nullptr, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<NbJsonObject> result = file_bucket.DeleteFile(kFileName, kEmpty, kEmpty);

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
}

static NbResult<NbHttpResponse> GetFilesCommon(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_OK);
    std::vector<char> body(kFileList.length());
    std::copy(kFileList.c_str(), kFileList.c_str() + kFileList.length(), body.begin());
    NbHttpResponse response(200, string("OK"), std::multimap<std::string, std::string>(), body);
    tmp_result.SetSuccessData(response);

    return tmp_result;
}

static NbResult<NbHttpResponse> GetFilesNorm1(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName),
              request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(60, timeout);
    return GetFilesCommon(request, timeout);
}

//NbFileBucketTest::GetFiles(パラメータなし)
TEST_F(NbFileBucketTest, GetFilesNorm1) {
    SetExpect(&executor_, &GetFilesNorm1);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<vector<NbFileMetadata>> result = file_bucket.GetFiles();

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    vector<NbFileMetadata> meta_list = result.GetSuccessData();
    EXPECT_EQ(2, meta_list.size());
    EXPECT_EQ("hello.txt", meta_list[0].GetFileName());
    EXPECT_EQ("hello2.txt", meta_list[1].GetFileName());
}

static NbResult<NbHttpResponse> GetFilesNorm2(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName + "?deleteMark=1&published=1"),
              request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(10, timeout);
    return GetFilesCommon(request, timeout);
}

//NbFileBucketTest::GetFiles(パラメータあり
TEST_F(NbFileBucketTest, GetFilesNorm2) {
    SetExpect(&executor_, &GetFilesNorm2);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    file_bucket.SetTimeout(10);
    EXPECT_EQ(10, file_bucket.GetTimeout());
    NbResult<vector<NbFileMetadata>> result = file_bucket.GetFiles(true, true);

    // 戻り値確認
    EXPECT_TRUE(result.IsSuccess());
    vector<NbFileMetadata> meta_list = result.GetSuccessData();
    EXPECT_EQ(2, meta_list.size());
    EXPECT_EQ("hello.txt", meta_list[0].GetFileName());
    EXPECT_EQ("hello2.txt", meta_list[1].GetFileName());
}

static NbResult<NbHttpResponse> GetFilesRestError(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName),
              request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_ERROR_RESPONSE);
    string body_str{"Http Response Error"};

    NbRestError response{404, body_str};
    tmp_result.SetRestError(response);

    return tmp_result;
}

//NbFileBucketTest::GetFiles(RESTエラー)
TEST_F(NbFileBucketTest, GetFilesRestError) {
    SetExpect(&executor_, &GetFilesRestError);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<vector<NbFileMetadata>> result = file_bucket.GetFiles();

    // 戻り値確認
    EXPECT_TRUE(result.IsRestError());
    NbRestError rest_error = result.GetRestError();
    EXPECT_EQ(404, result.GetRestError().status_code);
    EXPECT_EQ(string("Http Response Error"), result.GetRestError().reason);

    EXPECT_FALSE(NbUser::IsLoggedIn(service));
}

static NbResult<NbHttpResponse> GetFilesFatal(const NbHttpRequest &request, int timeout) {
    EXPECT_EQ(string("/files/" + kBucketName),
              request.GetUrl().substr(request.GetUrl().find("/files/")));
    EXPECT_EQ(NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, request.GetMethod());
    EXPECT_EQ(3, request.GetHeaders().size());
    EXPECT_EQ(kEmpty, request.GetBody());
    EXPECT_EQ(60, timeout);

    NbResult<NbHttpResponse> tmp_result(NbResultCode::NB_FATAL);

    return tmp_result;
}

//NbFileBucketTest::GetFiles(エラー)
TEST_F(NbFileBucketTest, GetFilesFatal) {
    SetExpect(&executor_, &GetFilesFatal);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<vector<NbFileMetadata>> result = file_bucket.GetFiles();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
}

//NbFileBucketTest::GetFiles(バケット名空)
TEST_F(NbFileBucketTest, GetFilesBucketNameEmpty) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kEmpty);
    NbResult<vector<NbFileMetadata>> result = file_bucket.GetFiles();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_BUCKET_NAME, result.GetResultCode());
}

//NbFileBucketTest::GetFiles(request_factory構築失敗)
TEST_F(NbFileBucketTest, GetFilesRequestFactoryFail) {
    shared_ptr<NbService> service = NbService::CreateService(kEmpty, kTenantId, kAppId, kAppKey, kProxy);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<vector<NbFileMetadata>> result = file_bucket.GetFiles();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_ENDPOINT_URL, result.GetResultCode());
}

//NbFileBucketTest::GetFiles(接続数オーバー)
TEST_F(NbFileBucketTest, GetFilesConnectionOver) {
    SetExpect(nullptr, nullptr);

    shared_ptr<NbService> service(mock_service_);

    NbFileBucket file_bucket(service, kBucketName);
    NbResult<vector<NbFileMetadata>> result = file_bucket.GetFiles();

    // 戻り値確認
    EXPECT_TRUE(result.IsFatalError());
    EXPECT_EQ(NbResultCode::NB_ERROR_CONNECTION_OVER, result.GetResultCode());
}

} //namespace necbaas
