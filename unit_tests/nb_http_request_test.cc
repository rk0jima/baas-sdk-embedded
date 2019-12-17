#include "gtest/gtest.h"
#include "necbaas/internal/nb_http_request.h"
#include "necbaas/internal/nb_logger.h"
#include <curlpp/Options.hpp>

namespace necbaas {

using std::string;

const string kEmpty{""};
const string kUrl{"http://localhost/api/1/tenantid/path"};

//Dump以外はNbHttpRequestFactoryクラスから実施

//NbHttpRequest Dump(ログ無効)
TEST(NbHttpRequest, DumpDisable) {
    std::list<string> headers;
    headers.push_back("X-HEADER-KEY1: abcdef");
    headers.push_back("X-HEADER-KEY2: 123456");

    string body{"message body"};
    NbHttpOptions opts;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, headers, body, kEmpty, opts);

    NbLogger::SetRestLogEnabled(false);

    testing::internal::CaptureStdout();
    request.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(kEmpty, out_string);
}

TEST(NbHttpRequest, GetHttpOptionsNone) {
    std::list<string> headers;

    string body{"message body"};
    NbHttpOptions opts;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, headers, body, kEmpty, opts);

    std::list<std::shared_ptr<curlpp::OptionBase>> opt_list = request.GetHttpOptions();
    EXPECT_TRUE(opt_list.empty());
}

TEST(NbHttpRequest, GetHttpOptionsSome) {
    std::list<string> headers;

    string body{"message body"};
    NbHttpOptions opts;
    opts.SslCert("/tmp/cert.pem")
        .SslCertType("PEM")
        .SslCertPasswd("passwd");

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, headers, body, kEmpty, opts);

    std::list<std::shared_ptr<curlpp::OptionBase>> opt_list = request.GetHttpOptions();
    EXPECT_EQ(opt_list.size(), 3);
    auto iter = std::find_if(opt_list.begin(), opt_list.end(), [](std::shared_ptr<curlpp::OptionBase> &x) {
        auto *p = dynamic_cast<curlpp::Options::SslCert *>(x.get());
        return p != nullptr && p->getValue() == "/tmp/cert.pem";
    });
    EXPECT_NE(iter, opt_list.end());
    iter = std::find_if(opt_list.begin(), opt_list.end(), [](std::shared_ptr<curlpp::OptionBase> &x) {
        auto *p = dynamic_cast<curlpp::Options::SslCertType *>(x.get());
        return p != nullptr && p->getValue() == "PEM";
    });
    EXPECT_NE(iter, opt_list.end());
    iter = std::find_if(opt_list.begin(), opt_list.end(), [](std::shared_ptr<curlpp::OptionBase> &x) {
        auto *p = dynamic_cast<curlpp::Options::SslCertPasswd *>(x.get());
        return p != nullptr && p->getValue() == "passwd";
    });
    EXPECT_NE(iter, opt_list.end());
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

//NbHttpRequest Dump(ヘッダなし、コンテントタイプなし)
TEST(NbHttpRequest, DumpHeaderNone) {
    std::list<string> headers;

    string body{"message body"};
    NbHttpOptions opts;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, headers, body, kEmpty, opts);

    NbLogger::SetRestLogEnabled(true);

    testing::internal::CaptureStdout();
    request.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(1, CountString(out_string, string("[request][method]")));
    EXPECT_EQ(1, CountString(out_string, string("[request][url]")));
    EXPECT_EQ(0, CountString(out_string, string("[request][header]")));
    EXPECT_EQ(0, CountString(out_string, string("[request][body]")));
}

