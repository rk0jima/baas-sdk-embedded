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

#include "necbaas/nb_acl.h"
#include "necbaas/nb_json_array.h"
#include "necbaas/internal/nb_constants.h"

namespace necbaas {

using std::string;
using std::list;

// コンストラクタ
NbAcl::NbAcl() {}

NbAcl::NbAcl(const NbJsonObject &json) {
    SetJson(json);
}

// デストラクタ.
NbAcl::~NbAcl() {}

const list<string> &NbAcl::GetAdmin() const {
    return admin_;
}

const string &NbAcl::GetOwner() const {
    return owner_;
}

void NbAcl::SetAdmin(const list<string> &admin) {
    SetEntries(admin, &admin_);
}

void NbAcl::SetOwner(const string &owner) {
    owner_ = owner;
}

list<string> &NbAcl::GetListByPermission(NbAclPermission permission) {
    switch (permission) {
        case NbAclPermission::ADMIN:
            return admin_;
        default:
            return NbAclBase::GetListByPermission(permission);
    }
}

void NbAcl::SetJson(NbJsonObject json) {
    owner_ = json.GetString(kKeyOwner);
    json.GetJsonArray(kKeyAdmin).GetAllString(&admin_);
    SortUnique(&admin_);
    NbAclBase::SetJson(json);
}

NbJsonObject NbAcl::ToJsonObject() const {
    NbJsonObject json = NbAclBase::ToJsonObject();
    NbJsonArray array;

    json[kKeyOwner] = owner_;

    array.PutAllList(admin_);
    json.PutJsonArray(kKeyAdmin, array);

    return json;
}

NbAcl NbAcl::CreateAclForAnonymous() {
    return CreateAclFor(kGroupAnonymous);
}

NbAcl NbAcl::CreateAclForAuthenticated() {
    return CreateAclFor(kGroupAuthenticated);
}

NbAcl NbAcl::CreateAclForUser(const NbUser &user) {
    NbAcl acl = CreateAclFor(user.GetUserId());
    acl.owner_ = user.GetUserId();
    return acl;
}

NbAcl NbAcl::CreateAclFor(const string &entry) {
    NbAcl acl;
    acl.AddEntry(NbAclPermission::READ, entry);
    acl.AddEntry(NbAclPermission::WRITE, entry);
    acl.AddEntry(NbAclPermission::ADMIN, entry);
    return acl;
}

NbAcl NbAcl::CreateAclFor(list<string> entries) {
    NbAcl acl;
    acl.SetRead(entries);
    acl.SetWrite(entries);
    acl.SetAdmin(entries);
    return acl;
}
} //namespace necbaas
