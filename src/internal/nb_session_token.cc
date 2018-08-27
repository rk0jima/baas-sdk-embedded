/*
 * Copyright (C) 2017 NEC Corporation
 */

#include "necbaas/internal/nb_session_token.h"

namespace necbaas {

using std::string;
using std::time_t;

NbSessionToken::NbSessionToken() {}

NbSessionToken::NbSessionToken(const string &session_token, time_t expire_at) {
    SetSessionToken(session_token, expire_at);
}

NbSessionToken::~NbSessionToken() {}

bool NbSessionToken::IsExpired(std::time_t expire_at) {
    return (expire_at <= std::time(nullptr));
}

bool NbSessionToken::IsValid() const {
    return !(session_token_.empty() || IsExpired(expire_at_));
}

void NbSessionToken::SetSessionToken(const string &session_token, time_t expire_at) {
    session_token_ = session_token;
    expire_at_ = expire_at;
    return;
}

string NbSessionToken::GetSessionToken() const {
    // セッショントークンが無効の場合は初期値を返す
    if (!IsValid()) {
        return string();
    }
    return session_token_;
}

time_t NbSessionToken::GetExpireAt() const { 
    // セッショントークンが無効の場合は0を返す
    if (!IsValid()) {
        return 0;
    }
    return expire_at_; 
}

void NbSessionToken::SetSessionUserEntity(const NbUserEntity &entity) {
    session_user_entity_ = entity;
    return;
}

NbUserEntity NbSessionToken::GetSessionUserEntity() const { 
    //  セッショントークンが無効の場合は初期値を返す
    if (!IsValid()) {
        return NbUserEntity();
    }
    return session_user_entity_; 
}

void NbSessionToken::ClearSessionToken() {
    session_token_.clear();
    expire_at_ = 0;
    NbUserEntity entity;
    session_user_entity_ = entity;

    return;
}
}  // namespace necbaas
