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

#include "necbaas/nb_object.h"
#include "necbaas/internal/nb_utility.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::shared_ptr;

// コンストラクタ
NbObject::NbObject() {}

NbObject::NbObject(const shared_ptr<NbService> &service, const string &bucket_name)
        : service_(service), bucket_name_(bucket_name) {}

NbObject::~NbObject() {}

NbResult<NbObject> NbObject::PartUpdateObject(const NbJsonObject &json, bool acl) {
    NBLOG(TRACE) << __func__;

    NbResult<NbObject> result;

    if (json.IsEmpty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_INVALID_ARGUMENT);
        NBLOG(ERROR) << "Json data is empty.";
        return result;
    }

    if (bucket_name_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_BUCKET_NAME);
        NBLOG(ERROR) << "Bucket name is empty.";
        return result;
    }

    if (object_id_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_OBJECT_ID);
        NBLOG(ERROR) << "Object ID is empty.";
        return result;
    }

    NbJsonObject body_json = json;
    RemoveReservationFields(&body_json);
    if (acl) {
        body_json.PutJsonObject(kKeyAcl, acl_.ToJsonObject());
    }    

    //HTTPリクエスト作成
    NbHttpRequestFactory request_factory = service_->GetHttpRequestFactory();
    if (request_factory.IsError()) {
        //request構築エラー
        result.SetResultCode(request_factory.GetError());
        return result;
    }
    request_factory.Put(kObjectsPath)
                   .AppendPath("/" + bucket_name_ + "/" + object_id_)
                   .AppendHeader(kHeaderContentType, kHeaderContentTypeJson)
                   .Body(body_json.ToJsonString());
    if (!etag_.empty()) {
        request_factory.AppendParam(kKeyETag, etag_);
    }
    NbHttpRequest request = request_factory.Build();

    //リクエスト実行
    NbRestExecutor *executor = service_->PopRestExecutor();
    if (!executor) {
        // 同時接続数オーバー
        result.SetResultCode(NbResultCode::NB_ERROR_CONNECTION_OVER);
        return result;
    }
    NbResult<NbHttpResponse> rest_result = executor->ExecuteRequest(request, timeout_);
    service_->PushRestExecutor(executor);

    result.SetResultCode(rest_result.GetResultCode());

    if (rest_result.IsSuccess()) {
        const NbHttpResponse &http_response = rest_result.GetSuccessData();
        NbJsonObject response_json(http_response.GetBody());
        SetCurrentParam(response_json);
        result.SetSuccessData(*this);
    } else if (rest_result.IsRestError()) {
        result.SetRestError(rest_result.GetRestError());
    }

    return result;
}

NbResult<NbObject> NbObject::DeleteObject(bool delete_mark) {
    NBLOG(TRACE) << __func__;

    NbResult<NbObject> result;

    if (bucket_name_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_BUCKET_NAME);
        NBLOG(ERROR) << "Bucket name is empty.";
        return result;
    }

    if (object_id_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_OBJECT_ID);
        NBLOG(ERROR) << "Object ID is empty.";
        return result;
    }

    //HTTPリクエスト作成
    NbHttpRequestFactory request_factory = service_->GetHttpRequestFactory();
    if (request_factory.IsError()) {
        //request構築エラー
        result.SetResultCode(request_factory.GetError());
        return result;
    }
    request_factory.Delete(kObjectsPath)
                   .AppendPath("/" + bucket_name_ + "/" + object_id_);
    if (!etag_.empty()) {
        request_factory.AppendParam(kKeyETag, etag_);
    }
    if (delete_mark) {
        request_factory.AppendParam(kKeyDeleteMark, "1");
    }
    NbHttpRequest request = request_factory.Build();

    //リクエスト実行
    NbRestExecutor *executor = service_->PopRestExecutor();
    if (!executor) {
        // 同時接続数オーバー
        result.SetResultCode(NbResultCode::NB_ERROR_CONNECTION_OVER);
        return result;
    }
    NbResult<NbHttpResponse> rest_result = executor->ExecuteRequest(request, timeout_);
    service_->PushRestExecutor(executor);

    result.SetResultCode(rest_result.GetResultCode());

    if (rest_result.IsSuccess()) {
        const NbHttpResponse &http_response = rest_result.GetSuccessData();
        NbJsonObject json_obj(http_response.GetBody());
        if (delete_mark) {
            //削除マークの場合は、レスポンスで自データ更新
            SetCurrentParam(json_obj);
            result.SetSuccessData(*this);
        } else {
            //完全削除の場合は、空オブジェクトを設定
            NbObject empty_obj(service_, bucket_name_);
            empty_obj.SetObjectData(json_obj);
            result.SetSuccessData(empty_obj);
        }
    } else if (rest_result.IsRestError()) {
        result.SetRestError(rest_result.GetRestError());
    }

    return result;
}

