#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_json_object.h"

namespace necbaas {

using std::string;
using std::vector;
using std::list;
using std::shared_ptr;

// jsonオブジェクト設定・取得および各種操作
TEST(NbJsonFT, JsonObjectNormal) {
    NbJsonObject json;

    // 値設定
    json["KeyString"] = "ValueString";
    json["KeyStringNumber"] = "123";
    json["KeyNumberInt"] = (int)1;
    json["KeyNumberInt64"] = (int64_t)12345678901;
    json["KeyNumberDouble"] = (double)3.141592;
    json["KeyBool"] = true;
    json.PutNull("KeyNull");
    NbJsonObject jsonObjectValue(R"({"a", -1.2})");
    json.PutJsonObject("KeyJsonObject", jsonObjectValue);
    NbJsonArray jsonArrayValue(R"([1,"test",3])");
    json.PutJsonArray("KeyJsonArray", jsonArrayValue);

    // 値取得
    EXPECT_EQ(string("ValueString"), json.GetString("KeyString"));
    EXPECT_EQ(string("123"), json.GetString("KeyStringNumber"));
    EXPECT_EQ(1, json.GetInt("KeyNumberInt"));
    EXPECT_EQ(12345678901, json.GetInt64("KeyNumberInt64"));
    EXPECT_EQ(3.141592, json.GetDouble("KeyNumberDouble"));
    EXPECT_TRUE(json.GetBoolean("KeyBool"));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, json.GetType("KeyNull"));
    EXPECT_EQ(NbJsonObject(R"({"a", -1.2})"), json.GetJsonObject("KeyJsonObject"));
    EXPECT_EQ(NbJsonArray(R"([1,"test",3])"), json.GetJsonArray("KeyJsonArray"));

    // キー存在確認
    EXPECT_TRUE(json.IsMember("KeyString"));
    EXPECT_TRUE(json.IsMember("KeyStringNumber"));
    EXPECT_TRUE(json.IsMember("KeyNumberInt"));
    EXPECT_TRUE(json.IsMember("KeyNumberInt64"));
    EXPECT_TRUE(json.IsMember("KeyNumberDouble"));
    EXPECT_TRUE(json.IsMember("KeyBool"));
    EXPECT_TRUE(json.IsMember("KeyNull"));
    EXPECT_TRUE(json.IsMember("KeyJsonObject"));
    EXPECT_TRUE(json.IsMember("KeyJsonArray"));

    // サイズ取得
    EXPECT_EQ(9, json.GetSize());

    // 空確認
    EXPECT_FALSE(json.IsEmpty());

    // キー一覧取得
    vector<string> key_set{"KeyString","KeyStringNumber","KeyNumberInt","KeyNumberInt64","KeyNumberDouble",
                           "KeyBool","KeyNull","KeyJsonObject","KeyJsonArray"};
    FTUtil::CompareList(key_set, json.GetKeySet());

    // データタイプ取得
    EXPECT_EQ(NbJsonType::NB_JSON_STRING, json.GetType("KeyString"));
    EXPECT_EQ(NbJsonType::NB_JSON_STRING, json.GetType("KeyStringNumber"));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, json.GetType("KeyNumberInt"));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, json.GetType("KeyNumberInt64"));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, json.GetType("KeyNumberDouble"));
    EXPECT_EQ(NbJsonType::NB_JSON_BOOLEAN, json.GetType("KeyBool"));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, json.GetType("KeyNull"));
    EXPECT_EQ(NbJsonType::NB_JSON_OBJECT, json.GetType("KeyJsonObject"));
    EXPECT_EQ(NbJsonType::NB_JSON_ARRAY, json.GetType("KeyJsonArray"));

    // キー削除
    json.Remove("KeyString");
    json.Remove("KeyNumberInt64");
    json.Remove("KeyJsonObject");

    // キー存在確認
    EXPECT_FALSE(json.IsMember("KeyString"));
    EXPECT_TRUE(json.IsMember("KeyStringNumber"));
    EXPECT_TRUE(json.IsMember("KeyNumberInt"));
    EXPECT_FALSE(json.IsMember("KeyNumberInt64"));
    EXPECT_TRUE(json.IsMember("KeyNumberDouble"));
    EXPECT_TRUE(json.IsMember("KeyBool"));
    EXPECT_TRUE(json.IsMember("KeyNull"));
    EXPECT_FALSE(json.IsMember("KeyJsonObject"));
    EXPECT_TRUE(json.IsMember("KeyJsonArray"));

    // キークリア
    json.Clear();

    // サイズ取得
    EXPECT_EQ(0, json.GetSize());

    // 空確認
    EXPECT_TRUE(json.IsEmpty());

    // キー一覧取得
    EXPECT_TRUE(json.GetKeySet().empty());
}

