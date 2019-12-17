/*
 * Copyright (C) 2019 NEC Corporation
 */

#include "necbaas/nb_http_options.h"
#include <curlpp/Options.hpp>

namespace necbaas {

NbHttpOptions &NbHttpOptions::SslCert(const std::string &val) {
    options_[CURLOPT_SSLCERT] = std::make_shared<curlpp::Options::SslCert>(val);
    return *this;
}

NbHttpOptions &NbHttpOptions::SslCertType(const std::string &val) {
    options_[CURLOPT_SSLCERTTYPE] = std::make_shared<curlpp::Options::SslCertType>(val);
    return *this;
}

NbHttpOptions &NbHttpOptions::SslCertPasswd(const std::string &val) {
    options_[CURLOPT_SSLCERTPASSWD] = std::make_shared<curlpp::Options::SslCertPasswd>(val);
    return *this;
}

NbHttpOptions &NbHttpOptions::SslKey(const std::string &val) {
    options_[CURLOPT_SSLKEY] = std::make_shared<curlpp::Options::SslKey>(val);
    return *this;
}

NbHttpOptions &NbHttpOptions::SslKeyType(const std::string &val) {
    options_[CURLOPT_SSLKEYTYPE] = std::make_shared<curlpp::Options::SslKeyType>(val);
    return *this;
}

NbHttpOptions &NbHttpOptions::SslKeyPasswd(const std::string &val) {
    options_[CURLOPT_SSLKEYPASSWD] = std::make_shared<curlpp::Options::SslKeyPasswd>(val);
    return *this;
}

NbHttpOptions &NbHttpOptions::SslVerifyPeer(bool val) {
    options_[CURLOPT_SSL_VERIFYPEER] = std::make_shared<curlpp::Options::SslVerifyPeer>(val);
    return *this;
}

NbHttpOptions &NbHttpOptions::CaInfo(const std::string &val) {
    options_[CURLOPT_CAINFO] = std::make_shared<curlpp::Options::CaInfo>(val);
    return *this;
}

NbHttpOptions &NbHttpOptions::CaPath(const std::string &val) {
    options_[CURLOPT_CAPATH] = std::make_shared<curlpp::Options::CaPath>(val);
    return *this;
}

std::map<CURLoption, std::shared_ptr<curlpp::OptionBase>> NbHttpOptions::GetOptions() const {
    return options_;
}

} //namespace necbaas
