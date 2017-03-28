#include "gtest/gtest.h"
#include "necbaas/nb_json_object.h"
#include "necbaas/nb_json_array.h"

namespace necbaas {

using std::string;
using std::vector;

static const string kDefaultObject{R"({"arrayKey":[3333,"abcd",false],"boolKey":true,"doubleKey":1234.5678,"int64Key":12345678901,"intKey":123,"nullKey":null,"objectKey":{"obj1":2222,"obj2":"val","obj3":true},"stringKey":"stringValue"})"};
static const string kDefaultArray{R"([1234567,"abcdef",true,{"obj1":2222,"obj2":"val","obj3":true},[3333,"abcd",false],null])"};

//NbJsonObject デフォルトコンストラクタ
TEST(NbJsonObject, NbJsonObject) {
    NbJsonObject obj;
    EXPECT_TRUE(obj.IsEmpty());
    EXPECT_EQ(0, obj.GetSize());
    EXPECT_EQ(string("{}"), obj.ToJsonString());

    vector<string> key_set = obj.GetKeySet();
    EXPECT_TRUE(key_set.empty());
}

//NbJsonObject コンストラクタ(操作含む)
TEST(NbJsonObject, NbJsonObjectString) {
    NbJsonObject obj(kDefaultObject);
    EXPECT_FALSE(obj.IsEmpty());
    EXPECT_EQ(8, obj.GetSize());
    EXPECT_EQ(kDefaultObject, obj.ToJsonString());

    vector<string> key_set = obj.GetKeySet();
    EXPECT_EQ("arrayKey", key_set[0]);
    EXPECT_EQ("boolKey", key_set[1]);
    EXPECT_EQ("doubleKey", key_set[2]);
    EXPECT_EQ("int64Key", key_set[3]);
    EXPECT_EQ("intKey", key_set[4]);
    EXPECT_EQ("nullKey", key_set[5]);
    EXPECT_EQ("objectKey", key_set[6]);
    EXPECT_EQ("stringKey", key_set[7]);

    EXPECT_TRUE(obj.IsMember("arrayKey"));

    EXPECT_EQ(NbJsonType::NB_JSON_ARRAY, obj.GetType("arrayKey"));
    EXPECT_EQ(NbJsonType::NB_JSON_BOOLEAN, obj.GetType("boolKey"));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, obj.GetType("doubleKey"));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, obj.GetType("int64Key"));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, obj.GetType("intKey"));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, obj.GetType("nullKey"));
    EXPECT_EQ(NbJsonType::NB_JSON_OBJECT, obj.GetType("objectKey"));
    EXPECT_EQ(NbJsonType::NB_JSON_STRING, obj.GetType("stringKey"));

    obj.Remove("int64Key");
    EXPECT_EQ(7, obj.GetSize());
    EXPECT_FALSE(obj.IsMember("int64Key"));

    obj.Clear();
    EXPECT_EQ(0, obj.GetSize());
}

//NbJsonObject コンストラクタ(JSON文字列不正)
TEST(NbJsonObject, NbJsonObjectInvalidJson) {
    NbJsonObject obj(string("text message"));
    EXPECT_TRUE(obj.IsEmpty());
    EXPECT_EQ(string("{}"), obj.ToJsonString());
}

//NbJsonObject コンストラクタ(JSON Objectでない)
TEST(NbJsonObject, NbJsonObjectNotObject) {
    NbJsonObject obj(string(R"(["aaa","bbb"])"));
    EXPECT_TRUE(obj.IsEmpty());
    EXPECT_EQ(string("{}"), obj.ToJsonString());
}

//NbJsonObject PutAll
TEST(NbJsonObject, PutAll) {
    NbJsonObject obj;
    obj.PutAll(kDefaultObject);
    EXPECT_FALSE(obj.IsEmpty());
    EXPECT_EQ(8, obj.GetSize());
    EXPECT_EQ(kDefaultObject, obj.ToJsonString());
}