// jsonオブジェクト 値取得（デフォルト値）
TEST(NbJsonFT, JsonObjectDefaultValue) {
    NbJsonObject json;

    // キーが存在しない
    EXPECT_EQ(string(""), json.GetString("TestKey"));
    EXPECT_EQ(0, json.GetInt("TestKey"));
    EXPECT_EQ(0, json.GetInt64("TestKey"));
    EXPECT_EQ(0.0, json.GetDouble("TestKey"));
    EXPECT_FALSE(json.GetBoolean("TestKey"));
    EXPECT_TRUE(json.GetJsonObject("TestKey").IsEmpty());
    EXPECT_TRUE(json.GetJsonArray("TestKey").IsEmpty());

    // キーが空
    EXPECT_EQ(string("abc"), json.GetString("", "abc"));
    EXPECT_EQ(123, json.GetInt("", 123));
    EXPECT_EQ(22222222222, json.GetInt64("", 22222222222));
    EXPECT_EQ(1.1, json.GetDouble("", 1.1));
    EXPECT_TRUE(json.GetBoolean("", true));
    EXPECT_TRUE(json.GetJsonObject("").IsEmpty());
    EXPECT_TRUE(json.GetJsonArray("").IsEmpty());
}

// jsonオブジェクト 値変換
TEST(NbJsonFT, JsonObjectConversion) {
    NbJsonObject json;
    json["int"] = (int)100;
    json["int64_a"] = (int64_t)200;
    json["int64_b"] = (int64_t)33333333333;
    json["double_a"] = 100.0;
    json["double_b"] = 44444444444.4;
    json["double_c"] = 1.23e+20;

    // int→int64_t
    EXPECT_EQ((int64_t)100, json.GetInt64("int"));
    // int→double
    EXPECT_EQ((double)100, json.GetDouble("int"));
    // int64_t→int (int範囲内)
    EXPECT_EQ((int)200, json.GetInt("int64_a"));
    // int64_t→int (int範囲外)
    EXPECT_EQ(0, json.GetInt("int64_b"));
    // int64_t→double
    EXPECT_EQ((double)33333333333, json.GetDouble("int64_b"));
    // double→int (int範囲内)
    EXPECT_EQ((int)100, json.GetInt("double_a"));
    // double→int (int範囲外)
    EXPECT_EQ((int)0, json.GetInt("double_b"));
    // double→int64_t (int範囲内)
    EXPECT_EQ((int64_t)44444444444, json.GetInt64("double_b"));
    //double→int64_t (int範囲外)
    EXPECT_EQ((int64_t)0, json.GetInt64("double_c"));

    // 変換不可
    json["bool"] = true;
    json["string"] = "100";

    EXPECT_EQ(0, json.GetInt("bool"));
    EXPECT_EQ(0, json.GetInt64("string"));
    EXPECT_EQ(0.0, json.GetDouble("bool"));
    EXPECT_EQ(0, json.GetBoolean("int64_a"));
    EXPECT_EQ(string(""), json.GetString("int"));
    EXPECT_TRUE(json.GetJsonObject("bool").IsEmpty());
    EXPECT_TRUE(json.GetJsonArray("string").IsEmpty());
}

