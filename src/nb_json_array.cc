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

#include "necbaas/nb_json_array.h"
#include "necbaas/nb_json_object.h"
#include "necbaas/internal/nb_utility.h"

namespace necbaas {

using std::string;
using std::vector;

static Json::Value tmp_value;

NbJsonArray::NbJsonArray() : value_(Json::Value::null) {
    Json::Reader reader;
    reader.parse("[]", value_, false);
}

NbJsonArray::NbJsonArray(const string &json_string) {
    PutAll(json_string);
}

NbJsonArray::~NbJsonArray() {}

void NbJsonArray::PutAll(const string &json) {
    Json::Reader reader;
    bool ret = reader.parse(json, value_, false);
    if(!ret || !value_.isArray()) {
        reader.parse("[]", value_, false);
    }
}

int NbJsonArray::GetInt(unsigned int index, int default_value) const {
    int ret = default_value;
    if (value_.size() > index && value_[index].isNumeric()) {
        try {
            ret = (int)value_[index].asInt();
        }
        catch (const Json::LogicError &ex) {
            NBLOG(ERROR) << ex.what();
        }
    }
    return ret;
}

int64_t NbJsonArray::GetInt64(unsigned int index, int64_t default_value) const {
    int64_t ret = default_value;
    if (value_.size() > index && value_[index].isNumeric()) {
        try {
            ret = (int64_t)value_[index].asInt64();
        }
        catch (const Json::LogicError &ex) {
            NBLOG(ERROR) << ex.what();
        }
    }
    return ret;
}

double NbJsonArray::GetDouble(unsigned int index, double default_value) const {
    double ret = default_value;
    if (value_.size() > index && value_[index].isNumeric()) {
        ret = value_[index].asDouble();
    }
    return ret;
}

bool NbJsonArray::GetBoolean(unsigned int index, bool default_value) const {
    bool ret = default_value;
    if (value_.size() > index && value_[index].isBool()) {
        ret = value_[index].asBool();
    }
    return ret;
}

string NbJsonArray::GetString(unsigned int index, const string &default_value) const {
    string ret = default_value;
    if (value_.size() > index && value_[index].isString()) {
        ret = value_[index].asString();
    }
    return ret;
}

NbJsonObject NbJsonArray::GetJsonObject(unsigned int index) const {
    NbJsonObject ret;
    if (value_.size() > index && value_[index].isObject()) {
        ret.Replace(value_[index]);
    }
    return ret;
}

NbJsonArray NbJsonArray::GetJsonArray(unsigned int index) const {
    NbJsonArray ret;
    if (value_.size() > index && value_[index].isArray()) {
        ret.Replace(value_[index]);
    }
    return ret;
}

const Json::Value &NbJsonArray::GetSubstitutableValue() const {
    return value_;
}

Json::Value& NbJsonArray::operator[](unsigned int index) {
    if (index == std::numeric_limits<unsigned int>::max()) {
        return tmp_value;
    }
    return value_[index];
}

const Json::Value& NbJsonArray::operator[](unsigned int index) const {
    //参照は未サポート
    return Json::Value::null;
}


bool NbJsonArray::PutJsonObject(unsigned int index, const NbJsonObject &json_object) {
    if (index == std::numeric_limits<unsigned int>::max()) {
        // UINTの最大値を超える場合は処理しない
        return false;
    }
    value_[index] = json_object.GetSubstitutableValue();
    return true;
}

bool NbJsonArray::PutJsonArray(unsigned int index, const NbJsonArray &json_array) {
    if (index == std::numeric_limits<unsigned int>::max()) {
        // UINTの最大値を超える場合は処理しない
        return false;
    }
    value_[index] = json_array.GetSubstitutableValue();
    return true;
}

bool NbJsonArray::PutNull(unsigned int index) {
    if (index == std::numeric_limits<unsigned int>::max()) {
        // UINTの最大値を超える場合は処理しない
        return false;
    }
    value_[index] = Json::Value::null;
    return true;
}

bool NbJsonArray::AppendJsonObject(const NbJsonObject &json_object) {
    if (value_.size() == std::numeric_limits<unsigned int>::max()) {
        // UINTの最大値を超える場合は処理しない
        return false;
    }
    value_[value_.size()] = json_object.GetSubstitutableValue();
    return true;
}

bool NbJsonArray::AppendJsonArray(const NbJsonArray &json_array) {
    if (value_.size() == std::numeric_limits<unsigned int>::max()) {
        // UINTの最大値を超える場合は処理しない
        return false;
    }
    value_[value_.size()] = json_array.GetSubstitutableValue();
    return true;
}

bool NbJsonArray::AppendNull() {
    if (value_.size() == std::numeric_limits<unsigned int>::max()) {
        // UINTの最大値を超える場合は処理しない
        return false;
    }
    value_[value_.size()] = Json::Value::null;
    return true;
}


unsigned int NbJsonArray::GetSize() const {
    return value_.size();
}

bool NbJsonArray::IsEmpty() const {
    return value_.empty();
}

NbJsonType NbJsonArray::GetType(unsigned int index) const {
    return NbUtility::ConvertJsonType(value_[index].type());
}

void NbJsonArray::Remove(unsigned int index) {
    Json::Value removed;
    value_.removeIndex(index, &removed);
}

void NbJsonArray::Clear() {
    value_.clear();
}

string NbJsonArray::ToJsonString() const {
    // 改行、空白を付けないように Json::FastWriter を使用する
    Json::FastWriter writer;
    writer.omitEndingLineFeed(); // 行末の改行を省略
    return writer.write(value_);
}

void NbJsonArray::Replace(const Json::Value &value) {
    value_ = value;
}
} //namespace necbaas
