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

#include "necbaas/internal/nb_user_entity.h"
#include <cstring>
#include "necbaas/internal/nb_constants.h"
#include "necbaas/internal/nb_utility.h"

namespace necbaas {

using std::string;
using std::vector;

NbUserEntity::NbUserEntity() {}

NbUserEntity::NbUserEntity(const NbJsonObject &json) { Set(json); }

NbUserEntity::~NbUserEntity() {}

void NbUserEntity::Set(NbJsonObject json) {
    id_ = json.GetString(kKeyId);
    username_ = json.GetString(kKeyUsername);
    email_ = json.GetString(kKeyEmail);
    options_ = json.GetJsonObject(kKeyOptions);
    created_time_ = json.GetString(kKeyCreatedAt);
    updated_time_ = json.GetString(kKeyUpdatedAt);

    groups_ = json.GetJsonArray(kKeyGroups);
    return;
}

NbJsonObject NbUserEntity::ToJsonObject() const {
    NbJsonObject json;
    json[kKeyId] = id_;
    json[kKeyUsername] = username_;
    json[kKeyEmail] = email_;
    json.PutJsonObject(kKeyOptions, options_);
    json[kKeyCreatedAt] = created_time_;
    json[kKeyUpdatedAt] = updated_time_;
    json.PutJsonArray(kKeyGroups, groups_);
    return json;
}

const string &NbUserEntity::GetId() const { return id_; }

const string &NbUserEntity::GetUserName() const { return username_; }

const string &NbUserEntity::GetEmail() const { return email_; }

const NbJsonObject &NbUserEntity::GetOptions() const { return options_; }

vector<string> NbUserEntity::GetGroups() const {
    vector<string> group_list;
    groups_.GetAllString(&group_list);
    return group_list;
}

std::tm NbUserEntity::GetCreatedTime() const { return NbUtility::DateStringToTm(created_time_); }

std::tm NbUserEntity::GetUpdatedTime() const { return NbUtility::DateStringToTm(updated_time_); }
}  // namespace necbaas