// json配列 設定・取得および各種操作
TEST(NbJsonFT, JsonArrayNormal) {
    NbJsonArray json;

    // 値設定
    json[0] = "ValueString";
    EXPECT_TRUE(json.Append(string("123")));
    json[2] = (int)1;
    EXPECT_TRUE(json.Append((int64_t)12345678901));
    json[4] = (double)3.141592;
    EXPECT_TRUE(json.Append(true));
    EXPECT_TRUE(json.PutNull(6));
    NbJsonObject jsonObjectValue(R"({"a", -1.2})");
    EXPECT_TRUE(json.AppendJsonObject(jsonObjectValue));
    NbJsonArray jsonArrayValue(R"([1,"test",3])");
    EXPECT_TRUE(json.PutJsonArray(8, jsonArrayValue));

    // 値取得
    EXPECT_EQ(string("ValueString"), json.GetString(0));
    EXPECT_EQ(string("123"), json.GetString(1));
    EXPECT_EQ(1, json.GetInt(2));
    EXPECT_EQ(12345678901, json.GetInt64(3));
    EXPECT_EQ(3.141592, json.GetDouble(4));
    EXPECT_TRUE(json.GetBoolean(5));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, json.GetType(6));
    EXPECT_EQ(NbJsonObject(R"({"a", -1.2})"), json.GetJsonObject(7));
    EXPECT_EQ(NbJsonArray(R"([1,"test",3])"), json.GetJsonArray(8));

    // サイズ取得
    EXPECT_EQ(9, json.GetSize());

    // 空確認
    EXPECT_FALSE(json.IsEmpty());

    // データタイプ取得
    EXPECT_EQ(NbJsonType::NB_JSON_STRING, json.GetType(0));
    EXPECT_EQ(NbJsonType::NB_JSON_STRING, json.GetType(1));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, json.GetType(2));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, json.GetType(3));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, json.GetType(4));
    EXPECT_EQ(NbJsonType::NB_JSON_BOOLEAN, json.GetType(5));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, json.GetType(6));
    EXPECT_EQ(NbJsonType::NB_JSON_OBJECT, json.GetType(7));
    EXPECT_EQ(NbJsonType::NB_JSON_ARRAY, json.GetType(8));

    // キー削除
    json.Remove(7);
    json.Remove(4);
    json.Remove(1);

    // キー存在確認
    EXPECT_EQ(NbJsonType::NB_JSON_STRING, json.GetType(0));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, json.GetType(1));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, json.GetType(2));
    EXPECT_EQ(NbJsonType::NB_JSON_BOOLEAN, json.GetType(3));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, json.GetType(4));
    EXPECT_EQ(NbJsonType::NB_JSON_ARRAY, json.GetType(5));

    // キークリア
    json.Clear();

    // サイズ取得
    EXPECT_EQ(0, json.GetSize());

    // 空確認
    EXPECT_TRUE(json.IsEmpty());
}

// json配列 値取得（デフォルト値）
TEST(NbJsonFT, JsonArrayDefaultValue) {
    NbJsonArray json;

    // Index範囲外
    EXPECT_EQ(string(""), json.GetString(0));
    EXPECT_EQ(0, json.GetInt(1));
    EXPECT_EQ(0, json.GetInt64(2));
    EXPECT_EQ(0.0, json.GetDouble(3));
    EXPECT_FALSE(json.GetBoolean(4));
    EXPECT_TRUE(json.GetJsonObject(5).IsEmpty());
    EXPECT_TRUE(json.GetJsonArray(6).IsEmpty());

    EXPECT_EQ(string("abc"), json.GetString(0, "abc"));
    EXPECT_EQ(123, json.GetInt(1, 123));
    EXPECT_EQ(22222222222, json.GetInt64(2, 22222222222));
    EXPECT_EQ(1.1, json.GetDouble(3, 1.1));
    EXPECT_TRUE(json.GetBoolean(4, true));
    EXPECT_TRUE(json.GetJsonObject(5).IsEmpty());
    EXPECT_TRUE(json.GetJsonArray(6).IsEmpty());
}

// json配列 値設定失敗（Index範囲外）
TEST(NbJsonFT, JsonArrayOutOfIndex) {
    NbJsonArray json;

    // Index範囲外
    json[std::numeric_limits<unsigned int>::max()] = 123;

    EXPECT_FALSE(json.PutJsonObject(std::numeric_limits<unsigned int>::max(), NbJsonObject()));
    EXPECT_FALSE(json.PutJsonArray(std::numeric_limits<unsigned int>::max(), NbJsonArray()));
    EXPECT_FALSE(json.PutNull(std::numeric_limits<unsigned int>::max()));
}

