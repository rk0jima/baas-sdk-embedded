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

#include "necbaas/nb_query.h"

namespace necbaas {

using std::string;
using std::vector;
using std::map;

//定数定義
const int NbQuery::kLimitDefault = 100;

// コンストラクタ
NbQuery::NbQuery() {}

// デストラクタ.
NbQuery::~NbQuery() {}

NbQuery &NbQuery::AddSimpleOp(const std::string &op, const std::string &key, int value) {
    return AddSimpleOpPrimitive(op, key, value);
}

NbQuery &NbQuery::AddSimpleOp(const std::string &op, const std::string &key, int64_t value) {
    return AddSimpleOpPrimitive(op, key, value);
}

NbQuery &NbQuery::AddSimpleOp(const std::string &op, const std::string &key, double value) {
    return AddSimpleOpPrimitive(op, key, value);
}

NbQuery &NbQuery::AddSimpleOp(const std::string &op, const std::string &key, bool value) {
    return AddSimpleOpPrimitive(op, key, value);
}

NbQuery &NbQuery::AddSimpleOp(const std::string &op, const std::string &key, const std::string &value) {
    return AddSimpleOpPrimitive(op, key, value);
}

NbQuery &NbQuery::AddSimpleOp(const std::string &op, const std::string &key, const NbJsonObject &value) {
    return AddSimpleOpJson(op, key, value);
}

NbQuery &NbQuery::AddSimpleOp(const std::string &op, const std::string &key, const NbJsonArray &value) {
    return AddSimpleOpJson(op, key, value);
}

NbQuery &NbQuery::In(const std::string &key, const NbJsonArray &array) {
    return AddSimpleOp("$in", key, array);
}

NbQuery &NbQuery::All(const std::string &key, const NbJsonArray &array) {
    return AddSimpleOp("$all", key, array);
}

NbQuery &NbQuery::Regex(const std::string &key, const std::string &regexp, const std::string &options) {
    if (!key.empty() && !regexp.empty()) {
        Json::Value json;
        json["$regex"] = regexp;

        if (!options.empty()) {
            json["$options"] = options;
        }
        conditions_[key] = json;
    }
    return *this;
}

NbQuery &NbQuery::Exists(const std::string &key) {
    return AddSimpleOp("$exists", key, true);
}

NbQuery &NbQuery::NotExists(const std::string &key) {
    return AddSimpleOp("$exists", key, false);
}

NbQuery &NbQuery::Not(const std::string &key) {
    if (!key.empty() && conditions_.isMember(key)) {
        Json::Value json;
        if (conditions_[key].isObject()) {
            json["$not"] = conditions_[key];
        } else {
            json["$not"]["$eq"] = conditions_[key];
        }
        conditions_[key] = json;
    }
    return *this;
}

NbQuery &NbQuery::Or(const std::vector<NbQuery> queries) {
    return ConcatQueries("$or", queries);
}

NbQuery &NbQuery::And(const std::vector<NbQuery> queries) {
    return ConcatQueries("$and", queries);
}

NbQuery &NbQuery::ConcatQueries(const string &op, vector<NbQuery> queries) {
    if (queries.empty()) {
        return *this;
    }

    Json::Value json;

    // 自インスタンスが空の場合はスキップ
    if (!conditions_.empty()) {
        json[0] = conditions_;
    }

    for (int i = 0; i < queries.size(); ++i) {
        json.append(queries[i].GetConditions());
    }

    conditions_.clear();
    conditions_[op] = json;

    return *this;
}

NbQuery &NbQuery::OrderBy(const std::vector<std::string> &order) {
    if (order.empty()) {
        return *this;
    }

    for (auto str : order) {
        if (str.empty()) {
            return *this;
        }
    }
    order_ = order;
    return *this;
}

NbQuery &NbQuery::Skip(int skip) {
    skip_ = skip;
    return *this;
}

NbQuery &NbQuery::Limit(int limit) {
    limit_ = limit;
    return *this;
}

NbQuery &NbQuery::DeleteMark(bool delete_mark) {
    delete_mark_ = delete_mark;
    return *this;
}

NbQuery &NbQuery::Projection(const map<string, bool> &projection) {
    if (projection.empty()) {
        return *this;
    }

    for (auto str : projection) {
        if (str.first.empty()) {
            return *this;
        }
    }
    projection_ = projection;
    return *this;
}

NbQuery &NbQuery::ReadPreference(NbReadPreference read_preference) {
    read_preference_ = read_preference;    
    return *this;
}

NbQuery &NbQuery::Timeout(int timeout) {
    timeout_ = timeout;
    return *this;
}

std::string NbQuery::GetConditionsString() const {
    if (conditions_.empty()) {
        return "";
    }
    Json::FastWriter writer;
    writer.omitEndingLineFeed(); // 行末の改行を省略
    return writer.write(conditions_);
}

std::string NbQuery::GetOrderString() const {
    std::string order_string{};

    for (auto order : order_) {
        order_string += order + ",";
    }
    
    if (!order_string.empty()) {
        order_string.erase(--order_string.end());
    }

    return order_string;
}

std::string NbQuery::GetSkipString() const {
    if (skip_ > 0) {
        return std::to_string(skip_);
    }
    return "";
}

std::string NbQuery::GetLimitString() const {

    if (limit_ >= -1 && limit_ < 100) {
        return std::to_string(limit_);
    }
    return "";
}

std::string NbQuery::GetDeleteMarkString() const {
    if (delete_mark_) {
        return "1";
    }
    return "";
}

std::string NbQuery::GetProjectionString() const {
    NbJsonObject object;
    for (auto projection : projection_) {
        object[projection.first] = projection.second ? 1 : 0;
    }

    if (object.IsEmpty()) {
        return "";
    }

    return object.ToJsonString();
}

std::string NbQuery::GetReadPreferenceString() const {
    switch (read_preference_) {
        case NbReadPreference::PRIMARY:
            return "";
        case NbReadPreference::SECONDARY_PREFERRED:
            return "secondaryPreferred";
    }
}

std::string NbQuery::GetTimeoutString() const {
    if (timeout_ > 0) {
        return std::to_string(timeout_);
    }
    return "";
}

const Json::Value &NbQuery::GetConditions() const {
    return conditions_;
}
} //namespace necbaas
