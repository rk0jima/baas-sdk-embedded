#include "gtest/gtest.h"
#include "necbaas/nb_json_object.h"
#include "necbaas/nb_json_array.h"

namespace necbaas {

using std::string;
using std::vector;
using std::list;

static const string kDefaultObject{R"({"arrayKey":[3333,"abcd",false],"boolKey":true,"doubleKey":1234.5678,"int64Key":12345678901,"intKey":123,"nullKey":null,"objectKey":{"obj1":2222,"obj2":"val","obj3":true},"stringKey":"stringValue"})"};
static const string kDefaultArray{R"([1234567,"abcdef",true,{"obj1":2222,"obj2":"val","obj3":true},[3333,"abcd",false],null])"};
static const string kDefaultArray2{R"([1234567,1229782938247303441,123.456,1.23e+20,"abcdef",true,{"obj1":2222,"obj2":"val","obj3":true},[3333,"abcd",false],null])"};

//NbJsonArray デフォルトコンストラクタ
TEST(NbJsonArray, NbJsonArray) {
    NbJsonArray array;
    EXPECT_TRUE(array.IsEmpty());
    EXPECT_EQ(0, array.GetSize());
    EXPECT_EQ(string("[]"), array.ToJsonString());
}

//NbJsonArray コンストラクタ(操作含む)
TEST(NbJsonArray, NbJsonArrayString) {
    NbJsonArray array(kDefaultArray);
    EXPECT_FALSE(array.IsEmpty());
    EXPECT_EQ(6, array.GetSize());
    EXPECT_EQ(kDefaultArray, array.ToJsonString());

    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, array.GetType(0));
    EXPECT_EQ(NbJsonType::NB_JSON_STRING, array.GetType(1));
    EXPECT_EQ(NbJsonType::NB_JSON_BOOLEAN, array.GetType(2));
    EXPECT_EQ(NbJsonType::NB_JSON_OBJECT, array.GetType(3));
    EXPECT_EQ(NbJsonType::NB_JSON_ARRAY, array.GetType(4));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, array.GetType(5));

    array.Remove(3);
    EXPECT_EQ(5, array.GetSize());
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, array.GetType(0));
    EXPECT_EQ(NbJsonType::NB_JSON_STRING, array.GetType(1));
    EXPECT_EQ(NbJsonType::NB_JSON_BOOLEAN, array.GetType(2));
    EXPECT_EQ(NbJsonType::NB_JSON_ARRAY, array.GetType(3));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, array.GetType(4));

    array.Clear();
    EXPECT_EQ(0, array.GetSize());
}

//NbJsonArray コンストラクタ(JSON文字列不正)
TEST(NbJsonArray, NbJsonArrayInvalidJson) {
    NbJsonArray array(string("text message"));
    EXPECT_TRUE(array.IsEmpty());
    EXPECT_EQ(string("[]"), array.ToJsonString());
}

//NbJsonArray コンストラクタ(JSON Arrayでない)
TEST(NbJsonArray, NbJsonArrayNotArray) {
    NbJsonArray array(string(R"({"aaa":"bbb"})"));
    EXPECT_TRUE(array.IsEmpty());
    EXPECT_EQ(string("[]"), array.ToJsonString());
}

//NbJsonArray PutAll
TEST(NbJsonArray, PutAll) {
    NbJsonArray array;
    array.PutAll(kDefaultArray);
    EXPECT_FALSE(array.IsEmpty());
    EXPECT_EQ(6, array.GetSize());
    EXPECT_EQ(kDefaultArray, array.ToJsonString());
}

//NbJsonArray PutAll(JSON文字列不正)
TEST(NbJsonArray, PutAllInvalidJson) {
    NbJsonArray array;
    array.PutAll("text message");
    EXPECT_TRUE(array.IsEmpty());
    EXPECT_EQ(string("[]"), array.ToJsonString());
}

//NbJsonArray PutAll(JSON Arrayでない)
TEST(NbJsonArray, PutAllNotArray) {
    NbJsonArray array;
    array.PutAll(string(R"({"aaa":"bbb"})"));
    EXPECT_TRUE(array.IsEmpty());
    EXPECT_EQ(string("[]"), array.ToJsonString());
}

