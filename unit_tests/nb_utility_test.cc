#include "gtest/gtest.h"
#include "necbaas/internal/nb_utility.h"
//#include "test_util.h"

namespace necbaas {

using std::string;

//NbUtility::DateConvert(正常)
TEST(NbUtility, DateConvertNorm) {

    std::tm time = NbUtility::DateStringToTm("2017-03-01T01:23:45.678Z");

    EXPECT_EQ(2017, time.tm_year + 1900);
    EXPECT_EQ(3, time.tm_mon + 1);
    EXPECT_EQ(1, time.tm_mday);
    EXPECT_EQ(1, time.tm_hour);
    EXPECT_EQ(23, time.tm_min);
    EXPECT_EQ(45, time.tm_sec);

    string time_str = NbUtility::TmToDateString(time);

    EXPECT_EQ(string("2017-03-01T01:23:45.000Z"), time_str);
}

//NbUtility::DateConvert(空)
TEST(NbUtility, DateConvertEmpty) {

    std::tm time = NbUtility::DateStringToTm("");

    EXPECT_EQ(0, time.tm_year);
    EXPECT_EQ(0, time.tm_mon);
    EXPECT_EQ(0, time.tm_mday);
    EXPECT_EQ(0, time.tm_hour);
    EXPECT_EQ(0, time.tm_min);
    EXPECT_EQ(0, time.tm_sec);

    string time_str = NbUtility::TmToDateString(time);

    EXPECT_EQ(string(""), time_str);
}

//NbUtility::DateStringToTm(不正)
TEST(NbUtility, DateStringToTmSubnorm) {

    std::tm time = NbUtility::DateStringToTm("2017-03-1T01:23:45.678Z");

    EXPECT_EQ(0, time.tm_year);
    EXPECT_EQ(0, time.tm_mon);
    EXPECT_EQ(0, time.tm_mday);
    EXPECT_EQ(0, time.tm_hour);
    EXPECT_EQ(0, time.tm_min);
    EXPECT_EQ(0, time.tm_sec);
}

//NbUtility::GetFileSize(1k byte)
TEST(NbUtility, GetFileSize1K) {
    string file_path = __FILE__;
    file_path.erase(file_path.rfind("/") + 1);
    file_path += "file/1k.txt";

    // 1K byte
    int file_size = NbUtility::GetFileSize(file_path);
    EXPECT_EQ(1024, file_size);
}

//NbUtility::GetFileSize(0 byte)
TEST(NbUtility, GetFileSize0) {
    string file_path = __FILE__;
    file_path.erase(file_path.rfind("/") + 1);
    file_path += "file/0.txt";

    // 0byte
    int file_size = NbUtility::GetFileSize(file_path);
    EXPECT_EQ(0, file_size);
}

//NbUtility::GetFileSize(Open error)
TEST(NbUtility, GetFileSizeOpenError) {
    string file_path = __FILE__;
    file_path.erase(file_path.rfind("/") + 1);
    file_path += "file/none.txt";

    int file_size = NbUtility::GetFileSize(file_path);
    EXPECT_EQ(0, file_size);
}

//NbUtility::ConvertJsonType
TEST(NbUtility, ConvertJsonType) {
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, NbUtility::ConvertJsonType(Json::intValue));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, NbUtility::ConvertJsonType(Json::uintValue));
    EXPECT_EQ(NbJsonType::NB_JSON_NUMBER, NbUtility::ConvertJsonType(Json::realValue));
    EXPECT_EQ(NbJsonType::NB_JSON_STRING, NbUtility::ConvertJsonType(Json::stringValue));
    EXPECT_EQ(NbJsonType::NB_JSON_BOOLEAN, NbUtility::ConvertJsonType(Json::booleanValue));
    EXPECT_EQ(NbJsonType::NB_JSON_ARRAY, NbUtility::ConvertJsonType(Json::arrayValue));
    EXPECT_EQ(NbJsonType::NB_JSON_OBJECT, NbUtility::ConvertJsonType(Json::objectValue));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, NbUtility::ConvertJsonType(Json::nullValue));
    EXPECT_EQ(NbJsonType::NB_JSON_NULL, NbUtility::ConvertJsonType(static_cast<Json::ValueType>(100)));
}

//NbUtility::CompareCaseInsensitiveString
TEST(NbUtility, CompareCaseInsensitiveString) {
    EXPECT_TRUE(NbUtility::CompareCaseInsensitiveString(string("Test-String123"), string("test-string123")));
    EXPECT_TRUE(NbUtility::CompareCaseInsensitiveString(string("Test-String123"), string("TEST-STRING123")));
    EXPECT_FALSE(NbUtility::CompareCaseInsensitiveString(string("Test-String123"), string("Test-String")));
    EXPECT_FALSE(NbUtility::CompareCaseInsensitiveString(string("TestString"), string("Test-String")));
    EXPECT_FALSE(NbUtility::CompareCaseInsensitiveString(string("Test-String123"), string("")));
    EXPECT_TRUE(NbUtility::CompareCaseInsensitiveString(string(""), string("")));
}
} //namespace necbaas