//NbJsonObject PutAll(JSON文字列不正)
TEST(NbJsonObject, PutAllInvalidJson) {
    NbJsonObject obj;
    obj.PutAll("text message");
    EXPECT_TRUE(obj.IsEmpty());
    EXPECT_EQ(string("{}"), obj.ToJsonString());
}

//NbJsonObject PutAll(JSON Objectでない)
TEST(NbJsonObject, PutAllNotObject) {
    NbJsonObject obj;
    obj.PutAll(string(R"(["aaa","bbb"])"));
    EXPECT_TRUE(obj.IsEmpty());
    EXPECT_EQ(string("{}"), obj.ToJsonString());
}

//NbJsonObject int
TEST(NbJsonObject, Int) {
    NbJsonObject obj;
    obj["testKey"] = std::numeric_limits<int>::max();
    EXPECT_EQ(std::numeric_limits<int>::max(), obj.GetInt("testKey"));
    EXPECT_EQ(std::numeric_limits<int>::max(), obj.GetInt64("testKey"));
    EXPECT_EQ(std::numeric_limits<int>::max(), obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());

    obj["testKey"] = std::numeric_limits<int>::lowest();
    EXPECT_EQ(std::numeric_limits<int>::lowest(), obj.GetInt("testKey"));
    EXPECT_EQ(std::numeric_limits<int>::lowest(), obj.GetInt64("testKey"));
    EXPECT_EQ(std::numeric_limits<int>::lowest(), obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());
}

//NbJsonObject unsigned int
TEST(NbJsonObject, UInt) {
    NbJsonObject obj;
    obj["testKey"] = std::numeric_limits<unsigned int>::max();
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), obj.GetInt64("testKey"));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());
}

//NbJsonObject int64_t
TEST(NbJsonObject, Int64) {
    NbJsonObject obj;
    obj["testKey"] = std::numeric_limits<int64_t>::max();
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(std::numeric_limits<int64_t>::max(), obj.GetInt64("testKey"));
    EXPECT_EQ(std::numeric_limits<int64_t>::max(), obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());

    obj["testKey"] = std::numeric_limits<int64_t>::lowest();
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(std::numeric_limits<int64_t>::lowest(), obj.GetInt64("testKey"));
    EXPECT_EQ(std::numeric_limits<int64_t>::lowest(), obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());

    int64_t int_max = std::numeric_limits<int>::max();
    obj["testKey"] = int_max;
    EXPECT_EQ(std::numeric_limits<int>::max(), obj.GetInt("testKey"));
}

//NbJsonObject uint64_t
TEST(NbJsonObject, UInt64) {
    NbJsonObject obj;
    obj["testKey"] = std::numeric_limits<uint64_t>::max();
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(0, obj.GetInt64("testKey"));
    EXPECT_EQ(std::numeric_limits<uint64_t>::max(), obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());
}

//NbJsonObject double
TEST(NbJsonObject, Double) {
    NbJsonObject obj;
    obj["testKey"] = std::numeric_limits<double>::max();
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(0, obj.GetInt64("testKey"));
    EXPECT_EQ(std::numeric_limits<double>::max(), obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());

    obj["testKey"] = std::numeric_limits<double>::lowest();
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(0, obj.GetInt64("testKey"));
    EXPECT_EQ(std::numeric_limits<double>::lowest(), obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());

    double int_max = std::numeric_limits<int>::max();
    obj["testKey"] = int_max;
    EXPECT_EQ(std::numeric_limits<int>::max(), obj.GetInt("testKey"));
    EXPECT_EQ(std::numeric_limits<int>::max(), obj.GetInt64("testKey"));
    EXPECT_EQ(std::numeric_limits<int>::max(), obj.GetDouble("testKey"));
}

//NbJsonObject Bool
TEST(NbJsonObject, Bool) {
    NbJsonObject obj;
    obj["testKey"] = true;
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(0, obj.GetInt64("testKey"));
    EXPECT_EQ(0, obj.GetDouble("testKey"));
    EXPECT_TRUE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());

    obj["testKey"] = false;
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(0, obj.GetInt64("testKey"));
    EXPECT_EQ(0, obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());
}