// json配列 リスト設定・取得
TEST(NbJsonFT, JsonArrayList) {
    NbJsonArray json;

    vector<int> vec_int{1,2,3,4,5,6,7,8,9,10};
    json.PutAllList(vec_int);
    vector<int> out_vec_int;
    EXPECT_EQ(vec_int.size(), json.GetSize());
    json.GetAllInt(&out_vec_int);
    EXPECT_EQ(vec_int, out_vec_int);

    list<int64_t> list_int64{1,2,3,4,5,6,7,8,9,10};
    json.PutAllList(list_int64);
    list<int64_t> out_list_int64;
    EXPECT_EQ(list_int64.size(), json.GetSize());
    json.GetAllInt64(&out_list_int64);
    EXPECT_EQ(list_int64, out_list_int64);

    vector<double> vec_double{1,2,3,4,5,6,7,8,9,10};
    json.PutAllList(vec_double);
    vector<double> out_vec_double;
    EXPECT_EQ(vec_double.size(), json.GetSize());
    json.GetAllDouble(&out_vec_double);
    EXPECT_EQ(vec_double, out_vec_double);

    list<bool> list_bool{true,false,true,false,false,true,true,true,false,true};
    json.PutAllList(list_bool);
    list<bool> out_list_bool;
    EXPECT_EQ(list_bool.size(), json.GetSize());
    json.GetAllBoolean(&out_list_bool);
    EXPECT_EQ(list_bool, out_list_bool);

    vector<string> vec_str{"aaa","bbb","ccc","ddd","eee","fff","ggg","hhh","iii","jjj"};
    json.PutAllList(vec_str);
    vector<string> out_vec_str;
    EXPECT_EQ(vec_str.size(), json.GetSize());
    json.GetAllString(&out_vec_str);
    EXPECT_EQ(vec_str, out_vec_str);
}

// json配列 データ型違いはデフォルト値
TEST(NbJsonFT, JsonArrayListDefault) {
    NbJsonArray json;

    // 値設定
    json[0] = "ValueString";
    EXPECT_TRUE(json.Append(string("123")));
    json[2] = (int)1;
    EXPECT_TRUE(json.Append((int64_t)12345678901));
    json[4] = (double)3.141592;
    EXPECT_TRUE(json.Append(true));
    EXPECT_TRUE(json.PutNull(6));
    NbJsonObject jsonObjectValue(R"({"a", -1.2})");
    EXPECT_TRUE(json.AppendJsonObject(jsonObjectValue));
    NbJsonArray jsonArrayValue(R"([1,"test",3])");
    EXPECT_TRUE(json.PutJsonArray(8, jsonArrayValue));

    // Int
    vector<int> out_vec_int;
    json.GetAllInt(&out_vec_int);
    EXPECT_EQ((vector<int>{0,0,1,0,3,0,0,0,0}), out_vec_int);

    // Int64
    list<int64_t> out_list_int64;
    json.GetAllInt64(&out_list_int64);
    EXPECT_EQ((list<int64_t>{0,0,1,12345678901,3,0,0,0,0}), out_list_int64);

    // Double
    vector<double> out_vec_double;
    json.GetAllDouble(&out_vec_double);
    EXPECT_EQ((vector<double>{0,0,1,12345678901,3.141592,0,0,0,0}), out_vec_double);

    // Boolean
    list<bool> out_list_bool;
    json.GetAllBoolean(&out_list_bool);
    EXPECT_EQ((list<bool>{false,false,false,false,false,true,false,false,false}), out_list_bool);

    // String
    vector<string> out_vec_string;
    json.GetAllString(&out_vec_string);
    EXPECT_EQ((vector<string>{string("ValueString"),string("123"),string(),string(),string(),string(),string(),string(),string()}), out_vec_string);
}

// json配列 値変換
TEST(NbJsonFT, JsonArrayConversion) {
    NbJsonArray json;
    json[0] = (int)100;
    json[1] = (int64_t)200;
    json[2] = (int64_t)33333333333;
    json[3] = 100.0;
    json[4] = 44444444444.4;
    json[5] = 1.23e+20;

    // int→int64_t
    EXPECT_EQ((int64_t)100, json.GetInt64(0));
    // int→double
    EXPECT_EQ((double)100, json.GetDouble(0));
    // int64_t→int (int範囲内)
    EXPECT_EQ((int)200, json.GetInt(1));
    // int64_t→int (int範囲外)
    EXPECT_EQ(0, json.GetInt(2));
    // int64_t→double
    EXPECT_EQ((double)33333333333, json.GetDouble(2));
    // double→int (int範囲内)
    EXPECT_EQ((int)100, json.GetInt(3));
    // double→int (int範囲外)
    EXPECT_EQ((int)0, json.GetInt(4));
    // double→int64_t (int範囲内)
    EXPECT_EQ((int64_t)44444444444, json.GetInt64(4));
    //double→int64_t (int範囲外)
    EXPECT_EQ((int64_t)0, json.GetInt64(5));

    // 変換不可
    json[6] = true;
    json[7] = "100";

    EXPECT_EQ(0, json.GetInt(6));
    EXPECT_EQ(0, json.GetInt64(7));
    EXPECT_EQ(0.0, json.GetDouble(6));
    EXPECT_EQ(0, json.GetBoolean(3));
    EXPECT_EQ(string(""), json.GetString(0));
    EXPECT_TRUE(json.GetJsonObject(6).IsEmpty());
    EXPECT_TRUE(json.GetJsonArray(7).IsEmpty());
}