//NbHttpRequest Dump(コンテントタイプ: application/json)
TEST(NbHttpRequest, DumpApplicationJson) {
    std::list<string> headers;
    headers.push_back("X-HEADER-KEY1: abcdef");
    headers.push_back("X-HEADER-KEY2: 123456");
    headers.push_back("Content-Type: application/json");

    string body{"message body"};
    NbHttpOptions opts;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, headers, body, kEmpty, opts);

    NbLogger::SetRestLogEnabled(true);

    testing::internal::CaptureStdout();
    request.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(1, CountString(out_string, string("[request][method]")));
    EXPECT_EQ(1, CountString(out_string, string("[request][url]")));
    EXPECT_EQ(3, CountString(out_string, string("[request][header]")));
    EXPECT_EQ(1, CountString(out_string, string("[request][body]")));
}

//NbHttpRequest Dump(コンテントタイプ: text/plain)
TEST(NbHttpRequest, DumpTextPlain) {
    std::list<string> headers;
    headers.push_back("X-HEADER-KEY1: abcdef");
    headers.push_back("Content-Type: text/plain");
    headers.push_back("X-HEADER-KEY2: 123456");

    string body{"message body"};
    NbHttpOptions opts;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, headers, body, kEmpty, opts);

    NbLogger::SetRestLogEnabled(true);

    testing::internal::CaptureStdout();
    request.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(1, CountString(out_string, string("[request][method]")));
    EXPECT_EQ(1, CountString(out_string, string("[request][url]")));
    EXPECT_EQ(3, CountString(out_string, string("[request][header]")));
    EXPECT_EQ(1, CountString(out_string, string("[request][body]")));
}

//NbHttpRequest Dump(コンテントタイプ: text/html)
TEST(NbHttpRequest, DumpTextHtml) {
    std::list<string> headers;
    headers.push_back("Content-Type: text/html");
    headers.push_back("X-HEADER-KEY1: abcdef");
    headers.push_back("X-HEADER-KEY2: 123456");

    string body{"message body"};
    NbHttpOptions opts;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, headers, body, kEmpty, opts);

    NbLogger::SetRestLogEnabled(true);

    testing::internal::CaptureStdout();
    request.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(1, CountString(out_string, string("[request][method]")));
    EXPECT_EQ(1, CountString(out_string, string("[request][url]")));
    EXPECT_EQ(3, CountString(out_string, string("[request][header]")));
    EXPECT_EQ(1, CountString(out_string, string("[request][body]")));
}

//NbHttpRequest Dump(コンテントタイプ: text/xml)
TEST(NbHttpRequest, DumpTextXml) {
    std::list<string> headers;
    headers.push_back("Content-Type: text/xml");

    string body{"message body"};
    NbHttpOptions opts;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, headers, body, kEmpty, opts);

    NbLogger::SetRestLogEnabled(true);

    testing::internal::CaptureStdout();
    request.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(1, CountString(out_string, string("[request][method]")));
    EXPECT_EQ(1, CountString(out_string, string("[request][url]")));
    EXPECT_EQ(1, CountString(out_string, string("[request][header]")));
    EXPECT_EQ(1, CountString(out_string, string("[request][body]")));
}

//NbHttpRequest Dump(コンテントタイプ: image/jpeg)
TEST(NbHttpRequest, DumpImageJpeg) {
    std::list<string> headers;
    headers.push_back("X-HEADER-KEY1: abcdef");
    headers.push_back("X-HEADER-KEY2: 123456");
    headers.push_back("Content-Type: image/jpeg");

    string body{"message body"};
    NbHttpOptions opts;

    NbHttpRequest request(kUrl, NbHttpRequestMethod::HTTP_REQUEST_TYPE_GET, headers, body, kEmpty, opts);

    NbLogger::SetRestLogEnabled(true);

    testing::internal::CaptureStdout();
    request.Dump();
    string out_string = testing::internal::GetCapturedStdout();
    EXPECT_EQ(1, CountString(out_string, string("[request][method]")));
    EXPECT_EQ(1, CountString(out_string, string("[request][url]")));
    EXPECT_EQ(3, CountString(out_string, string("[request][header]")));
    EXPECT_EQ(0, CountString(out_string, string("[request][body]")));
}
} //namespace necbaas
