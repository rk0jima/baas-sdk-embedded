#include "gtest/gtest.h"
#include "necbaas/internal/nb_http_file_download_handler.h"

namespace necbaas {

using std::string;
using std::vector;

static const vector<string> kHeaders = {
    "HTTP/1.1 200 OK\r\n",
    "Content-Type: application/json\r\n",
    "Content-Length: 20\r\n",
    "\r\n"};

static char kBuf[] = "1234567890abcdefghij";

//NbHttpFileDownloadHandler 通常処理
TEST(NbHttpFileDownloadHandler, Normal) {
    {
        NbHttpFileDownloadHandler handler(string("download.dat"));

        EXPECT_FALSE(handler.IsError());

        for (auto header : kHeaders) {
            EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), 1, header.size()));
        }

        EXPECT_EQ(10, handler.WriteCallback(kBuf, 1, 10));
        EXPECT_EQ(0, handler.WriteCallback(kBuf, 1, 0));
        EXPECT_EQ(10, handler.WriteCallback(kBuf + 10, 1, 10));

        NbHttpResponse response = handler.Parse();
        EXPECT_EQ(0, response.GetBody().size());
    } // handlerをここで解放してファイルclose
    std::ifstream file_stream;
    file_stream.open(string("download.dat"), std::ios::in | std::ios::binary);
    char buf[strlen(kBuf) + 1];
    EXPECT_EQ(strlen(kBuf), file_stream.read(buf, strlen(kBuf)).gcount());
    buf[strlen(kBuf)] = '\0';
    EXPECT_STREQ(kBuf, buf);
    std::remove("download.dat");
}

//NbHttpFileDownloadHandler 通常処理(ステータスコード200台)
TEST(NbHttpFileDownloadHandler, StatusCode2XX) {
static const vector<string> headers = {
    "HTTP/1.1 234 Status Test\r\n",
    "Content-Type: application/json\r\n",
    "Content-Length: 20\r\n",
    "\r\n"};
    {
        NbHttpFileDownloadHandler handler(string("download.dat"));

        EXPECT_FALSE(handler.IsError());

        for (auto header : headers) {
            EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), 1, header.size()));
        }

        EXPECT_EQ(10, handler.WriteCallback(kBuf, 1, 10));
        EXPECT_EQ(0, handler.WriteCallback(kBuf, 1, 0));
        EXPECT_EQ(10, handler.WriteCallback(kBuf + 10, 1, 10));

        NbHttpResponse response = handler.Parse();
        EXPECT_EQ(0, response.GetBody().size());
    } // handlerをここで解放してファイルclose
    std::ifstream file_stream;
    file_stream.open(string("download.dat"), std::ios::in | std::ios::binary);
    char buf[strlen(kBuf) + 1];
    EXPECT_EQ(strlen(kBuf), file_stream.read(buf, strlen(kBuf)).gcount());
    buf[strlen(kBuf)] = '\0';
    EXPECT_STREQ(kBuf, buf);
    std::remove("download.dat");
}

//NbHttpFileDownloadHandler ファイルオープンエラー
TEST(NbHttpFileDownloadHandler, FileOpenError) {
    {
        NbHttpFileDownloadHandler handler(string("notfounddir/download.dat"));

        EXPECT_TRUE(handler.IsError());

        for (auto header : kHeaders) {
            EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), 1, header.size()));
        }

        EXPECT_EQ(0, handler.WriteCallback(kBuf, 1, 10));
        EXPECT_EQ(0, handler.WriteCallback(kBuf, 1, 0));
        EXPECT_EQ(0, handler.WriteCallback(kBuf + 10, 1, 10));
    }
}

//NbHttpFileDownloadHandler ステータスコードエラー
TEST(NbHttpFileDownloadHandler, StatusCodeError) {
static const vector<string> headers = {
    "HTTP/1.1 404 Not Found\r\n",
    "Content-Type: application/json\r\n",
    "Content-Length: 20\r\n",
    "\r\n"};
    {
        NbHttpFileDownloadHandler handler(string("download.dat"));

        EXPECT_FALSE(handler.IsError());

        for (auto header : headers) {
            EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), 1, header.size()));
        }

        EXPECT_EQ(10, handler.WriteCallback(kBuf, 1, 10));
        EXPECT_EQ(0, handler.WriteCallback(kBuf, 1, 0));
        EXPECT_EQ(10, handler.WriteCallback(kBuf + 10, 1, 10));

        NbHttpResponse response = handler.Parse();
        EXPECT_EQ(404, response.GetStatusCode());
        EXPECT_EQ(string("Not Found"), response.GetReasonPhrase());
        std::multimap<std::string, std::string> resp_headers = response.GetHeaders();
        EXPECT_EQ(2, resp_headers.size());
        EXPECT_EQ(string("application/json"), resp_headers.find("Content-Type")->second);
        EXPECT_EQ(string("20"), resp_headers.find("Content-Length")->second);

        string body_str(response.GetBody().begin(), response.GetBody().end());
        EXPECT_EQ(kBuf, body_str);
    } // handlerをここで解放してファイルclose
    std::ifstream file_stream;
    file_stream.open(string("download.dat"), std::ios::in | std::ios::binary);
    char buf[strlen(kBuf) + 1];
    EXPECT_EQ(0, file_stream.read(buf, strlen(kBuf)).gcount());
    std::remove("download.dat");
}
} //namespace necbaas
