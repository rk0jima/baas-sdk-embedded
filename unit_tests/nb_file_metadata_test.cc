#include "gtest/gtest.h"
#include "necbaas/nb_file_metadata.h"
#include "necbaas/internal/nb_utility.h"

namespace necbaas {

using std::string;

const string kMetadata{R"(
     {"_id":"58ae91555ef3760f5b9d92b1",
      "filename":"testFile",
      "contentType":"text\/plain",
      "length":27,
      "ACL":
           {"owner":"",
            "r":["g:authenticated"],
            "w":["g:authenticated"],
            "c":[],
            "u":[],
            "d":[],
            "admin":["g:authenticated"]},
      "createdAt":"2017-02-23T07:37:57.926Z",
      "updatedAt":"2017-02-23T07:37:58.926Z",
      "metaETag":"942e51ba-e34f-486d-ba83-851e5b9e3452",
      "fileETag":"34031d14-73ec-49c2-b03e-810b5793ef4f",
      "cacheDisabled":true})"};

const string kEmpty{""};

//NbFileMetadata デフォルトコンストラクタ
TEST(NbFileMetadata, NbFileMetadata) {
    NbFileMetadata metadata;
    EXPECT_EQ(kEmpty, metadata.GetFileName());
    EXPECT_EQ(kEmpty, metadata.GetContentType());
    EXPECT_TRUE(metadata.GetAcl().GetAdmin().empty());
    EXPECT_EQ(0, metadata.GetLength());
    EXPECT_EQ(kEmpty, NbUtility::TmToDateString(metadata.GetCreatedTime()));
    EXPECT_EQ(kEmpty, NbUtility::TmToDateString(metadata.GetUpdatedTime()));
    EXPECT_EQ(kEmpty, metadata.GetMetaETag());
    EXPECT_EQ(kEmpty, metadata.GetFileETag());
    EXPECT_FALSE(metadata.IsCacheDisabled());
}

//NbFileMetadata Json引数でのコンストラクタ
TEST(NbFileMetadata, NbFileMetadataJson) {
    NbJsonObject json(kMetadata);
    NbFileMetadata metadata(string("testBucket"), json);
    EXPECT_EQ(string("testFile"), metadata.GetFileName());
    EXPECT_EQ(string("text/plain"), metadata.GetContentType());
    EXPECT_EQ(string("g:authenticated"), metadata.GetAcl().GetAdmin().front());
    EXPECT_EQ(27, metadata.GetLength());
    EXPECT_EQ(string("2017-02-23T07:37:57.000Z"), NbUtility::TmToDateString(metadata.GetCreatedTime()));
    EXPECT_EQ(string("2017-02-23T07:37:58.000Z"), NbUtility::TmToDateString(metadata.GetUpdatedTime()));
    EXPECT_EQ(string("942e51ba-e34f-486d-ba83-851e5b9e3452"), metadata.GetMetaETag());
    EXPECT_EQ(string("34031d14-73ec-49c2-b03e-810b5793ef4f"), metadata.GetFileETag());
    EXPECT_TRUE(metadata.IsCacheDisabled());
}
} //namespace necbaas
