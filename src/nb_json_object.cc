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

#include "necbaas/nb_json_object.h"
#include "necbaas/nb_json_array.h"
#include "necbaas/internal/nb_utility.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::vector;

NbJsonObject::NbJsonObject() : value_(Json::Value::null) {
    Json::Reader reader;
    reader.parse("{}", value_, false);
};

NbJsonObject::NbJsonObject(const string &json_string) {
    PutAll(json_string);
}

NbJsonObject::NbJsonObject(const vector<char> &json_char) {
    Json::Reader reader;
    bool ret = reader.parse(json_char.data(), json_char.data() + json_char.size(), value_, false);
    if(!ret || !value_.isObject()) {
        reader.parse("{}", value_, false);
    }
}

NbJsonObject::~NbJsonObject() {};

bool NbJsonObject::PutAll(const string &json) {
    Json::Reader reader;
    bool ret = reader.parse(json, value_, false);
    if(!ret || !value_.isObject()) {
        reader.parse("{}", value_, false);
    }
    return ret;
}

vector<string> NbJsonObject::GetKeySet() const {
    return value_.getMemberNames();
}


int NbJsonObject::GetInt(const string &key, int default_value) const {
    int ret = default_value;
    if (value_.isMember(key) && value_[key].isNumeric()) {
        try {
            ret = (int)value_[key].asInt();
        }
        catch (const Json::LogicError &ex) {
            NBLOG(ERROR) << ex.what(); 
        }
    }
    return ret;
}

int64_t NbJsonObject::GetInt64(const string &key, int64_t default_value) const {
    int64_t ret = default_value;
    if (value_.isMember(key) && value_[key].isNumeric()) {
        try {
            ret = (int64_t)value_[key].asInt64();
        }
        catch (const Json::LogicError &ex) {
            NBLOG(ERROR) << ex.what();
        }
    }
    return ret;
}

double NbJsonObject::GetDouble(const string &key, double default_value) const {
    double ret = default_value;
    if (value_.isMember(key) && value_[key].isNumeric()) {
        ret = value_[key].asDouble();
    }
    return ret;
}

bool NbJsonObject::GetBoolean(const string &key, bool default_value) const {
    bool ret = default_value;
    if (value_.isMember(key) && value_[key].isBool()) {
        ret = value_[key].asBool();
    }
    return ret;
}

string NbJsonObject::GetString(const string &key, const string default_value) const {
    string ret = default_value;
    if (value_.isMember(key) && value_[key].isString()) {
        ret = value_[key].asString();
    }
    return ret;
}

NbJsonObject NbJsonObject::GetJsonObject(const string &key) const {
    NbJsonObject ret;
    if (value_.isMember(key) && value_[key].isObject()) {
        ret.Replace(value_[key]);
    }
    return ret;
}

NbJsonArray NbJsonObject::GetJsonArray(const string &key) const {
    NbJsonArray ret;
    if (value_.isMember(key) && value_[key].isArray()) {
        ret.Replace(value_[key]);
    }
    return ret;
}

const Json::Value &NbJsonObject::GetSubstitutableValue() const {
    return value_;
}

Json::Value& NbJsonObject::operator[](const string &key) {
    return value_[key];
}

const Json::Value& NbJsonObject::operator[](const string &key) const {
    //参照は未サポート
    return Json::Value::null;
}

void NbJsonObject::PutJsonObject(const string &key, const NbJsonObject &json_object) {
    if (!key.empty()) {
        value_[key] = json_object.GetSubstitutableValue();
    }
}

void NbJsonObject::PutJsonArray(const string &key, const NbJsonArray &json_array) {
    if (!key.empty()) {
        value_[key] = json_array.GetSubstitutableValue();
    }
}

void NbJsonObject::PutNull(const string &key) {
    if (!key.empty()) {
        value_[key] = Json::Value::null;
    }
}

unsigned int NbJsonObject::GetSize() const {
    return value_.size();
}

bool NbJsonObject::IsEmpty() const {
    return value_.empty();
}

bool NbJsonObject::IsMember(const string &key) const {
    return value_.isMember(key);
}

NbJsonType NbJsonObject::GetType(const string &key) const {
    return NbUtility::ConvertJsonType(value_[key].type());
}

void NbJsonObject::Remove(const string &key) {
    value_.removeMember(key);
}

void NbJsonObject::Clear() {
    value_.clear();
}

string NbJsonObject::ToJsonString() const {
    // 改行、空白を付けないように Json::FastWriter を使用する
    Json::FastWriter writer;
    writer.omitEndingLineFeed(); // 行末の改行を省略
    return writer.write(value_);
}

void NbJsonObject::Replace(const Json::Value &value) {
    value_ = value;
}
} //namespace necbaas
