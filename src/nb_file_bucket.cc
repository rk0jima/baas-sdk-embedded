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

#include "necbaas/nb_file_bucket.h"
#include <curlpp/cURLpp.hpp>
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;

// コンストラクタ
NbFileBucket::NbFileBucket(const shared_ptr<NbService> &service, const string &bucket_name)
    : service_(service), bucket_name_(bucket_name) {}

// デストラクタ.
NbFileBucket::~NbFileBucket() {}

NbResult<int> NbFileBucket::DownloadFile(const string &file_name, const string &file_path) {
    NBLOG(TRACE) << __func__;

    NbResult<int> result;

    if (file_name.empty() || file_path.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_INVALID_ARGUMENT);
        NBLOG(ERROR) << "File name or file path is empty.";
        return result;
    }

    if (bucket_name_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_BUCKET_NAME);
        NBLOG(ERROR) << "Bucket name is empty.";
        return result;
    }

    NbResult<NbHttpResponse> rest_result = service_->ExecuteFileDownload(
        [this, &file_name](NbHttpRequestFactory &request_factory) -> NbHttpRequest {
            return request_factory.Get(kFilesPath)
                                  .AppendPath("/" + bucket_name_ + "/" + curlpp::escape(file_name))
                                  .Build();
        }, file_path, timeout_);

    result.SetResultCode(rest_result.GetResultCode());

    if (rest_result.IsSuccess()) {
        int file_size = 0;
        auto headers = rest_result.GetSuccessData().GetHeaders();
        auto x_content_length = headers.find(kHeaderXContentLength);
        if (x_content_length != headers.end()) {
            file_size = std::atoi(x_content_length->second.c_str());
        }
        result.SetSuccessData(file_size);
    } else if (rest_result.IsRestError()) {
        result.SetRestError(rest_result.GetRestError());
    }

    return result;
}

NbResult<NbFileMetadata> NbFileBucket::UploadNewFile(const string &file_name, const string &file_path,
                                                     const string &content_type, const string &acl,
                                                     bool cache_disable) {
    NBLOG(TRACE) << __func__;

    NbResult<NbFileMetadata> result;

    if (file_name.empty() || file_path.empty() || content_type.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_INVALID_ARGUMENT);
        NBLOG(ERROR) << "File name or file path or Content-Type is empty.";
        return result;
    }

    if (bucket_name_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_BUCKET_NAME);
        NBLOG(ERROR) << "Bucket name is empty.";
        return result;
    }

    NbResult<NbHttpResponse> rest_result = service_->ExecuteFileUpload(
        [this, &file_name, &content_type, &acl, cache_disable](NbHttpRequestFactory &request_factory) -> NbHttpRequest {
            request_factory.Post(kFilesPath)
                           .AppendPath("/" + bucket_name_ + "/" + curlpp::escape(file_name))
                           .AppendHeader(kHeaderContentType, content_type);
            if (!acl.empty()) {
                request_factory.AppendHeader(kHeaderXAcl, acl);
            }
            if (cache_disable) {
                request_factory.AppendParam(kKeyCacheDisabled, "true");
            }
            return request_factory.Build();
        }, file_path, timeout_);

    result.SetResultCode(rest_result.GetResultCode());

    if (rest_result.IsSuccess()) {
        const NbHttpResponse &http_response = rest_result.GetSuccessData();
        NbJsonObject json(http_response.GetBody());
        NbFileMetadata metadata(bucket_name_, json);
        result.SetSuccessData(metadata);
    } else if (rest_result.IsRestError()) {
        result.SetRestError(rest_result.GetRestError());
    }

    return result;
}

NbResult<NbFileMetadata> NbFileBucket::UploadNewFile(const string &file_name, const string &file_path,
                                                     const string &content_type, const NbAcl &acl,
                                                     bool cache_disable) {
    string acl_string = acl.ToJsonObject().ToJsonString();
    return UploadNewFile(file_name, file_path, content_type, acl_string, cache_disable);
}

NbResult<NbFileMetadata> NbFileBucket::UploadNewFile(const string &file_name, const string &file_path,
                                                     const string &content_type, bool cache_disable) {
    return UploadNewFile(file_name, file_path, content_type, "", cache_disable);
}

NbResult<NbFileMetadata> NbFileBucket::UploadUpdateFile(const string &file_path, const NbFileMetadata &metadata) {
    return UploadUpdateFile(metadata.GetFileName(), file_path, metadata.GetContentType(),
                            metadata.GetMetaETag(), metadata.GetFileETag());
}

