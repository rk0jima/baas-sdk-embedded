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

#include "necbaas/nb_acl_base.h"
#include <cassert>
#include "necbaas/nb_json_array.h"
#include "necbaas/internal/nb_constants.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::list;

// コンストラクタ
NbAclBase::NbAclBase() {}

// デストラクタ.
NbAclBase::~NbAclBase() {}

void NbAclBase::SortUnique(list<string> *acl_list) {
    acl_list->sort();
    acl_list->unique();
}

list<string> &NbAclBase::GetListByPermission(NbAclPermission permission) {
    switch (permission) {
        case NbAclPermission::READ:
            return read_;
        case NbAclPermission::WRITE:
            return write_;
        case NbAclPermission::CREATE:
            return create_;
        case NbAclPermission::UPDATE:
            return update_;
        case NbAclPermission::DELETE:
            return delete_;
        default:
            NBLOG(ERROR) << "invalid permission: " << static_cast<int>(permission);
            assert(false);
    }
}

void NbAclBase::AddEntry(NbAclPermission permission, const string &entry) {
    if (!entry.empty()) {
        list<string> &acl_list = GetListByPermission(permission);
        acl_list.push_back(entry);
        SortUnique(&acl_list);
    }
}

const list<string> &NbAclBase::GetWrite() const {
    return write_;
}

const list<string> &NbAclBase::GetRead() const {
    return read_;
}

const list<string> &NbAclBase::GetCreate() const {
    return create_;
}

const list<string> &NbAclBase::GetUpdate() const {
    return update_;
}

const list<string> &NbAclBase::GetDelete() const {
    return delete_;
}

void NbAclBase::RemoveEntry(NbAclPermission permission, const string &entry) {
    if (!entry.empty()) {
        list<string> &acl_list = GetListByPermission(permission);
        for (auto it = acl_list.begin(); it != acl_list.end(); ++it) {
            if (*it == entry) {
                GetListByPermission(permission).erase(it);
                break;
            }
        }
    }
}

void NbAclBase::SetEntries(const list<string> &entries, list<string> *acl_list) {
    acl_list->clear();
    for (auto entry : entries) {
        if (!entry.empty()) {
            acl_list->push_back(entry);
        }
    }
    SortUnique(acl_list);
}

void NbAclBase::SetWrite(const list<string> &write) {
    SetEntries(write, &write_);
}

void NbAclBase::SetRead(const list<string> &read) {
    SetEntries(read, &read_);
}

void NbAclBase::SetCreate(const list<string> &create) {
    SetEntries(create, &create_);
}

void NbAclBase::SetUpdate(const list<string> &update) {
    SetEntries(update, &update_);
}

void NbAclBase::SetDelete(const list<string> &del) {
    SetEntries(del, &delete_);
}

void NbAclBase::SetJson(const NbJsonObject &json) {
    json.GetJsonArray(kKeyRead).GetAllString(&read_);
    SortUnique(&read_);
    json.GetJsonArray(kKeyWrite).GetAllString(&write_);
    SortUnique(&write_);
    json.GetJsonArray(kKeyCreate).GetAllString(&create_);
    SortUnique(&create_);
    json.GetJsonArray(kKeyUpdate).GetAllString(&update_);
    SortUnique(&update_);
    json.GetJsonArray(kKeyDelete).GetAllString(&delete_);
    SortUnique(&delete_);
}

NbJsonObject NbAclBase::ToJsonObject() const {
    NbJsonObject json;
    NbJsonArray array;

    array.PutAllList(read_);
    json.PutJsonArray(kKeyRead, array);

    array.PutAllList(write_);
    json.PutJsonArray(kKeyWrite, array);

    array.PutAllList(create_);
    json.PutJsonArray(kKeyCreate, array);

    array.PutAllList(update_);
    json.PutJsonArray(kKeyUpdate, array);

    array.PutAllList(delete_);
    json.PutJsonArray(kKeyDelete, array);

    return json;
}

string NbAclBase::ToJsonString() const {
    return ToJsonObject().ToJsonString();
}
} //namespace necbaas
