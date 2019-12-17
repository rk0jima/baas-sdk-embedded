#include "gtest/gtest.h"
#include "test_util.h"
#include "necbaas/nb_http_options.h"
#include <curlpp/Options.hpp>

#define CURLOPT_GETVAL(optp, type) \
    dynamic_cast<curlpp::Options::type *>(optp)->getValue()

namespace necbaas {

TEST(NbHttpOptions, Empty) {
    auto opts = NbHttpOptions();
    EXPECT_TRUE(TestUtil::NbHttpOptions_GetOptions(opts).empty());
}

TEST(NbHttpOptions, SetOptions) {
    auto opts = NbHttpOptions()
        .SslCert("/path/to/cert.pem")
        .SslCertType("PEM")
        .SslCertPasswd("certpasswd")
        .SslKey("/path/to/key.pem")
        .SslKeyPasswd("keypasswd")
        .SslVerifyPeer(true)
        .CaInfo("/path/to/cacert.pem")
        .CaPath("/path/to/ca");

    auto map = TestUtil::NbHttpOptions_GetOptions(opts);
    // SSLCERTPASSWD and SSLKEYPASSWD is the same thing.
    EXPECT_EQ(map.size(), 7);

    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSLCERT].get(), SslCert), "/path/to/cert.pem");
    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSLCERTTYPE].get(), SslCertType), "PEM");
    // SSLCERTPASSWD is overridden.
    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSLCERTPASSWD].get(), SslCertPasswd), "keypasswd");
    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSLKEY].get(), SslKey), "/path/to/key.pem");
    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSLKEYTYPE].get(), SslKeyType), "PEM");
    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSLKEYPASSWD].get(), SslKeyPasswd), "keypasswd");
    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSL_VERIFYPEER].get(), SslVerifyPeer), true);
    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_CAINFO].get(), CaInfo), "/path/to/cacert.pem");
    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_CAPATH].get(), CaPath), "/path/to/ca");
}

TEST(NbHttpOptions, OptionOverride) {
    auto opts = NbHttpOptions()
        .SslCert("/path/to/cert.pem")
        .SslCertType("PEM")
        .SslCertPasswd("certpasswd");

    auto map = TestUtil::NbHttpOptions_GetOptions(opts);
    EXPECT_EQ(map.size(), 3);

    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSLCERT].get(), SslCert), "/path/to/cert.pem");
    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSLCERTTYPE].get(), SslCertType), "PEM");
    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSLCERTPASSWD].get(), SslCertPasswd), "certpasswd");

    opts.SslCert("/new/path/to/cert.der")
        .SslCertType("DER");

    map = TestUtil::NbHttpOptions_GetOptions(opts);
    EXPECT_EQ(map.size(), 3);

    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSLCERT].get(), SslCert), "/new/path/to/cert.pem");
    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSLCERTTYPE].get(), SslCertType), "DER");
    EXPECT_EQ(CURLOPT_GETVAL(map[CURLOPT_SSLCERTPASSWD].get(), SslCertPasswd), "certpasswd");
}

} //namespace necbaas
