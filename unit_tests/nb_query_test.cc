#include "gtest/gtest.h"
#include "necbaas/nb_query.h"

namespace necbaas {

using std::string;

const int kIntValue = 0x12345678;
const int64_t kInt64Value = 0x123456789abcdef;
const double kDoubleValue = 12345.678;
const bool kBoolValue = true;
const string kStringValue = "stringValue";
const NbJsonObject kJsonObjectValue{string(R"({"key1":"value1","key2":123, "key3":false})")};
const NbJsonArray kJsonArrayValue{R"(["array1", 1234, false])"};

const string kIntKey = "intKey";
const string kInt64Key = "int64Key";
const string kDoubleKey = "doubleKey";
const string kBoolKey = "booKey";
const string kStringKey = "stringKey";
const string kJsonObjectKey = "jsonObjectKey";
const string kJsonArrayKey = "jsonArrayKey";

//NbQuery Conditions.EqualTo
TEST(NbQuery, EqualTo) {
    NbQuery query;
    query.EqualTo(kIntKey, kIntValue);
    query.EqualTo(kInt64Key, kInt64Value);
    query.EqualTo(kDoubleKey, kDoubleValue);
    query.EqualTo(kBoolKey, kBoolValue);
    query.EqualTo(kStringKey, kStringValue);
    query.EqualTo(kJsonObjectKey, kJsonObjectValue);
    query.EqualTo(kJsonArrayKey, kJsonArrayValue);
    
    Json::Value conditons = query.GetConditions();
    EXPECT_EQ(kIntValue, conditons[kIntKey].asInt());
    EXPECT_EQ(kInt64Value, conditons[kInt64Key].asInt64());
    EXPECT_EQ(kDoubleValue, conditons[kDoubleKey].asDouble());
    EXPECT_TRUE(conditons[kBoolKey].asBool());
    EXPECT_EQ(kStringValue, conditons[kStringKey].asString());
    EXPECT_EQ("value1", conditons[kJsonObjectKey]["$eq"]["key1"].asString());
    EXPECT_EQ(123, conditons[kJsonObjectKey]["$eq"]["key2"].asInt());
    EXPECT_FALSE(conditons[kJsonObjectKey]["$eq"]["key3"].asBool());
    EXPECT_EQ("array1", conditons[kJsonArrayKey][0].asString());
    EXPECT_EQ(1234, conditons[kJsonArrayKey][1].asInt());
    EXPECT_FALSE(conditons[kJsonArrayKey][2].asBool());
}

//NbQuery Conditions.EqualTo(キー空)
TEST(NbQuery, EqualToKeyEmpty) {
    NbQuery query;
    query.EqualTo(string(""), kIntValue);
    EXPECT_TRUE(query.GetConditions().empty());
    query.EqualTo(string(""), kInt64Value);
    EXPECT_TRUE(query.GetConditions().empty());
    query.EqualTo(string(""), kDoubleValue);
    EXPECT_TRUE(query.GetConditions().empty());
    query.EqualTo(string(""), kBoolValue);
    EXPECT_TRUE(query.GetConditions().empty());
    query.EqualTo(string(""), kStringValue);
    EXPECT_TRUE(query.GetConditions().empty());
    query.EqualTo(string(""), kJsonObjectValue);
    EXPECT_TRUE(query.GetConditions().empty());
    query.EqualTo(string(""), kJsonArrayValue);
    EXPECT_TRUE(query.GetConditions().empty());
}

void ConditionsCommon(const NbQuery &query, const string &op) {
    Json::Value conditons = query.GetConditions();
    EXPECT_EQ(kIntValue, conditons[kIntKey][op].asInt());
    EXPECT_EQ(kInt64Value, conditons[kInt64Key][op].asInt64());
    EXPECT_EQ(kDoubleValue, conditons[kDoubleKey][op].asDouble());
    EXPECT_TRUE(conditons[kBoolKey][op].asBool());
    EXPECT_EQ(kStringValue, conditons[kStringKey][op].asString());
    EXPECT_EQ("value1", conditons[kJsonObjectKey][op]["key1"].asString());
    EXPECT_EQ(123, conditons[kJsonObjectKey][op]["key2"].asInt());
    EXPECT_FALSE(conditons[kJsonObjectKey][op]["key3"].asBool());
    EXPECT_EQ("array1", conditons[kJsonArrayKey][op][0].asString());
    EXPECT_EQ(1234, conditons[kJsonArrayKey][op][1].asInt());
    EXPECT_FALSE(conditons[kJsonArrayKey][op][2].asBool());
}

//NbQuery Conditions.LessThan
TEST(NbQuery, LessThan) {
    NbQuery query;
    query.LessThan(kIntKey, kIntValue);
    query.LessThan(kInt64Key, kInt64Value);
    query.LessThan(kDoubleKey, kDoubleValue);
    query.LessThan(kBoolKey, kBoolValue);
    query.LessThan(kStringKey, kStringValue);
    query.LessThan(kJsonObjectKey, kJsonObjectValue);
    query.LessThan(kJsonArrayKey, kJsonArrayValue);

    ConditionsCommon(query, "$lt");
}

//NbQuery Conditions.LessThanOrEqual
TEST(NbQuery, LessThanOrEqual) {
    NbQuery query;
    query.LessThanOrEqual(kIntKey, kIntValue);
    query.LessThanOrEqual(kInt64Key, kInt64Value);
    query.LessThanOrEqual(kDoubleKey, kDoubleValue);
    query.LessThanOrEqual(kBoolKey, kBoolValue);
    query.LessThanOrEqual(kStringKey, kStringValue);
    query.LessThanOrEqual(kJsonObjectKey, kJsonObjectValue);
    query.LessThanOrEqual(kJsonArrayKey, kJsonArrayValue);

    ConditionsCommon(query, "$lte");
}

//NbQuery Conditions.GreaterThan
TEST(NbQuery, GreaterThan) {
    NbQuery query;
    query.GreaterThan(kIntKey, kIntValue);
    query.GreaterThan(kInt64Key, kInt64Value);
    query.GreaterThan(kDoubleKey, kDoubleValue);
    query.GreaterThan(kBoolKey, kBoolValue);
    query.GreaterThan(kStringKey, kStringValue);
    query.GreaterThan(kJsonObjectKey, kJsonObjectValue);
    query.GreaterThan(kJsonArrayKey, kJsonArrayValue);

    ConditionsCommon(query, "$gt");
}

//NbQuery Conditions.GreaterThanOrEqual
TEST(NbQuery, GreaterThanOrEqual) {
    NbQuery query;
    query.GreaterThanOrEqual(kIntKey, kIntValue);
    query.GreaterThanOrEqual(kInt64Key, kInt64Value);
    query.GreaterThanOrEqual(kDoubleKey, kDoubleValue);
    query.GreaterThanOrEqual(kBoolKey, kBoolValue);
    query.GreaterThanOrEqual(kStringKey, kStringValue);
    query.GreaterThanOrEqual(kJsonObjectKey, kJsonObjectValue);
    query.GreaterThanOrEqual(kJsonArrayKey, kJsonArrayValue);

    ConditionsCommon(query, "$gte");
}

//NbQuery Conditions.NotEquals
TEST(NbQuery, NotEquals) {
    NbQuery query;
    query.NotEquals(kIntKey, kIntValue);
    query.NotEquals(kInt64Key, kInt64Value);
    query.NotEquals(kDoubleKey, kDoubleValue);
    query.NotEquals(kBoolKey, kBoolValue);
    query.NotEquals(kStringKey, kStringValue);
    query.NotEquals(kJsonObjectKey, kJsonObjectValue);
    query.NotEquals(kJsonArrayKey, kJsonArrayValue);

    ConditionsCommon(query, "$ne");
}

//NbQuery Conditions.In
TEST(NbQuery, In) {
    NbQuery query;
    query.In(kJsonArrayKey, kJsonArrayValue);

    Json::Value conditons = query.GetConditions();
    string op = "$in";
    EXPECT_EQ("array1", conditons[kJsonArrayKey][op][0].asString());
    EXPECT_EQ(1234, conditons[kJsonArrayKey][op][1].asInt());
    EXPECT_FALSE(conditons[kJsonArrayKey][op][2].asBool());
}

//NbQuery Conditions.All
TEST(NbQuery, All) {
    NbQuery query;
    query.All(kJsonArrayKey, kJsonArrayValue);

    Json::Value conditons = query.GetConditions();
    string op = "$all";
    EXPECT_EQ("array1", conditons[kJsonArrayKey][op][0].asString());
    EXPECT_EQ(1234, conditons[kJsonArrayKey][op][1].asInt());
    EXPECT_FALSE(conditons[kJsonArrayKey][op][2].asBool());
}

//NbQuery Conditions.Regex
TEST(NbQuery, Regex) {
    NbQuery query;
    const string kRegexKey = "regexKey";
    const string kRegexp = "^ABC$";
    const string kRegexOption = "imxs";

    //Keyが空
    query.Regex(string(""), kRegexp, kRegexOption);
    Json::Value conditons = query.GetConditions();
    EXPECT_TRUE(conditons.empty());

    //正規表現が空
    query.Regex(kRegexKey, string(""), kRegexOption);
    conditons = query.GetConditions();
    EXPECT_TRUE(conditons.empty());

    //正常登録
    query.Regex(kRegexKey, kRegexp, kRegexOption);
    conditons = query.GetConditions();
    EXPECT_EQ(kRegexp, conditons[kRegexKey]["$regex"].asString());
    EXPECT_EQ(kRegexOption, conditons[kRegexKey]["$options"].asString());

    //オプションなし
    NbQuery query2;
    query2.Regex(kRegexKey, kRegexp);
    conditons = query2.GetConditions();
    EXPECT_EQ(kRegexp, conditons[kRegexKey]["$regex"].asString());
    EXPECT_FALSE(conditons[kRegexKey]["$options"]);
}

//NbQuery Conditions.Exists
TEST(NbQuery, Exists) {
    NbQuery query;
    query.Exists(kBoolKey);

    Json::Value conditons = query.GetConditions();
    string op = "$exists";
    EXPECT_TRUE(conditons[kBoolKey][op].asBool());
}

//NbQuery Conditions.NotExists
TEST(NbQuery, NotExists) {
    NbQuery query;
    query.NotExists(kBoolKey);

    Json::Value conditons = query.GetConditions();
    string op = "$exists";
    EXPECT_FALSE(conditons[kBoolKey][op].asBool());
}

//NbQuery Conditions.Not
TEST(NbQuery, Not) {
    NbQuery query;

    query.EqualTo(kIntKey, kIntValue);
    query.Not(kIntKey);
    Json::Value conditons = query.GetConditions();
    EXPECT_EQ(kIntValue ,conditons[kIntKey]["$not"]["$eq"].asInt());

    query.EqualTo(kJsonObjectKey, kJsonObjectValue);
    query.Not(kJsonObjectKey);
    conditons = query.GetConditions();
    EXPECT_EQ("value1", conditons[kJsonObjectKey]["$not"]["$eq"]["key1"].asString());
    EXPECT_EQ(123, conditons[kJsonObjectKey]["$not"]["$eq"]["key2"].asInt());
    EXPECT_FALSE(conditons[kJsonObjectKey]["$not"]["$eq"]["key3"].asBool());

    query.EqualTo(kJsonArrayKey, kJsonArrayValue);
    query.Not(kJsonArrayKey);
    conditons = query.GetConditions();
    EXPECT_EQ("array1", conditons[kJsonArrayKey]["$not"]["$eq"][0].asString());
    EXPECT_EQ(1234, conditons[kJsonArrayKey]["$not"]["$eq"][1].asInt());
    EXPECT_FALSE(conditons[kJsonArrayKey]["$not"]["$eq"][2].asBool());

    query.LessThan(kStringKey, kStringValue);
    query.Not(kStringKey);
    conditons = query.GetConditions();
    EXPECT_EQ(kStringValue ,conditons[kStringKey]["$not"]["$lt"].asString());
}

//NbQuery Conditions.Or
TEST(NbQuery, Or) {
    NbQuery query;
    query.EqualTo(kIntKey, kIntValue);

    std::vector<NbQuery> queries;
    NbQuery query2;
    query2.LessThan(kDoubleKey, kDoubleValue);
    queries.push_back(query2);
    query2.GreaterThan(kStringKey, kStringValue);
    queries.push_back(query2);
    query2.NotEquals(kBoolKey, kBoolValue);
    queries.push_back(query2);

    query.Or(queries);
    Json::Value conditons = query.GetConditions();
    EXPECT_EQ(kIntValue, conditons["$or"][0][kIntKey].asInt());
    EXPECT_EQ(kDoubleValue, conditons["$or"][1][kDoubleKey]["$lt"].asDouble());
    EXPECT_EQ(kStringValue, conditons["$or"][2][kStringKey]["$gt"].asString());
    EXPECT_TRUE(conditons["$or"][3][kBoolKey]["$ne"].asBool());

    NbQuery empty_query;
    empty_query.Or(queries);
    conditons = empty_query.GetConditions();
    EXPECT_EQ(kDoubleValue, conditons["$or"][0][kDoubleKey]["$lt"].asDouble());
    EXPECT_EQ(kStringValue, conditons["$or"][1][kStringKey]["$gt"].asString());
    EXPECT_TRUE(conditons["$or"][2][kBoolKey]["$ne"].asBool());
}

//NbQuery Conditions.Or(queries空)
TEST(NbQuery, OrEmpty) {
    NbQuery query;
    query.EqualTo(kIntKey, kIntValue);
    std::vector<NbQuery> queries;
    query.Or(queries);
    Json::Value conditons = query.GetConditions();
    EXPECT_EQ(kIntValue, conditons[kIntKey].asInt());
}

//NbQuery Conditions.And
TEST(NbQuery, And) {
    NbQuery query;
    query.EqualTo(kIntKey, kIntValue);

    std::vector<NbQuery> queries;
    NbQuery query2;
    query2.LessThan(kDoubleKey, kDoubleValue);
    queries.push_back(query2);
    query2.GreaterThan(kStringKey, kStringValue);
    queries.push_back(query2);
    query2.NotEquals(kBoolKey, kBoolValue);
    queries.push_back(query2);

    query.And(queries);
    Json::Value conditons = query.GetConditions();
    EXPECT_EQ(kIntValue, conditons["$and"][0][kIntKey].asInt());
    EXPECT_EQ(kDoubleValue, conditons["$and"][1][kDoubleKey]["$lt"].asDouble());
    EXPECT_EQ(kStringValue, conditons["$and"][2][kStringKey]["$gt"].asString());
    EXPECT_TRUE(conditons["$and"][3][kBoolKey]["$ne"].asBool());

    NbQuery empty_query;
    empty_query.And(queries);
    conditons = empty_query.GetConditions();
    EXPECT_EQ(kDoubleValue, conditons["$and"][0][kDoubleKey]["$lt"].asDouble());
    EXPECT_EQ(kStringValue, conditons["$and"][1][kStringKey]["$gt"].asString());
    EXPECT_TRUE(conditons["$and"][2][kBoolKey]["$ne"].asBool());
}

//NbQuery Conditions.And(queries空)
TEST(NbQuery, AndEmpty) {
    NbQuery query;
    query.EqualTo(kIntKey, kIntValue);
    std::vector<NbQuery> queries;
    query.And(queries);
    Json::Value conditons = query.GetConditions();
    EXPECT_EQ(kIntValue, conditons[kIntKey].asInt());
}

//NbQuery OrderBy
TEST(NbQuery, OrderBy) {
    //３つ登録
    NbQuery query;
    std::vector<std::string> order{"order1", "order2", "-order3"};
    query.OrderBy(order);
    EXPECT_EQ("order1,order2,-order3", query.GetOrderString());

    //空
    query = NbQuery();
    order = std::vector<std::string>();
    query.OrderBy(order);
    EXPECT_EQ(string(""), query.GetOrderString());

    //空文字あり
    query = NbQuery();
    order = std::vector<std::string>{"order1", "", "-order3"};
    query.OrderBy(order);
    EXPECT_EQ(string(""), query.GetOrderString());
}

//NbQuery Skip
TEST(NbQuery, Skip) {
    //初期値(0)
    NbQuery query;
    EXPECT_EQ(string(""), query.GetSkipString());

    //0(デフォルト値)
    query.Skip(0);
    EXPECT_EQ(string(""), query.GetSkipString());

    //1最小値
    query.Skip(1);
    EXPECT_EQ(string("1"), query.GetSkipString());
}

//NbQuery Limit
TEST(NbQuery, Limit) {
    //初期値(100)
    NbQuery query;
    EXPECT_EQ(string(""), query.GetLimitString());

    //100(デフォルト値)
    query.Limit(100);
    EXPECT_EQ(string(""), query.GetLimitString());

    //99(最大値)
    query.Limit(99);
    EXPECT_EQ(string("99"), query.GetLimitString());

    //-1(最小値)
    query.Limit(-1);
    EXPECT_EQ(string("-1"), query.GetLimitString());

    //-2(範囲外)
    query.Limit(-2);
    EXPECT_EQ(string(""), query.GetLimitString());
}

//NbQuery DeleteMark
TEST(NbQuery, DeleteMark) {
    //初期値(false)
    NbQuery query;
    EXPECT_EQ(string(""), query.GetDeleteMarkString());

    //true
    query.DeleteMark(true);
    EXPECT_EQ(string("1"), query.GetDeleteMarkString());

    //false
    query.DeleteMark(false);
    EXPECT_EQ(string(""), query.GetDeleteMarkString());
}

//NbQuery Projection
TEST(NbQuery, Projection) {
    NbQuery query;
    std::map<string, bool> projection;

    //空
    query.Projection(projection);
    EXPECT_EQ(string(""), query.GetProjectionString());

    //正常
    projection["projection1"] = true;
    projection["projection2"] = false;
    projection["projection3"] = true;
    query.Projection(projection);
    EXPECT_EQ(string(R"({"projection1":1,"projection2":0,"projection3":1})"), query.GetProjectionString());

    // mapの中に空文字あり
    query = NbQuery();
    projection[""] = false;
    query.Projection(projection);
    EXPECT_EQ(string(""), query.GetProjectionString());
}

//NbQuery ReadPreference
TEST(NbQuery, ReadPreference) {
    //初期値(PRIMARY)
    NbQuery query;
    EXPECT_EQ(string(""), query.GetReadPreferenceString());

    //SECONDARY_PREFERRED
    query.ReadPreference(NbReadPreference::SECONDARY_PREFERRED);
    EXPECT_EQ(string("secondaryPreferred"), query.GetReadPreferenceString());

    //PRIMARY
    query.ReadPreference(NbReadPreference::PRIMARY);
    EXPECT_EQ(string(""), query.GetReadPreferenceString());
}


//NbQuery Timeout
TEST(NbQuery, Timeout) {
    //初期値(0)
    NbQuery query;
    EXPECT_EQ(string(""), query.GetTimeoutString());

    //最小値(1)
    query.Timeout(1);
    EXPECT_EQ(string("1"), query.GetTimeoutString());

    //デフォルト値(0)
    query.Timeout(0);
    EXPECT_EQ(string(""), query.GetTimeoutString());
}

//NbQuery GetConditionsString
TEST(NbQuery, GetConditionsString) {
    //Conditions設定なし
    NbQuery query;
    EXPECT_EQ(string(""), query.GetConditionsString());

    //Conditions設定あり
    query.EqualTo(string("key1"), string("value1"));
    EXPECT_EQ(string(R"({"key1":"value1"})"), query.GetConditionsString());
}


} //namespace necbaas
