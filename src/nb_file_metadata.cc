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

#include "necbaas/nb_file_metadata.h"
#include "necbaas/internal/nb_utility.h"

namespace necbaas {

using std::string;

// コンストラクタ
NbFileMetadata::NbFileMetadata() {}

NbFileMetadata::NbFileMetadata(const std::string &bucket_name, const NbJsonObject &json) {
    parent_bucket_name_ = bucket_name;
    file_name_ = json.GetString(kKeyFilename);
    content_type_ = json.GetString(kKeyContentType);
    acl_ = NbAcl(json.GetJsonObject(kKeyAcl));
    length_ = json.GetInt(kKeyLength);
    created_time_ = json.GetString(kKeyCreatedAt);
    updated_time_ = json.GetString(kKeyUpdatedAt);
    meta_etag_ = json.GetString(kKeyMetaETag);
    file_etag_ = json.GetString(kKeyFileETag);
    cache_disabled_ = json.GetBoolean(kKeyCacheDisabled);
}

// デストラクタ.
NbFileMetadata::~NbFileMetadata() {}

const string &NbFileMetadata::GetFileName() const {
    return file_name_;
}

const string &NbFileMetadata::GetContentType() const {
    return content_type_;
}

const NbAcl &NbFileMetadata::GetAcl() const {
    return acl_;
}

int NbFileMetadata::GetLength() const {
    return length_;
}

std::tm NbFileMetadata::GetCreatedTime() const {
    return NbUtility::DateStringToTm(created_time_);
}

std::tm NbFileMetadata::GetUpdatedTime() const {
    return NbUtility::DateStringToTm(updated_time_);
}

const string &NbFileMetadata::GetMetaETag() const {
    return meta_etag_;
}

const string &NbFileMetadata::GetFileETag() const {
    return file_etag_;
}

bool NbFileMetadata::IsCacheDisabled() const {
    return cache_disabled_;
}
} //namespace necbaas
