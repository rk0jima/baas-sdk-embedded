#include "gtest/gtest.h"
#include "necbaas/internal/nb_http_file_upload_handler.h"

namespace necbaas {

using std::string;
using std::vector;

static const vector<string> kHeaders = {
    "HTTP/1.1 200 OK\r\n",
    "Content-Type: application/json\r\n",
    "Content-Length: 20\r\n",
    "\r\n"};

static char kBuf[] = "1234567890abcdefghij";

static string MakeFilePath(string file_name) {
    string file_path = __FILE__;
    file_path.erase(file_path.rfind("/") + 1);
    file_path += "file/" + file_name;
    return file_path;
}

//NbHttpFileUploadHandler 通常処理
TEST(NbHttpFileUploadHandler, Normal) {
    char buf[strlen(kBuf) + 1];
    memset(buf, 0, sizeof(buf));

    NbHttpFileUploadHandler handler(MakeFilePath("upload.dat"));

    EXPECT_FALSE(handler.IsError());

    EXPECT_EQ(10, handler.ReadCallback(buf, 1, 10));
    EXPECT_EQ(0, handler.ReadCallback(buf, 1, 0));
    EXPECT_EQ(10, handler.ReadCallback(buf + 10, 1, 10));
    EXPECT_EQ(0, handler.ReadCallback(buf + 10, 1, 10));
    EXPECT_EQ(0, handler.ReadCallback(buf + 10, 1, 10));

    EXPECT_STREQ(kBuf, buf);
}

//NbHttpFileUploadHandler 通常処理(読み込みサイズ10→15)
TEST(NbHttpFileUploadHandler, ReadSize15) {
    char buf[strlen(kBuf) + 1];
    memset(buf, 0, sizeof(buf));

    NbHttpFileUploadHandler handler(MakeFilePath("upload.dat"));

    EXPECT_FALSE(handler.IsError());

    EXPECT_EQ(15, handler.ReadCallback(buf, 1, 15));
    EXPECT_EQ(5, handler.ReadCallback(buf + 15, 1, 15));
    EXPECT_EQ(0, handler.ReadCallback(buf, 1, 15));

    EXPECT_STREQ(kBuf, buf);
}

//NbHttpFileUploadHandler ファイルオープンエラー
TEST(NbHttpFileUploadHandler, FileOpenError) {
    char buf[strlen(kBuf) + 1];

    NbHttpFileUploadHandler handler(string("notfounddir/upload.dat"));

    EXPECT_TRUE(handler.IsError());

    EXPECT_EQ(0, handler.ReadCallback(buf, 1, 10));
}

} //namespace necbaas