//NbJsonArray int
TEST(NbJsonArray, Int) {
    NbJsonArray array;
    EXPECT_TRUE(array.Append(std::numeric_limits<int>::max()));
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(std::numeric_limits<int>::max(), array.GetInt(0));
    EXPECT_EQ(std::numeric_limits<int>::max(), array.GetInt64(0));
    EXPECT_EQ(std::numeric_limits<int>::max(), array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    array[0] = std::numeric_limits<int>::lowest();
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(std::numeric_limits<int>::lowest(), array.GetInt(0));
    EXPECT_EQ(std::numeric_limits<int>::lowest(), array.GetInt64(0));
    EXPECT_EQ(std::numeric_limits<int>::lowest(), array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    int test_val = 1234;
    EXPECT_TRUE(array.Append(test_val));
    EXPECT_EQ(2, array.GetSize());
    EXPECT_EQ(test_val, array.GetInt(1));

    //Index最大値
    array[std::numeric_limits<unsigned int>::max()] = 0;
    EXPECT_EQ(2, array.GetSize());

    array[std::numeric_limits<unsigned int>::max() - 1] = 0;
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
    EXPECT_FALSE(array.Append(test_val));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
}

//NbJsonArray unsigned int
TEST(NbJsonArray, UInt) {
    NbJsonArray array;
    EXPECT_TRUE(array.Append(std::numeric_limits<unsigned int>::max()));
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetInt64(0));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    array[0] = std::numeric_limits<unsigned int>::lowest() + 1;
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(std::numeric_limits<unsigned int>::lowest() + 1, array.GetInt(0));
    EXPECT_EQ(std::numeric_limits<unsigned int>::lowest() + 1, array.GetInt64(0));
    EXPECT_EQ(std::numeric_limits<unsigned int>::lowest() + 1, array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    unsigned int test_val = 1234;
    EXPECT_TRUE(array.Append(test_val));
    EXPECT_EQ(2, array.GetSize());
    EXPECT_EQ(test_val, array.GetInt(1));

    //Index最大値
    array[std::numeric_limits<unsigned int>::max() - 1] = 0;
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
    EXPECT_FALSE(array.Append(test_val));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
}

//NbJsonArray int64_t
TEST(NbJsonArray, Int64) {
    NbJsonArray array;
    EXPECT_TRUE(array.Append(std::numeric_limits<int64_t>::max()));
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(std::numeric_limits<int64_t>::max(), array.GetInt64(0));
    EXPECT_EQ(std::numeric_limits<int64_t>::max(), array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    array[0] = std::numeric_limits<int64_t>::lowest();
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(std::numeric_limits<int64_t>::lowest(), array.GetInt64(0));
    EXPECT_EQ(std::numeric_limits<int64_t>::lowest(), array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    int64_t int_max = std::numeric_limits<int>::max();
    array[0] = int_max;
    EXPECT_EQ(std::numeric_limits<int>::max(), array.GetInt(0));

    int64_t test_val = 1234;
    EXPECT_TRUE(array.Append(test_val));
    EXPECT_EQ(2, array.GetSize());
    EXPECT_EQ(test_val, array.GetInt(1));

    //Index最大値
    array[std::numeric_limits<unsigned int>::max() - 1] = 0;
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
    EXPECT_FALSE(array.Append(test_val));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
}

//NbJsonArray uint64_t
TEST(NbJsonArray, UInt64) {
    NbJsonArray array;
    EXPECT_TRUE(array.Append(std::numeric_limits<uint64_t>::max()));
    EXPECT_EQ(1, array.GetSize());
    array[0] = std::numeric_limits<uint64_t>::max();
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(0, array.GetInt64(0));
    EXPECT_EQ(std::numeric_limits<uint64_t>::max(), array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    array[0] = std::numeric_limits<uint64_t>::lowest() + 1;
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(std::numeric_limits<uint64_t>::lowest() + 1, array.GetInt(0));
    EXPECT_EQ(std::numeric_limits<uint64_t>::lowest() + 1, array.GetInt64(0));
    EXPECT_EQ(std::numeric_limits<uint64_t>::lowest() + 1, array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    uint64_t test_val = 1234;
    EXPECT_TRUE(array.Append(test_val));
    EXPECT_EQ(2, array.GetSize());
    EXPECT_EQ(test_val, array.GetInt(1));

    //Index最大値
    array[std::numeric_limits<unsigned int>::max() - 1] = 0;
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
    EXPECT_FALSE(array.Append(test_val));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
}

//NbJsonArray double
TEST(NbJsonArray, Double) {
    NbJsonArray array;
    EXPECT_TRUE(array.Append(std::numeric_limits<double>::max()));
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(0, array.GetInt64(0));
    EXPECT_EQ(std::numeric_limits<double>::max(), array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    array[0] = std::numeric_limits<double>::lowest();
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(0, array.GetInt64(0));
    EXPECT_EQ(std::numeric_limits<double>::lowest(), array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    double int_max = std::numeric_limits<int>::max();
    array[0] = int_max;
    EXPECT_EQ(std::numeric_limits<int>::max(), array.GetInt(0));
    EXPECT_EQ(std::numeric_limits<int>::max(), array.GetInt64(0));
    EXPECT_EQ(std::numeric_limits<int>::max(), array.GetDouble(0));

    double test_val = 1234.56;
    EXPECT_TRUE(array.Append(test_val));
    EXPECT_EQ(2, array.GetSize());
    EXPECT_EQ(1234, array.GetInt(1));
    EXPECT_EQ(1234, array.GetInt64(1));
    EXPECT_EQ(test_val, array.GetDouble(1));

    //Index最大値
    array[std::numeric_limits<unsigned int>::max() - 1] = 0;
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
    EXPECT_FALSE(array.Append(test_val));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
}

//NbJsonArray Bool
TEST(NbJsonArray, Bool) {
    NbJsonArray array;
    EXPECT_TRUE(array.Append(true));
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(0, array.GetInt64(0));
    EXPECT_EQ(0, array.GetDouble(0));
    EXPECT_TRUE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    array[0] = false;
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(0, array.GetInt64(0));
    EXPECT_EQ(0, array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    bool test_val = true;
    EXPECT_TRUE(array.Append(test_val));
    EXPECT_EQ(2, array.GetSize());
    EXPECT_TRUE(array.GetBoolean(1));

    //Index最大値
    array[std::numeric_limits<unsigned int>::max() - 1] = 0;
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
    EXPECT_FALSE(array.Append(test_val));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
}

//NbJsonArray std::string
TEST(NbJsonArray, String) {
    NbJsonArray array;
    EXPECT_TRUE(array.Append(string("testValue")));
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(0, array.GetInt64(0));
    EXPECT_EQ(0, array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string("testValue"), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    array[0] = string("");
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(0, array.GetInt64(0));
    EXPECT_EQ(0, array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    string test_val = "testValue2";
    EXPECT_TRUE(array.Append(test_val));
    EXPECT_EQ(2, array.GetSize());
    EXPECT_EQ(test_val, array.GetString(1));

    //Index最大値
    array[std::numeric_limits<unsigned int>::max() - 1] = 0;
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
    EXPECT_FALSE(array.Append(test_val));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
}

//NbJsonArray JsonObject
TEST(NbJsonArray, JsonObject) {
    NbJsonArray array;
    NbJsonObject testValue(kDefaultObject);
    EXPECT_TRUE(array.AppendJsonObject(testValue));
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(0, array.GetInt64(0));
    EXPECT_EQ(0, array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_EQ(kDefaultObject, array.GetJsonObject(0).ToJsonString());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    testValue = NbJsonObject();
    EXPECT_TRUE(array.PutJsonObject(0, testValue));
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(0, array.GetInt64(0));
    EXPECT_EQ(0, array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_EQ(string("{}"), array.GetJsonObject(0).ToJsonString());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    testValue.PutAll(kDefaultObject);
    EXPECT_TRUE(array.AppendJsonObject(testValue));
    EXPECT_EQ(2, array.GetSize());
    EXPECT_EQ(kDefaultObject, array.GetJsonObject(1).ToJsonString());

    //Index最大値
    array[std::numeric_limits<unsigned int>::max() - 1] = 0;
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
    EXPECT_FALSE(array.AppendJsonObject(testValue));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
}

//NbJsonArray JsonArray
TEST(NbJsonArray, JsonArray) {
    NbJsonArray array;
    NbJsonArray testValue(kDefaultArray);
    EXPECT_TRUE(array.AppendJsonArray(testValue));
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(0, array.GetInt64(0));
    EXPECT_EQ(0, array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_EQ(kDefaultArray, array.GetJsonArray(0).ToJsonString());

    testValue = NbJsonArray();
    EXPECT_TRUE(array.PutJsonArray(0, testValue));
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(0, array.GetInt64(0));
    EXPECT_EQ(0, array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_EQ("[]", array.GetJsonArray(0).ToJsonString());

    testValue.PutAll(kDefaultArray);
    EXPECT_TRUE(array.AppendJsonArray(testValue));
    EXPECT_EQ(2, array.GetSize());
    EXPECT_EQ(kDefaultArray, array.GetJsonArray(1).ToJsonString());

    //Index最大値
    array[std::numeric_limits<unsigned int>::max() - 1] = 0;
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
    EXPECT_FALSE(array.AppendJsonArray(testValue));
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
}

//NbJsonArray Null
TEST(NbJsonArray, Null) {
    NbJsonArray array;
    EXPECT_TRUE(array.AppendNull());
    EXPECT_EQ(1, array.GetSize());
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, array.GetType(0));
    EXPECT_EQ(0, array.GetInt(0));
    EXPECT_EQ(0, array.GetInt64(0));
    EXPECT_EQ(0, array.GetDouble(0));
    EXPECT_FALSE(array.GetBoolean(0));
    EXPECT_EQ(string(""), array.GetString(0));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());

    EXPECT_TRUE(array.AppendNull());
    EXPECT_EQ(2, array.GetSize());
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, array.GetType(1));

    //Index最大値
    array[std::numeric_limits<unsigned int>::max() - 1] = 0;
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
    EXPECT_FALSE(array.AppendNull());
    EXPECT_EQ(std::numeric_limits<unsigned int>::max(), array.GetSize());
}

//NbJsonArray KeyNone&defaltValue
TEST(NbJsonArray, KeyNone) {
    NbJsonArray array;
    EXPECT_EQ(12345, array.GetInt(0, 12345));
    EXPECT_EQ(98765, array.GetInt64(0, 98765));
    EXPECT_EQ(123.456, array.GetDouble(0, 123.456));
    EXPECT_TRUE(array.GetBoolean(0, true));
    EXPECT_EQ(string("defaultValue"), array.GetString(0, "defaultValue"));
    EXPECT_TRUE(array.GetJsonObject(0).IsEmpty());
    EXPECT_TRUE(array.GetJsonArray(0).IsEmpty());
}

//NbJsonArray const operator[]
TEST(NbJsonArray, ArraySubscripting) {
    const NbJsonArray array(kDefaultArray);
    EXPECT_EQ(Json::Value::null, array[0]);
}

//NbJsonArray PutJsonObject(Index最大)
TEST(NbJsonArray, PutJsonObjectKeyEmpty) {
    NbJsonArray array;
    NbJsonObject testValue(kDefaultObject);
    EXPECT_FALSE(array.PutJsonObject(std::numeric_limits<unsigned int>::max(), testValue));
}

//NbJsonArray PutJsonArray(Index最大)
TEST(NbJsonArray, PutJsonArrayKeyEmpty) {
    NbJsonArray array;
    NbJsonArray testValue(kDefaultArray);
    EXPECT_FALSE(array.PutJsonArray(std::numeric_limits<unsigned int>::max(), testValue));
}

//NbJsonArray PutNull
TEST(NbJsonArray, PutNull) {
    NbJsonArray array;
    EXPECT_TRUE(array.PutNull(2));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, array.GetType(0));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, array.GetType(1));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, array.GetType(2));

    EXPECT_FALSE(array.PutNull(std::numeric_limits<unsigned int>::max()));
}

//NbJsonArray GetType(存在しないキー)
TEST(NbJsonArray, GetType) {
    NbJsonArray array;
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, array.GetType(0));
}


//NbJsonArray リスト一括操作(Int)
TEST(NbJsonArray, ListInt) {
    NbJsonArray array;
    //IntでPut、IntでGet
    vector<int> vec{std::numeric_limits<int>::lowest(),1,2,3,4,5,6,7,8,std::numeric_limits<int>::max()};
    list<int> lis{std::numeric_limits<int>::max(),8,7,6,5,4,3,2,1,std::numeric_limits<int>::lowest()};
    
    array.PutAllList(vec);
    vector<int> out_vec;
    EXPECT_EQ(vec.size(), array.GetSize());
    array.GetAllInt(&out_vec);
    EXPECT_EQ(vec, out_vec);

    array.PutAllList(lis);
    EXPECT_EQ(vec.size(), array.GetSize());
    list<int> out_lis;
    array.GetAllInt(&out_lis);
    EXPECT_EQ(lis, out_lis);

    //Int以外を含む
    array.PutAll(kDefaultArray2);
    array.GetAllInt(&out_vec);
    EXPECT_EQ(1234567, out_vec[0]);
    EXPECT_EQ(0, out_vec[1]);
    EXPECT_EQ(123, out_vec[2]);
    EXPECT_EQ(0, out_vec[3]);
    EXPECT_EQ(0, out_vec[4]);
    EXPECT_EQ(0, out_vec[5]);
    EXPECT_EQ(0, out_vec[6]);
    EXPECT_EQ(0, out_vec[7]);
    EXPECT_EQ(0, out_vec[8]);
}

//NbJsonArray リスト一括操作(Int64)
TEST(NbJsonArray, ListInt64) {
    NbJsonArray array;
    //Int64でPut、Int64でGet
    vector<int64_t> vec{std::numeric_limits<int64_t>::lowest(),1,2,3,4,5,6,7,8,std::numeric_limits<int64_t>::max()};
    list<int64_t> lis{std::numeric_limits<int64_t>::max(),8,7,6,5,4,3,2,1,std::numeric_limits<int64_t>::lowest()};

    array.PutAllList(vec);
    vector<int64_t> out_vec;
    EXPECT_EQ(vec.size(), array.GetSize());
    array.GetAllInt64(&out_vec);
    EXPECT_EQ(vec, out_vec);

    array.PutAllList(lis);
    EXPECT_EQ(vec.size(), array.GetSize());
    list<int64_t> out_lis;
    array.GetAllInt64(&out_lis);
    EXPECT_EQ(lis, out_lis);

    //UIntでPut、Int64でGet
    vector<unsigned int> vec_uint{std::numeric_limits<unsigned int>::lowest(),1,2,3,4,5,6,7,8,std::numeric_limits<unsigned int>::max()};
    list<unsigned int> lis_uint{std::numeric_limits<unsigned int>::max(),8,7,6,5,4,3,2,1,std::numeric_limits<unsigned int>::lowest()};

    array.PutAllList(vec_uint);
    EXPECT_EQ(vec.size(), array.GetSize());
    array.GetAllInt64(&out_vec);
    vec = vector<int64_t>{std::numeric_limits<unsigned int>::lowest(),1,2,3,4,5,6,7,8,std::numeric_limits<unsigned int>::max()};
    EXPECT_EQ(vec, out_vec);

    array.PutAllList(lis_uint);
    EXPECT_EQ(vec.size(), array.GetSize());
    array.GetAllInt64(&out_lis);
    lis = list<int64_t>{std::numeric_limits<unsigned int>::max(),8,7,6,5,4,3,2,1,std::numeric_limits<unsigned int>::lowest()};
    EXPECT_EQ(lis, out_lis);

    //Int64以外を含む
    array.PutAll(kDefaultArray2);
    array.GetAllInt64(&out_vec);
    EXPECT_EQ(1234567, out_vec[0]);
    EXPECT_EQ(1229782938247303441, out_vec[1]);
    EXPECT_EQ(123, out_vec[2]);
    EXPECT_EQ(0, out_vec[3]);
    EXPECT_EQ(0, out_vec[4]);
    EXPECT_EQ(0, out_vec[5]);
    EXPECT_EQ(0, out_vec[6]);
    EXPECT_EQ(0, out_vec[7]);
    EXPECT_EQ(0, out_vec[8]);
}

//NbJsonArray リスト一括操作(Double)
TEST(NbJsonArray, ListDouble) {
    NbJsonArray array;
    //DoubleでPut、DoubleでGet
    vector<double> vec{std::numeric_limits<double>::lowest(),1,2,3,4,5,6,7,8,std::numeric_limits<double>::max()};
    list<double> lis{std::numeric_limits<double>::max(),8,7,6,5,4,3,2,1,std::numeric_limits<double>::lowest()};

    array.PutAllList(vec);
    vector<double> out_vec;
    EXPECT_EQ(vec.size(), array.GetSize());
    array.GetAllDouble(&out_vec);
    EXPECT_EQ(vec, out_vec);

    array.PutAllList(lis);
    EXPECT_EQ(vec.size(), array.GetSize());
    list<double> out_lis;
    array.GetAllDouble(&out_lis);
    EXPECT_EQ(lis, out_lis);

    //UInt64でPut、DoubleでGet
    vector<uint64_t> vec_uint{std::numeric_limits<uint64_t>::lowest(),1,2,3,4,5,6,7,8,std::numeric_limits<uint64_t>::max()};
    list<uint64_t> lis_uint{std::numeric_limits<uint64_t>::max(),8,7,6,5,4,3,2,1,std::numeric_limits<uint64_t>::lowest()};

    array.PutAllList(vec_uint);
    EXPECT_EQ(vec.size(), array.GetSize());
    array.GetAllDouble(&out_vec);
    vec = vector<double>{std::numeric_limits<uint64_t>::lowest(),1,2,3,4,5,6,7,8,
                         static_cast<double>(std::numeric_limits<uint64_t>::max())};
    EXPECT_EQ(vec, out_vec);

    array.PutAllList(lis_uint);
    EXPECT_EQ(vec.size(), array.GetSize());
    array.GetAllDouble(&out_lis);
    lis = list<double>{static_cast<double>(std::numeric_limits<uint64_t>::max()),
                       8,7,6,5,4,3,2,1,std::numeric_limits<uint64_t>::lowest()};
    EXPECT_EQ(lis, out_lis);

    //Int64以外を含む
    array.PutAll(kDefaultArray2);
    array.GetAllDouble(&out_vec);
    EXPECT_EQ(1234567, out_vec[0]);
    EXPECT_EQ(1229782938247303441, out_vec[1]);
    EXPECT_EQ(123.456, out_vec[2]);
    EXPECT_EQ(1.23e+20, out_vec[3]);
    EXPECT_EQ(0, out_vec[4]);
    EXPECT_EQ(0, out_vec[5]);
    EXPECT_EQ(0, out_vec[6]);
    EXPECT_EQ(0, out_vec[7]);
    EXPECT_EQ(0, out_vec[8]);
}

//NbJsonArray リスト一括操作(Boolean)
TEST(NbJsonArray, ListBoolean) {
    NbJsonArray array;
    //BooleanでPut、BooleanでGet
    vector<bool> vec{true,false,true,false,false,true,true,true,false,true};
    list<bool> lis{false,true,false,true,true,false,false,false,true,false};

    array.PutAllList(vec);
    vector<bool> out_vec;
    EXPECT_EQ(vec.size(), array.GetSize());
    array.GetAllBoolean(&out_vec);
    EXPECT_EQ(vec, out_vec);

    array.PutAllList(lis);
    EXPECT_EQ(vec.size(), array.GetSize());
    list<bool> out_lis;
    array.GetAllBoolean(&out_lis);
    EXPECT_EQ(lis, out_lis);

    //Boolean以外を含む
    array.PutAll(kDefaultArray2);
    array.GetAllBoolean(&out_vec);
    EXPECT_FALSE(out_vec[0]);
    EXPECT_FALSE(out_vec[1]);
    EXPECT_FALSE(out_vec[2]);
    EXPECT_FALSE(out_vec[3]);
    EXPECT_FALSE(out_vec[4]);
    EXPECT_TRUE(out_vec[5]);
    EXPECT_FALSE(out_vec[6]);
    EXPECT_FALSE(out_vec[7]);
    EXPECT_FALSE(out_vec[8]);
}

//NbJsonArray リスト一括操作(String)
TEST(NbJsonArray, ListString) {
    NbJsonArray array;
    //StringでPut、StringでGet
    vector<string> vec{"aaa","bbb","ccc","ddd","eee","fff","ggg","hhh","iii","jjj"};
    list<string> lis{"zzz","yyy","xxx","www","vvv","uuu","ttt","sss","rrr","qqq"};

    array.PutAllList(vec);
    vector<string> out_vec;
    EXPECT_EQ(vec.size(), array.GetSize());
    array.GetAllString(&out_vec);
    EXPECT_EQ(vec, out_vec);

    array.PutAllList(lis);
    EXPECT_EQ(vec.size(), array.GetSize());
    list<string> out_lis;
    array.GetAllString(&out_lis);
    EXPECT_EQ(lis, out_lis);

    //Boolean以外を含む
    array.PutAll(kDefaultArray2);
    array.GetAllString(&out_vec);
    EXPECT_EQ(string(""), out_vec[0]);
    EXPECT_EQ(string(""), out_vec[1]);
    EXPECT_EQ(string(""), out_vec[2]);
    EXPECT_EQ(string(""), out_vec[3]);
    EXPECT_EQ(string("abcdef"), out_vec[4]);
    EXPECT_EQ(string(""), out_vec[5]);
    EXPECT_EQ(string(""), out_vec[6]);
    EXPECT_EQ(string(""), out_vec[7]);
    EXPECT_EQ(string(""), out_vec[8]);
}
} //namespace necbaas