NbResult<NbObject> NbObject::Save(bool acl) {
    NBLOG(TRACE) << __func__;

    NbResult<NbObject> result;

    if (bucket_name_.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_BUCKET_NAME);
        NBLOG(ERROR) << "Bucket name is empty.";
        return result;
    }

    //HTTPリクエスト作成
    NbHttpRequestFactory request_factory = service_->GetHttpRequestFactory();
    if (request_factory.IsError()) {
        //request構築エラー
        result.SetResultCode(request_factory.GetError());
        return result;
    }

    NbJsonObject json(*this);
    RemoveReservationFields(&json);
    
    if (object_id_.empty()) { //新規
        if (acl) {
            json.PutJsonObject(kKeyAcl, acl_.ToJsonObject());
        }

        request_factory.Post(kObjectsPath)
                       .AppendPath("/" + bucket_name_)
                       .AppendHeader(kHeaderContentType, kHeaderContentTypeJson)
                       .Body(json.ToJsonString());
    } else { //更新
        json.PutJsonObject(kKeyAcl, acl_.ToJsonObject());
        if (!created_time_.empty()) {
            json[kKeyCreatedAt] = created_time_;
        }

        NbJsonObject json_full;
        json_full.PutJsonObject("$full_update", json);

        request_factory.Put(kObjectsPath)
                       .AppendPath("/" + bucket_name_ + "/" + object_id_)
                       .AppendHeader(kHeaderContentType, kHeaderContentTypeJson)
                       .Body(json_full.ToJsonString());
        if (!etag_.empty()) {
            request_factory.AppendParam(kKeyETag, etag_);
        }
    }
    NbHttpRequest request = request_factory.Build();

    //リクエスト実行
    NbRestExecutor *executor = service_->PopRestExecutor();
    if (!executor) {
        // 同時接続数オーバー
        result.SetResultCode(NbResultCode::NB_ERROR_CONNECTION_OVER);
        return result;
    }
    NbResult<NbHttpResponse> rest_result = executor->ExecuteRequest(request, timeout_);
    service_->PushRestExecutor(executor);

    result.SetResultCode(rest_result.GetResultCode());

    if (rest_result.IsSuccess()) {
        const NbHttpResponse &http_response = rest_result.GetSuccessData();
        NbJsonObject response_json(http_response.GetBody());
        SetCurrentParam(response_json);
        result.SetSuccessData(*this);
    } else if (rest_result.IsRestError()) {
        result.SetRestError(rest_result.GetRestError());
    }

    return result;
}

void NbObject::SetCurrentParam(const NbJsonObject &json) {
    value_ = json.GetSubstitutableValue();

    if (value_.isMember(kKeyId)) {
        object_id_ = json.GetString(kKeyId);
        value_.removeMember(kKeyId);
    } else {
        object_id_.clear();
    }

    if (value_.isMember(kKeyCreatedAt)) {
        created_time_ = json.GetString(kKeyCreatedAt);
        value_.removeMember(kKeyCreatedAt);
    } else {
        created_time_.clear();
    }

    if (value_.isMember(kKeyUpdatedAt)) {
        updated_time_ = json.GetString(kKeyUpdatedAt);
        value_.removeMember(kKeyUpdatedAt);
    } else {
        updated_time_.clear();
    }

    if (value_.isMember(kKeyAcl)) {
        acl_ = NbAcl(json.GetJsonObject(kKeyAcl));
        value_.removeMember(kKeyAcl);
    } else {
        acl_ = NbAcl();
    }

    if (value_.isMember(kKeyETag)) {
        etag_ = json.GetString(kKeyETag);
        value_.removeMember(kKeyETag);
    } else {
        etag_.clear();
    }

    if (value_.isMember(kKeyDeleted)) {
        deleted_ = json.GetBoolean(kKeyDeleted);
        value_.removeMember(kKeyDeleted);
    } else {
        deleted_ = false;
    }
}

void NbObject::RemoveReservationFields(NbJsonObject *json) {
    json->Remove(kKeyId);
    json->Remove(kKeyCreatedAt);
    json->Remove(kKeyUpdatedAt);
    json->Remove(kKeyAcl);
    json->Remove(kKeyETag);
    json->Remove(kKeyDeleted);
}

int NbObject::GetTimeout() const {
    return timeout_;
}

void NbObject::SetTimeout(int timeout) {
    timeout_ = timeout;
}

const string &NbObject::GetBucketName() const {
    return bucket_name_;
}

const string &NbObject::GetObjectId() const {
    return object_id_;
}

void NbObject::SetObjectId(const string &object_id) {
    object_id_ = object_id;
}

const std::tm NbObject::GetCreatedTime() const {
    return NbUtility::DateStringToTm(created_time_);
}

const std::tm NbObject::GetUpdatedTime() const {
    return NbUtility::DateStringToTm(updated_time_);
}

void NbObject::SetCreatedTime(const std::tm &created_time) {
    created_time_ = NbUtility::TmToDateString(created_time);
}

const string &NbObject::GetETag() const {
    return etag_;
}

void NbObject::SetETag(const string &etag) {
    etag_ = etag;
}

const NbAcl &NbObject::GetAcl() const {
    return acl_;
}

void NbObject::SetAcl(const NbAcl &acl) {
    acl_ = acl;
}

bool NbObject::IsDeleteMark() const {
    return deleted_;
}

void NbObject::SetObjectData(const NbJsonObject &json) {
    value_ = json.GetSubstitutableValue();
}
} //namespace necbaas
