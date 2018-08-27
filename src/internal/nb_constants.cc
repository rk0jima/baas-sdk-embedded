/*
 * Copyright (C) 2017 NEC Corporation
 */

#include "necbaas/internal/nb_constants.h"

namespace necbaas {

using std::string;

//
// HTTP関連
//
const int kHttpConnectionMax = 20;
const int kRestTimeoutDefault = 60;

//
// URI パス定義
//
const string kPathApiVersion = "1";
const string kLoginUrl = "/login";
const string kObjectBucketPath = "/buckets/object";
const string kObjectsPath = "/objects";
const string kFileBucketPath = "/buckets/file";
const string kFilesPath = "/files";
const string kMetaPath = "/meta";
const string kApigwUrl = "/api";

//
// HTTPヘッダ
//
const string kHeaderAppId = "X-Application-Id";
const string kHeaderAppKey = "X-Application-Key";
const string kHeaderContentType = "Content-Type";
const string kHeaderContentTypeJson = "application/json";
const string kHeaderSessionToken = "X-Session-Token";
const string kHeaderUserAgent = "User-Agent";
const string kHeaderUserAgentDefault = "baas embedded sdk";
const string kHeaderContentLength = "Content-Length";
const string kHeaderXContentLength = "X-Content-Length";
const string kHeaderXAcl = "X-ACL";
const string kHeaderHost = "Host";

//
// Key
//
// common
const string kKeyId = "_id";
const string kKeyCreatedAt = "createdAt";
const string kKeyUpdatedAt = "updatedAt";
const string kKeyAcl = "ACL";
const string kKeyContentAcl = "contentACL";
const string kKeyETag = "etag";
const string kKeyResults = "results";
const string kKeyReasonCode = "reasonCode";
const string kKeyDetail = "detail";

// user / group
const string kKeyUsername = "username";
const string kKeyPassword = "password";
const string kKeyEmail = "email";
const string kKeySessionToken = "sessionToken";
const string kKeyExpire = "expire";
const string kKeyGroups = "groups";
const string kKeyUsers = "users";
const string kKeyOptions = "options";

// bucket
const string kKeyName = "name";
const string kKeyDescription = "description";

// file
const string kKeyFilename = "filename";
const string kKeyContentType = "contentType";
const string kKeyLength = "length";
const string kKeyPublished = "published";
const string kKeyDeleted = "_deleted";
const string kKeyMetaETag = "metaETag";
const string kKeyFileETag = "fileETag";
const string kKeyCacheDisabled = "cacheDisabled";
const string kKeyDeleteMark = "deleteMark";

// object
const string kKeyWhere = "where";
const string kKeyOrder = "order";
const string kKeySkip = "skip";
const string kKeyLimit = "limit";
const string kKeyCount = "count";
const string kKeyProjection = "projection";
const string kKeyReadPreference = "readPreference";
const string kKeyTimeout = "timeout";

// ACL
const std::string kKeyOwner = "owner";
const std::string kKeyRead = "r";
const std::string kKeyWrite = "w";
const std::string kKeyCreate = "c";
const std::string kKeyUpdate = "u";
const std::string kKeyDelete = "d";
const std::string kKeyAdmin = "admin";
const std::string kGroupAnonymous = "g:anonymous";
const std::string kGroupAuthenticated = "g:authenticated";
}  // namespace necbaas
