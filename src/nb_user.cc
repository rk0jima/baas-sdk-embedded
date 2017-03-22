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

#include "necbaas/nb_user.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::vector;
using std::shared_ptr;
using std::time_t;

// コンストラクタ

NbUser::NbUser() {}

NbUser::NbUser(const shared_ptr<NbService> &service)
        : service_(service) {}

// デストラクタ
NbUser::~NbUser() {}

// ログイン（ユーザネーム）
NbResult<NbUser> NbUser::LoginWithUsername(const shared_ptr<NbService> &service, const string &username,
                                           const string &password, int timeout) {
    return Login(service, username, "", password, timeout);
}

// ログイン（E-mailアドレス）
NbResult<NbUser> NbUser::LoginWithEmail(const shared_ptr<NbService> &service, const string &email,
                                        const string &password, int timeout) {
    return Login(service, "", email, password, timeout);
}

// ログイン
NbResult<NbUser> NbUser::Login(const shared_ptr<NbService> &service, const string &username,
                               const string &email, const string &password, int timeout) {
    NBLOG(TRACE) << __func__;

    NbResult<NbUser> result;
    if ((username.empty() && email.empty()) || password.empty()) {
        //エラー処理
        result.SetResultCode(NbResultCode::NB_ERROR_INVALID_ARGUMENT);
        NBLOG(ERROR) << "Account or password is empty.";
        return result;
    }

    NbJsonObject body_json;
    if (!username.empty()) {
        body_json[kKeyUsername] = username;
    }
    if (!email.empty()) {
        body_json[kKeyEmail] = email;
    }
    body_json[kKeyPassword] = password;

    //HTTPリクエスト作成
    NbHttpRequestFactory request_factory = service->GetHttpRequestFactory();
    if (request_factory.IsError()) {
        //request構築エラー
        result.SetResultCode(request_factory.GetError());
        return result;
    }
    NbHttpRequest request = request_factory.Post(kLoginUrl)
                                           .AppendHeader(kHeaderContentType, kHeaderContentTypeJson)
                                           .Body(body_json.ToJsonString())
                                           .SessionNone()
                                           .Build();
    //リクエスト実行
    NbRestExecutor *executor = service->PopRestExecutor();
    if (!executor) {
        // 同時接続数オーバー
        result.SetResultCode(NbResultCode::NB_ERROR_CONNECTION_OVER);
        return result;
    }
    NbResult<NbHttpResponse> rest_result = executor->ExecuteRequest(request, timeout);
    service->PushRestExecutor(executor);

    result.SetResultCode(rest_result.GetResultCode());

    if (rest_result.IsSuccess()) {    
        const NbHttpResponse &http_response = rest_result.GetSuccessData();

        NbJsonObject json(http_response.GetBody());

        NbUser::SetCurrentUser(service, json);

        result.SetSuccessData(NbUser::GetCurrentUser(service));
    } else {
        // 一律Restエラー情報をコピー
        result.SetRestError(rest_result.GetRestError());
    }
        
    return result;
}

NbResult<NbUser> NbUser::Logout(const shared_ptr<NbService> &service, int timeout) {
    NBLOG(TRACE) << __func__;

    NbResult<NbUser> result;

    if (!IsLoggedIn(service)) {
        // 未ログイン
        result.SetResultCode(NbResultCode::NB_ERROR_NOT_LOGGED_IN);
        NBLOG(ERROR) << "Not logged in.";
        return result;
    }

    //HTTPリクエスト作成
    NbHttpRequestFactory request_factory = service->GetHttpRequestFactory();

    // この時点で内部に保存しているセッショントークンをクリア(ログアウト)する
    // RESTに使用するセッショントークンは、ファクトリインスタンスにコピーされている
    service->ClearSessionToken();

    if (request_factory.IsError()) {
        //request構築エラー
        result.SetResultCode(request_factory.GetError());
        return result;
    }
    NbHttpRequest request = request_factory.Delete(kLoginUrl)
                                           .Build();
    //リクエスト実行
    NbRestExecutor *executor = service->PopRestExecutor();
    if (!executor) {
        // 同時接続数オーバー
        result.SetResultCode(NbResultCode::NB_ERROR_CONNECTION_OVER);
        return result;
    }
    NbResult<NbHttpResponse> rest_result = executor->ExecuteRequest(request, timeout);
    service->PushRestExecutor(executor);

    result.SetResultCode(rest_result.GetResultCode());

    if (rest_result.IsSuccess()) {
        const NbHttpResponse &http_response = rest_result.GetSuccessData();

        NbJsonObject json(http_response.GetBody());
        NbUserEntity user_entity(json);
        NbUser user(service);
        user.SetUserEntity(user_entity);
        result.SetSuccessData(user);

    } else if (rest_result.IsRestError()) {
        result.SetRestError(rest_result.GetRestError());
    }

    return result;
}