// パース・変換 - JSON文字列⇔JSONオブジェクト
TEST(NbJsonFT, JsonObjectParse) {
    string json_string1 = "{\"a\":1,\"b\":\"txt\",\"c\":true,\"d\":{\"d1\":1,\"d2\":2,\"d3\":[1,2,3]},\"e\":[\"e1\",\"e2\",\"e3\"],\"f\":null,\"g\":\"\u3042\u3044る\",\"\u3046\u3048\":\"好きです\",\"h\":\" !\\\"#$%&'()*+-.,/:;<=>?@[]^_`{|}~\",\" !\\\"#$%&'()*+-.,/:;<=>?@[]^_`{|}~\":\"記号\"}";

    NbJsonObject json1;
    json1.PutAll(json_string1);

    NbJsonObject expected_json;
    expected_json["a"] = 1;
    expected_json["b"] = "txt";
    expected_json["c"] = true;
    NbJsonObject obj(R"({"d1": 1,"d2": 2,"d3": [1,2,3]})");
    expected_json.PutJsonObject("d", obj); 
    NbJsonArray ary(R"(["e1", "e2", "e3"])");
    expected_json.PutJsonArray("e", ary);
    expected_json.PutNull("f");
    expected_json["g"] = "\u3042\u3044る";
    expected_json["\u3046\u3048"] = "好きです";
    expected_json["h"] = " !\"#$%&'()*+-.,/:;<=>?@[]^_`{|}~";
    expected_json[" !\"#$%&'()*+-.,/:;<=>?@[]^_`{|}~"] = "記号";

    EXPECT_EQ(expected_json, json1);

    string json_string2 = R"({" !\"#$%&'()*+-.,/:;<=>?@[]^_`{|}~":"記号","a":1,"b":"txt","c":true,"d":{"d1":1,"d2":2,"d3":[1,2,3]},"e":["e1","e2","e3"],"f":null,"g":"あいる","h":" !\"#$%&'()*+-.,/:;<=>?@[]^_`{|}~","うえ":"好きです"})";

    EXPECT_EQ(json_string2, expected_json.ToJsonString());
}

// パース・変換 - JSON配列⇔JSONオブジェクト
TEST(NbJsonFT, JsonArrayParse) {
    string json_string1 = "[1,\"string\",false,{\"a\":\"b\"},[1,2,3],null,\"\u3042\u3044る\",\" !\\\"#$%&'()*+-.,/:;<=>?@[]^_`{|}~\"]";

    NbJsonArray json1;
    json1.PutAll(json_string1);

    NbJsonArray expected_json;
    expected_json[0] = 1;
    expected_json[1] = "string";
    expected_json[2] = false;
    NbJsonObject obj(R"({"a": "b"})");
    expected_json.PutJsonObject(3, obj);
    NbJsonArray ary(R"([1,2,3])");
    expected_json.PutJsonArray(4, ary);
    expected_json.PutNull(5);
    expected_json[6] = "\u3042\u3044る";
    expected_json[7] = " !\"#$%&'()*+-.,/:;<=>?@[]^_`{|}~";

    EXPECT_EQ(expected_json, json1);

    string json_string2 = R"([1,"string",false,{"a":"b"},[1,2,3],null,"あいる"," !\"#$%&'()*+-.,/:;<=>?@[]^_`{|}~"])";

    EXPECT_EQ(json_string2, expected_json.ToJsonString());
}

// パース・変換 - JSON文字列パース失敗
TEST(NbJsonFT, JsonObjectParseFail) {
    NbJsonObject json;
    json.PutAll(";l;:sdflg:s;dfgl");
    EXPECT_TRUE(json.IsEmpty());
}
    
// パース・変換 - JSON配列パース失敗
TEST(NbJsonFT, JsonArrayParseFail) {
    NbJsonArray json;
    json.PutAll("");
    EXPECT_TRUE(json.IsEmpty());
}
} //namespace necbaas
