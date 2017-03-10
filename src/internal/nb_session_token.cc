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

void NbSessionToken::SetSessionToken(const string &session_token, time_t expire_at) {
    session_token_ = session_token;
    expire_at_ = expire_at;
    return;
}

const string &NbSessionToken::GetSessionToken() const { return session_token_; }

time_t NbSessionToken::GetExpireAt() const { return expire_at_; }

void NbSessionToken::SetSessionUserEntity(const NbUserEntity &entity) {
    session_user_entity_ = entity;
    return;
}

const NbUserEntity &NbSessionToken::GetSessionUserEntity() const { return session_user_entity_; }

void NbSessionToken::ClearSessionToken() {
    session_token_.clear();
    expire_at_ = 0;
    NbUserEntity entity;
    session_user_entity_ = entity;

    return;
}
}  // namespace necbaas