NbResult<NbFileMetadata> NbFileBucket::UploadUpdateFile(const string &file_name, const string &file_path,
                                                        const string &content_type, const string &meta_etag,
                                                        const string &file_etag) {
    NBLOG(TRACE) << __func__;

    NbResult<NbFileMetadata> result;

    if (file_name.empty() || file_path.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_INVALID_ARGUMENT);
        return result;
        NBLOG(ERROR) << "File name or file path is empty.";
    }

    if (bucket_name_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_BUCKET_NAME);
        NBLOG(ERROR) << "Bucket name is empty.";
        return result;
    }

    NbResult<NbHttpResponse> rest_result = service_->ExecuteFileUpload(
        [this, &file_name, &content_type, &meta_etag, &file_etag](NbHttpRequestFactory &request_factory) -> NbHttpRequest {
            request_factory.Put(kFilesPath)
                           .AppendPath("/" + bucket_name_ + "/" + curlpp::escape(file_name));
            if (!content_type.empty()) {
                request_factory.AppendHeader(kHeaderContentType, content_type);
            }
            if (!meta_etag.empty()) {
                request_factory.AppendParam(kKeyMetaETag, meta_etag);
            }
            if (!file_etag.empty()) {
                request_factory.AppendParam(kKeyFileETag, file_etag);
            }
            return request_factory.Build();
        }, file_path, timeout_);

    result.SetResultCode(rest_result.GetResultCode());

    if (rest_result.IsSuccess()) {
        const NbHttpResponse &http_response = rest_result.GetSuccessData();
        NbJsonObject json(http_response.GetBody());
        NbFileMetadata metadata(bucket_name_ ,json);
        result.SetSuccessData(metadata);
    } else if (rest_result.IsRestError()) {
        result.SetRestError(rest_result.GetRestError());
    }

    return result;
}

NbResult<NbFileMetadata> NbFileBucket::DeleteFile(const NbFileMetadata &metadata, bool delete_mark) {
    return DeleteFile(metadata.GetFileName(), metadata.GetMetaETag(),
                      metadata.GetFileETag(), delete_mark);
}

NbResult<NbFileMetadata> NbFileBucket::DeleteFile(const string &file_name, const string &meta_etag,
                                                const string &file_etag, bool delete_mark) {
    NBLOG(TRACE) << __func__;

    NbResult<NbFileMetadata> result;

    if (file_name.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_INVALID_ARGUMENT);
        NBLOG(ERROR) << "File name is empty.";
        return result;
    }

    if (bucket_name_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_BUCKET_NAME);
        NBLOG(ERROR) << "Bucket name is empty.";
        return result;
    }

    NbResult<NbHttpResponse> rest_result = service_->ExecuteRequest(
        [this, &file_name, &meta_etag, &file_etag, delete_mark](NbHttpRequestFactory &request_factory) -> NbHttpRequest {
            request_factory.Delete(kFilesPath)
                           .AppendPath("/" + bucket_name_ + "/" + curlpp::escape(file_name));
            if (!meta_etag.empty()) {
                request_factory.AppendParam(kKeyMetaETag, meta_etag);
            }
            if (!file_etag.empty()) {
                request_factory.AppendParam(kKeyFileETag, file_etag);
            }
            if (delete_mark) {
                request_factory.AppendParam(kKeyDeleteMark, "1");
            }
            return request_factory.Build();
        }, timeout_);

    result.SetResultCode(rest_result.GetResultCode());

    if (rest_result.IsSuccess()) {
        const NbHttpResponse &http_response = rest_result.GetSuccessData();
        NbJsonObject json(http_response.GetBody());
        NbFileMetadata metadata(bucket_name_, json);
        result.SetSuccessData(metadata);
    } else if (rest_result.IsRestError()) {
        result.SetRestError(rest_result.GetRestError());
    }

    return result;
}

NbResult<vector<NbFileMetadata>> NbFileBucket::GetFiles(bool published, bool delete_mark) {
    NBLOG(TRACE) << __func__;

    NbResult<vector<NbFileMetadata>> result;

    if (bucket_name_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_BUCKET_NAME);
        NBLOG(ERROR) << "Bucket name is empty.";
        return result;
    }

    NbResult<NbHttpResponse> rest_result = service_->ExecuteRequest(
        [this, &published, delete_mark](NbHttpRequestFactory &request_factory) -> NbHttpRequest {
            request_factory.Get(kFilesPath)
                           .AppendPath("/" + bucket_name_);
            if (published) {
                request_factory.AppendParam(kKeyPublished, "1");
            }
            if (delete_mark) {
                request_factory.AppendParam(kKeyDeleteMark, "1");
            }
            return request_factory.Build();
        }, timeout_);

    result.SetResultCode(rest_result.GetResultCode());

    if (rest_result.IsSuccess()) {
        const NbHttpResponse &http_response = rest_result.GetSuccessData();
        NbJsonObject json(http_response.GetBody());
        NbJsonArray json_array = json.GetJsonArray(kKeyResults);
        vector<NbFileMetadata> metadata_list;
        for (int i = 0; i < json_array.GetSize(); ++i) {
            NbFileMetadata metadata(bucket_name_, json_array.GetJsonObject(i));
            metadata_list.push_back(metadata);
        }
        result.SetSuccessData(metadata_list);
    } else if (rest_result.IsRestError()) {
        result.SetRestError(rest_result.GetRestError());
    }

    return result;
}

int NbFileBucket::GetTimeout() const {
    return timeout_;
}

void NbFileBucket::SetTimeout(int timeout) {
    timeout_ = timeout;
}
} //namespace necbaas