bool NbUser::IsLoggedIn(const shared_ptr<NbService> &service) {
    NbSessionToken session_token = service->GetSessionToken();

    return (!session_token.GetSessionToken().empty() &&
            !NbSessionToken::IsExpired(session_token.GetExpireAt()));
}

time_t NbUser::GetSessionTokenExpiration(const shared_ptr<NbService> &service) {
    NbSessionToken session_token = service->GetSessionToken();
    return session_token.GetExpireAt();
}

const string NbUser::GetSessionToken(const shared_ptr<NbService> &service) {
    NbSessionToken session_token = service->GetSessionToken();
    return session_token.GetSessionToken();
}

string NbUser::ExportCurrentLogin(const shared_ptr<NbService> &service) {
    if (!IsLoggedIn(service)) {
        NBLOG(ERROR) << "Not logged in.";
        return "";
    }

    NbJsonObject json = service->GetSessionToken().GetSessionUserEntity().ToJsonObject();
    
    json[kKeySessionToken] = NbUser::GetSessionToken(service);
    json[kKeyExpire] = (int)NbUser::GetSessionTokenExpiration(service);

    return json.ToJsonString();
}

NbResultCode NbUser::ImportCurrentLogin(const shared_ptr<NbService> &service, const string &import) {
    NbJsonObject json(import);

    if (json.GetString(kKeySessionToken).empty()) {
        NBLOG(ERROR) << "Import data is invalid.";
        return NbResultCode::NB_ERROR_INVALID_ARGUMENT;
    }

    if (NbSessionToken::IsExpired(static_cast<std::time_t>(json.GetInt64(kKeyExpire)))) {
        NBLOG(ERROR) << "Session token is expired.";
        return NbResultCode::NB_ERROR_SESSION_EXPIRED;
    }

    NbUser::SetCurrentUser(service, json);

    return NbResultCode::NB_OK;
}

void NbUser::SetCurrentUser(const shared_ptr<NbService> &service, NbJsonObject json) {
    string token = json.GetString(kKeySessionToken);
    time_t expire = (time_t)json.GetInt64(kKeyExpire);

    NbSessionToken session_token(token, expire);

    NbUserEntity user_entity(json);
    session_token.SetSessionUserEntity(user_entity);

    service->SetSessionToken(session_token);
}

NbUser NbUser::GetCurrentUser(const shared_ptr<NbService> &service) {
    NbSessionToken session_token = service->GetSessionToken();
    NbUser user(service);
    user.SetUserEntity(session_token.GetSessionUserEntity());
    return user;
}

void NbUser::SetUserEntity(const NbUserEntity &entity) {
    user_entity_ = entity;
}

const string &NbUser::GetUserName() const {
    return user_entity_.GetUserName();
}

const string &NbUser::GetEmail() const {
    return user_entity_.GetEmail();
}

const NbJsonObject &NbUser::GetOptions() const {
    return user_entity_.GetOptions();
}

vector<string> NbUser::GetGroups() const {
    return user_entity_.GetGroups();
}

const string &NbUser::GetUserId() const {
    return user_entity_.GetId();
}

std::tm NbUser::GetCreatedTime() const {
    return user_entity_.GetCreatedTime();
}

std::tm NbUser::GetUpdatedTime() const {
    return user_entity_.GetUpdatedTime();
}
} //namespace necbaas
