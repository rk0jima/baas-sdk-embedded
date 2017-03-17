#include "gtest/gtest.h"
#include "necbaas/internal/nb_http_handler.h"

namespace necbaas {

using std::string;
using std::vector;

static const vector<string> kHeaders = {
    "HTTP/1.1 200 OK\r\n",
    "Server:      \r\n",
    "X-Content-Type-Options: nosniff\r\n",
    "Cache-Control:no-cache, no-store, max-age=0, must-revalidate\r\n",
    "",
    "Expires: ",
    "Content-Type:        application/json\r\n",
    "Content-Length: 20\r\n",
    "Date: Tue, 07 Mar 2017 08:49:58 GMT\r\n",
    "\r\n"};

static char kBody[] = "1234567890abcdefghij";

//NbHttpHandler 通常処理(ヘッダの様々なパターンを含む)
TEST(NbHttpHandler, Normal) {
    NbHttpHandler handler;

    for (auto header : kHeaders) {
        EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), 1, header.size()));
    }

    EXPECT_EQ(10, handler.WriteCallback(kBody, 1, 10));
    EXPECT_EQ(0, handler.WriteCallback(kBody, 1, 0));
    EXPECT_EQ(10, handler.WriteCallback(kBody + 10, 1, 10));

    NbHttpResponse response = handler.Parse();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string("OK"), response.GetReasonPhrase());
    std::multimap<std::string, std::string> resp_headers = response.GetHeaders();
    EXPECT_EQ(7, resp_headers.size());
    EXPECT_EQ(string(""), resp_headers.find("Server")->second);
    EXPECT_EQ(string("nosniff"), resp_headers.find("X-Content-Type-Options")->second);
    EXPECT_EQ(string("no-cache, no-store, max-age=0, must-revalidate"), resp_headers.find("Cache-Control")->second);
    EXPECT_EQ(string(""), resp_headers.find("Expires")->second);
    EXPECT_EQ(string("application/json"), resp_headers.find("Content-Type")->second);
    EXPECT_EQ(string("20"), resp_headers.find("Content-Length")->second);
    EXPECT_EQ(string("Tue, 07 Mar 2017 08:49:58 GMT"), resp_headers.find("Date")->second);

    string body_str(response.GetBody().begin(), response.GetBody().end());
    EXPECT_EQ(kBody, body_str);
}

//NbHttpHandler 通常処理(コールバックのsizeとnmembを入れ替え)
TEST(NbHttpHandler, Normal2) {
    NbHttpHandler handler;

    for (auto header : kHeaders) {
        EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), header.size(), 1));
    }

    EXPECT_EQ(10, handler.WriteCallback(kBody, 10, 1));
    EXPECT_EQ(0, handler.WriteCallback(kBody, 10, 0));
    EXPECT_EQ(10, handler.WriteCallback(kBody + 10, 10, 1));

    NbHttpResponse response = handler.Parse();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string("OK"), response.GetReasonPhrase());
    std::multimap<std::string, std::string> resp_headers = response.GetHeaders();
    EXPECT_EQ(7, resp_headers.size());
    EXPECT_EQ(string(""), resp_headers.find("Server")->second);
    EXPECT_EQ(string("nosniff"), resp_headers.find("X-Content-Type-Options")->second);
    EXPECT_EQ(string("no-cache, no-store, max-age=0, must-revalidate"), resp_headers.find("Cache-Control")->second);
    EXPECT_EQ(string(""), resp_headers.find("Expires")->second);
    EXPECT_EQ(string("application/json"), resp_headers.find("Content-Type")->second);
    EXPECT_EQ(string("20"), resp_headers.find("Content-Length")->second);
    EXPECT_EQ(string("Tue, 07 Mar 2017 08:49:58 GMT"), resp_headers.find("Date")->second);

    string body_str(response.GetBody().begin(), response.GetBody().end());
    EXPECT_EQ(kBody, body_str);
}

//NbHttpHandler 通知なし
TEST(NbHttpHandler, CalllBackNone) {
    NbHttpHandler handler;
    NbHttpResponse response = handler.Parse();
    EXPECT_EQ(0, response.GetStatusCode());
    EXPECT_EQ(0, response.GetHeaders().size());
    EXPECT_EQ(0, response.GetBody().size());
}

//NbHttpHandler ステータスライン複数)
TEST(NbHttpHandler, StatusLines) {
static const vector<string> headers = {
    "HTTP/1.1 200 OK\r\n",
    "Server:      \r\n",
    "X-Content-Type-Options: nosniff\r\n",
    "Cache-Control:no-cache, no-store, max-age=0, must-revalidate\r\n",
    "",
    "Expires: 0\r\n",
    "HTTP/1.1 500 Internal Server Error\r\n",
    "Content-Type:        application/json\r\n",
    "Content-Length: 20\r\n",
    "Date: Tue, 07 Mar 2017 08:49:58 GMT\r\n",
    "\r\n",
    "HTTP/1.1 900 Find Status Line\r\n",
    "Content-Type: image/jpeg\r\n",
    "Content-Length: 12345\r\n",
    "Date: Tue, 07 Mar 2017 08:49:59 GMT\r\n",
    "\r\n"};

    NbHttpHandler handler;

    for (auto header : headers) {
        EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), header.size(), 1));
    }

    NbHttpResponse response = handler.Parse();
    EXPECT_EQ(900, response.GetStatusCode());
    EXPECT_EQ(string("Find Status Line"), response.GetReasonPhrase());
    std::multimap<std::string, std::string> resp_headers = response.GetHeaders();
    EXPECT_EQ(3, resp_headers.size());
    EXPECT_EQ(string("image/jpeg"), resp_headers.find("Content-Type")->second);
    EXPECT_EQ(string("12345"), resp_headers.find("Content-Length")->second);
    EXPECT_EQ(string("Tue, 07 Mar 2017 08:49:59 GMT"), resp_headers.find("Date")->second);
}

