#include "gtest/gtest.h"
#include "necbaas/nb_http_response.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;

const string kEmpty{""};

//NbHttpResponse コンストラクタ
TEST(NbHttpResponse, NbHttpResponse) {
    NbHttpResponse response;

    EXPECT_EQ(0, response.GetStatusCode());
    EXPECT_EQ(kEmpty, response.GetReasonPhrase());
    EXPECT_TRUE(response.GetHeaders().empty());
    EXPECT_TRUE(response.GetBody().empty());
}


//NbHttpResponse コンストラクタ(値設定)
TEST(NbHttpResponse, NbHttpResponseValue) {
    std::multimap<std::string, std::string> headers;
    headers.insert(std::make_pair("X-HEADER-KEY1", "abcdef"));
    headers.insert(std::make_pair("X-HEADER-KEY2", "123456"));    

    std::vector<char> body{'1', '2', '3', '4', '5'};

    NbHttpResponse response(200, "OK", headers, body);

    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string("OK"), response.GetReasonPhrase());
    EXPECT_EQ(headers, response.GetHeaders());
    EXPECT_EQ(body, response.GetBody());
}

//NbHttpResponse Dump(ログ無効)
TEST(NbHttpResponse, DumpDisable) {
    std::multimap<std::string, std::string> headers;
    headers.insert(std::make_pair("X-HEADER-KEY1", "abcdef"));
    headers.insert(std::make_pair("X-HEADER-KEY2", "123456"));

    std::vector<char> body{'1', '2', '3', '4', '5'};

    NbHttpResponse response(200, "OK", headers, body);

    NbLogger::SetRestLogEnabled(false);

    testing::internal::CaptureStdout();
    response.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(string(""), out_string);
}

static int CountString(const string &str, const string &search) {
    int count = 0;
    for (int pos = 0; pos < str.size(); ++pos) {
        pos = str.find(search, pos);
        if (pos == string::npos) {
            break;
        }
        ++count;
    }
    return count;
} 

//NbHttpResponse Dump(ヘッダなし、コンテントタイプなし)
TEST(NbHttpResponse, DumpHeaderNone) {
    std::multimap<std::string, std::string> headers;

    std::vector<char> body{'1', '2', '3', '4', '5'};

    NbHttpResponse response(200, "OK", headers, body);

    NbLogger::SetRestLogEnabled(true);

    testing::internal::CaptureStdout();
    response.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(1, CountString(out_string, string("[response][status-code]")));
    EXPECT_EQ(1, CountString(out_string, string("[response][reason-phrase]")));
    EXPECT_EQ(0, CountString(out_string, string("[response][header]")));
    EXPECT_EQ(0, CountString(out_string, string("[response][body]")));
}

//NbHttpResponse Dump(コンテントタイプ: application/json)
TEST(NbHttpResponse, DumpApplicationJson) {
    std::multimap<std::string, std::string> headers;
    headers.insert(std::make_pair("X-HEADER-KEY1", "abcdef"));
    headers.insert(std::make_pair("X-HEADER-KEY2", "123456"));
    headers.insert(std::make_pair("Content-Type", "application/json"));

    std::vector<char> body{'1', '2', '3', '4', '5'};

    NbHttpResponse response(200, "OK", headers, body);

    NbLogger::SetRestLogEnabled(true);

    testing::internal::CaptureStdout();
    response.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(1, CountString(out_string, string("[response][status-code]")));
    EXPECT_EQ(1, CountString(out_string, string("[response][reason-phrase]")));
    EXPECT_EQ(3, CountString(out_string, string("[response][header]")));
    EXPECT_EQ(1, CountString(out_string, string("[response][body]")));
}

//NbHttpResponse Dump(コンテントタイプ: text/plain)
TEST(NbHttpResponse, DumpTextPlain) {
    std::multimap<std::string, std::string> headers;
    headers.insert(std::make_pair("X-HEADER-KEY1", "abcdef"));
    headers.insert(std::make_pair("Content-Type", "text/plain"));
    headers.insert(std::make_pair("X-HEADER-KEY2", "123456"));

    std::vector<char> body{'1', '2', '3', '4', '5'};

    NbHttpResponse response(200, "OK", headers, body);

    NbLogger::SetRestLogEnabled(true);

    testing::internal::CaptureStdout();
    response.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(1, CountString(out_string, string("[response][status-code]")));
    EXPECT_EQ(1, CountString(out_string, string("[response][reason-phrase]")));
    EXPECT_EQ(3, CountString(out_string, string("[response][header]")));
    EXPECT_EQ(1, CountString(out_string, string("[response][body]")));
}

//NbHttpResponse Dump(コンテントタイプ: text/html)
TEST(NbHttpResponse, DumpTextHtml) {
    std::multimap<std::string, std::string> headers;
    headers.insert(std::make_pair("Content-Type", "text/html"));
    headers.insert(std::make_pair("X-HEADER-KEY1", "abcdef"));
    headers.insert(std::make_pair("X-HEADER-KEY2", "123456"));

    std::vector<char> body{'1', '2', '3', '4', '5'};

    NbHttpResponse response(200, "OK", headers, body);

    NbLogger::SetRestLogEnabled(true);

    testing::internal::CaptureStdout();
    response.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(1, CountString(out_string, string("[response][status-code]")));
    EXPECT_EQ(1, CountString(out_string, string("[response][reason-phrase]")));
    EXPECT_EQ(3, CountString(out_string, string("[response][header]")));
    EXPECT_EQ(1, CountString(out_string, string("[response][body]")));
}

//NbHttpResponse Dump(コンテントタイプ: text/xml)
TEST(NbHttpResponse, DumpTextXml) {
    std::multimap<std::string, std::string> headers;
    headers.insert(std::make_pair("Content-Type", "text/xml"));

    std::vector<char> body{'1', '2', '3', '4', '5'};

    NbHttpResponse response(200, "OK", headers, body);

    NbLogger::SetRestLogEnabled(true);

    testing::internal::CaptureStdout();
    response.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(1, CountString(out_string, string("[response][status-code]")));
    EXPECT_EQ(1, CountString(out_string, string("[response][reason-phrase]")));
    EXPECT_EQ(1, CountString(out_string, string("[response][header]")));
    EXPECT_EQ(1, CountString(out_string, string("[response][body]")));
}

//NbHttpResponse Dump(コンテントタイプ: image/jpeg)
TEST(NbHttpResponse, DumpImageJpeg) {
    std::multimap<std::string, std::string> headers;
    headers.insert(std::make_pair("X-HEADER-KEY1", "abcdef"));
    headers.insert(std::make_pair("X-HEADER-KEY2", "123456"));
    headers.insert(std::make_pair("Content-Type", "image/jpeg"));

    std::vector<char> body{'1', '2', '3', '4', '5'};

    NbHttpResponse response(200, "OK", headers, body);

    NbLogger::SetRestLogEnabled(true);

    testing::internal::CaptureStdout();
    response.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(1, CountString(out_string, string("[response][status-code]")));
    EXPECT_EQ(1, CountString(out_string, string("[response][reason-phrase]")));
    EXPECT_EQ(3, CountString(out_string, string("[response][header]")));
    EXPECT_EQ(0, CountString(out_string, string("[response][body]")));
}
} //namespace necbaas
