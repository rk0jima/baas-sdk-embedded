#include "gtest/gtest.h"
#include "ft_util.h"
#include "necbaas/nb_file_bucket.h"

namespace necbaas {
namespace FTCloudfn {

using std::string;
using std::shared_ptr;

static const string api_json{R"({
"swagger": "2.0",
"basePath": "/embedded",
"produces": ["application/json"],
"paths": {
  "/echoBack": {
    "get": {
      "operationId": "echoBack",
      "summary": "Echo back",
      "description": "Echo back" },
    "put": {
      "operationId": "echoBack",
      "summary": "Echo back",
      "description": "Echo back" },
    "post": {
      "operationId": "echoBack",
      "summary": "Echo back",
      "description": "Echo back" },
    "delete": {
      "operationId": "echoBack",
      "summary": "Echo back",
      "description": "Echo back" }},
  "/": {
    "get": {
      "operationId": "echoBack",
      "summary": "Echo back",
      "description": "Echo back" },
    "put": {
      "operationId": "echoBack",
      "summary": "Echo back",
      "description": "Echo back" },
    "post": {
      "operationId": "echoBack",
      "summary": "Echo back",
      "description": "Echo back" },
    "delete": {
      "operationId": "echoBack",
      "summary": "Echo back",
      "description": "Echo back" }},
  "/timeout": {
    "get": {
      "operationId": "timeout",
      "summary": "Echo back",
      "description": "Echo back" }},
  "/wait70": {
    "get": {
      "operationId": "wait70",
      "summary": "Echo back",
      "description": "Echo back" }},
  "/wait40": {
    "get": {
      "operationId": "wait40",
      "summary": "Echo back",
      "description": "Echo back" }},
  "/success299": {
    "get": {
      "operationId": "success299",
      "summary": "Echo back",
      "description": "Echo back" }},
  "/error555": {
    "get": {
      "operationId": "error555",
      "summary": "Echo back",
      "description": "Echo back" }}}
})"};

static const string function_json{R"({
"echoBack": {
  "code": {
    "bucket": "CUSTOM_CODE",
    "file": "embeddedSdkTest-1.0.0.tgz" },
  "handler": "echoBack",
  "env": {
    "spec": "node6",
    "timeout": 600,
    "memorySize": 256 }
},
"timeout": {
  "code": {
    "bucket": "CUSTOM_CODE",
    "file": "embeddedSdkTest-1.0.0.tgz" },
  "handler": "timeout",
  "env": {
    "spec": "node6",
    "timeout": 1,
    "memorySize": 256 }
},
"wait70": {
  "code": {
    "bucket": "CUSTOM_CODE",
    "file": "embeddedSdkTest-1.0.0.tgz" },
  "handler": "wait70",
  "env": {
    "spec": "node6",
    "timeout": 80,
    "memorySize": 256 }
},
"wait40": {
  "code": {
    "bucket": "CUSTOM_CODE",
    "file": "embeddedSdkTest-1.0.0.tgz" },
  "handler": "wait40",
  "env": {
    "spec": "node6",
    "timeout": 80,
    "memorySize": 256 }
},
"success299": {
  "code": {
    "bucket": "CUSTOM_CODE",
    "file": "embeddedSdkTest-1.0.0.tgz" },
  "handler": "success299",
  "env": {
    "spec": "node6",
    "timeout": 600,
    "memorySize": 256 }
},
"error555": {
  "code": {
    "bucket": "CUSTOM_CODE",
    "file": "embeddedSdkTest-1.0.0.tgz" },
  "handler": "error555",
  "env": {
    "spec": "node6",
    "timeout": 600,
    "memorySize": 256 }
}
})"};

void RegisterCloudfnDefault(shared_ptr<NbService> service) {
    NbResult<NbHttpResponse> rest_result = service->ExecuteRequest(
        [](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Put("/apigw/apis")
                          .AppendPath(string("/") + string("embedded"))
                          .AppendHeader("Content-Type", "application/json")
                          .Body(api_json)
                          .Build();
        }, 60);
    EXPECT_TRUE(rest_result.IsSuccess());

    rest_result = service->ExecuteRequest(
        [](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Put("/functions")
                          .AppendHeader("Content-Type", "application/json")
                          .Body(function_json)
                          .Build();
        }, 60);
    EXPECT_TRUE(rest_result.IsSuccess());
   
    NbFileBucket file_bucket(service, "CUSTOM_CODE");
    file_bucket.DeleteFile("embeddedSdkTest-1.0.0.tgz", "", "");
    NbResult<NbFileMetadata> file_result = file_bucket.UploadNewFile("embeddedSdkTest-1.0.0.tgz", FTUtil::MakeFilePath("embeddedSdkTest-1.0.0.tgz"), "application/gzip");
    EXPECT_TRUE(file_result.IsSuccess());
}

void DeleteCloudfnDefault(shared_ptr<NbService> service) {
    NbResult<NbHttpResponse> rest_result = service->ExecuteRequest(
        [](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Delete("/apigw/apis")
                          .Build();
        }, 60);
    EXPECT_TRUE(rest_result.IsSuccess());

    rest_result = service->ExecuteRequest(
        [](NbHttpRequestFactory &factory) -> NbHttpRequest {
            return factory.Delete("/functions")
                          .Build();
        }, 60);
    EXPECT_TRUE(rest_result.IsSuccess());

    NbFileBucket file_bucket(service, "CUSTOM_CODE");
    file_bucket.DeleteFile("embeddedSdkTest-1.0.0.tgz", "", "");
}

}
}