//NbHttpHandler ステータスラインなし
TEST(NbHttpHandler, StatusLinesNone) {
static const vector<string> headers = {
    "Server:      \r\n",
    "X-Content-Type-Options: nosniff\r\n",
    "Cache-Control:no-cache, no-store, max-age=0, must-revalidate\r\n",
    "",
    "Expires: 0\r\n",
    "Content-Type:        application/json\r\n",
    "Content-Length: 20\r\n",
    "Date: Tue, 07 Mar 2017 08:49:58 GMT\r\n",
    "\r\n",
    "Content-Type: image/jpeg\r\n",
    "Content-Length: 12345\r\n",
    "Date: Tue, 07 Mar 2017 08:49:59 GMT\r\n",
    "\r\n"};

    NbHttpHandler handler;

    for (auto header : headers) {
        EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), header.size(), 1));
    }

    NbHttpResponse response = handler.Parse();
    EXPECT_EQ(0, response.GetStatusCode());
    EXPECT_EQ(string(""), response.GetReasonPhrase());
    EXPECT_EQ(0, response.GetHeaders().size());
}

//NbHttpHandler ステータスラインのみ
TEST(NbHttpHandler, StatusLinesOnly) {
static const vector<string> headers = {
    "HTTP/1.1 200 OK\r\n",
    };

    NbHttpHandler handler;

    for (auto header : headers) {
        EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), header.size(), 1));
    }

    NbHttpResponse response = handler.Parse();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string("OK"), response.GetReasonPhrase());
    EXPECT_EQ(0, response.GetHeaders().size());
}

//NbHttpHandler ステータスライン長さが足りない
TEST(NbHttpHandler, StatusLinesShort) {
static const vector<string> headers = {
    "HTTP/1.1 200",
    };

    NbHttpHandler handler;

    for (auto header : headers) {
        EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), header.size(), 1));
    }

    NbHttpResponse response = handler.Parse();
    EXPECT_EQ(0, response.GetStatusCode());
    EXPECT_EQ(string(""), response.GetReasonPhrase());
    EXPECT_EQ(0, response.GetHeaders().size());
}

//NbHttpHandler HTTPバージョン不正
TEST(NbHttpHandler, InvalidHttpVersion) {
static const vector<string> headers = {
    "HTTP/1.2 200 OK\r\n",
    };

    NbHttpHandler handler;

    for (auto header : headers) {
        EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), header.size(), 1));
    }

    NbHttpResponse response = handler.Parse();
    EXPECT_EQ(0, response.GetStatusCode());
    EXPECT_EQ(string(""), response.GetReasonPhrase());
    EXPECT_EQ(0, response.GetHeaders().size());
}

//NbHttpHandler ステータスライン長さが足りない
TEST(NbHttpHandler, StatusLinesShort2) {
static const vector<string> headers = {
    "HTTP/1.1 200 ",
    };

    NbHttpHandler handler;

    for (auto header : headers) {
        EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), header.size(), 1));
    }

    NbHttpResponse response = handler.Parse();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string(""), response.GetReasonPhrase());
    EXPECT_EQ(0, response.GetHeaders().size());
}

//NbHttpHandler スペースなし
TEST(NbHttpHandler, SpaceNone) {
static const vector<string> headers = {
    "HTTP/1.1 200OK\r\n",
    };

    NbHttpHandler handler;

    for (auto header : headers) {
        EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), header.size(), 1));
    }

    NbHttpResponse response = handler.Parse();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string(""), response.GetReasonPhrase());
    EXPECT_EQ(0, response.GetHeaders().size());
}

//NbHttpHandler ステータスライン改行コードなし
TEST(NbHttpHandler, StatusLinesShort3) {
static const vector<string> headers = {
    "HTTP/1.1 200 O",
    };

    NbHttpHandler handler;

    for (auto header : headers) {
        EXPECT_EQ(header.size(), handler.WriteHeaderCallback((void *)header.c_str(), header.size(), 1));
    }

    NbHttpResponse response = handler.Parse();
    EXPECT_EQ(200, response.GetStatusCode());
    EXPECT_EQ(string(""), response.GetReasonPhrase());
    EXPECT_EQ(0, response.GetHeaders().size());
}

//NbHttpHandler ReadCallback
TEST(NbHttpHandler, ReadCallback) {
    NbHttpHandler handler;
    EXPECT_EQ(0, handler.ReadCallback(nullptr, 0, 0));
}
} //namespace necbaas