//NbJsonObject std::string
TEST(NbJsonObject, String) {
    NbJsonObject obj;
    obj["testKey"] = string("testValue");
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(0, obj.GetInt64("testKey"));
    EXPECT_EQ(0, obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string("testValue"), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());

    obj["testKey"] = string("");
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(0, obj.GetInt64("testKey"));
    EXPECT_EQ(0, obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());
}

//NbJsonObject JsonObject
TEST(NbJsonObject, JsonObject) {
    NbJsonObject obj;
    NbJsonObject testValue(kDefaultObject);
    obj.PutJsonObject("testKey", testValue);
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(0, obj.GetInt64("testKey"));
    EXPECT_EQ(0, obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_EQ(testValue, obj.GetJsonObject("testKey"));
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());

    testValue = NbJsonObject();
    obj.PutJsonObject("testKey", testValue);
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(0, obj.GetInt64("testKey"));
    EXPECT_EQ(0, obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_EQ(testValue, obj.GetJsonObject("testKey"));
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());
}

//NbJsonObject JsonArray
TEST(NbJsonObject, JsonArray) {
    NbJsonObject obj;
    NbJsonArray testValue(kDefaultArray);
    obj.PutJsonArray("testKey", testValue);
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(0, obj.GetInt64("testKey"));
    EXPECT_EQ(0, obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_EQ(testValue, obj.GetJsonArray("testKey"));

    testValue = NbJsonArray();
    obj.PutJsonArray("testKey", testValue);
    EXPECT_EQ(0, obj.GetInt("testKey"));
    EXPECT_EQ(0, obj.GetInt64("testKey"));
    EXPECT_EQ(0, obj.GetDouble("testKey"));
    EXPECT_FALSE(obj.GetBoolean("testKey"));
    EXPECT_EQ(string(""), obj.GetString("testKey"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_EQ(testValue, obj.GetJsonArray("testKey"));
}

//NbJsonObject KeyNone&defaltValue
TEST(NbJsonObject, KeyNone) {
    NbJsonObject obj;
    EXPECT_EQ(12345, obj.GetInt("testKey", 12345));
    EXPECT_EQ(98765, obj.GetInt64("testKey", 98765));
    EXPECT_EQ(123.456, obj.GetDouble("testKey", 123.456));
    EXPECT_TRUE(obj.GetBoolean("testKey", true));
    EXPECT_EQ(string("defaultValue"), obj.GetString("testKey", "defaultValue"));
    EXPECT_TRUE(obj.GetJsonObject("testKey").IsEmpty());
    EXPECT_TRUE(obj.GetJsonArray("testKey").IsEmpty());
}

//NbJsonObject const operator[]
TEST(NbJsonObject, ArraySubscripting) {
    const NbJsonObject obj(kDefaultObject);
    EXPECT_EQ(Json::Value::null, obj["stringKey"]);
}

//NbJsonObject PutJsonObject(キー空)
TEST(NbJsonObject, PutJsonObjectKeyEmpty) {
    NbJsonObject obj;
    NbJsonObject testValue(kDefaultObject);
    obj.PutJsonObject("", testValue);
    EXPECT_TRUE(obj.GetJsonObject("").IsEmpty());
}

//NbJsonObject PutJsonArray(キー空)
TEST(NbJsonObject, PutJsonArrayKeyEmpty) {
    NbJsonObject obj;
    NbJsonArray testValue(kDefaultArray);
    obj.PutJsonArray("", testValue);
    EXPECT_TRUE(obj.GetJsonArray("").IsEmpty());
}

//NbJsonObject PutNull
TEST(NbJsonObject, PutNull) {
    NbJsonObject obj;
    obj.PutNull("testKey");
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, obj.GetType("testKey"));

    obj.PutNull("");
    EXPECT_FALSE(obj.IsMember(""));
}

//NbJsonObject GetType(存在しないキー)
TEST(NbJsonObject, GetType) {
    NbJsonObject obj;
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, obj.GetType("testKey"));
}
} //namespace necbaas
