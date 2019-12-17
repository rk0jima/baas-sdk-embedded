/*
 * Copyright (C) 2019 NEC Corporation
 */

#ifndef NECBAAS_NBHTTPOPTIONS_H
#define NECBAAS_NBHTTPOPTIONS_H

#include <string>
#include <map>
#include <memory>
#include <curlpp/OptionBase.hpp>

namespace necbaas {

/**
 * @class NbHttpOptions nb_http_options.h "necbaas/nb_http_options.h"
 * HTTPオプション.
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbHttpOptions {
    // ユーザにCURLオプションは露出せずNbHttpRequstのみにアクセスを許す
    friend class NbHttpRequest;
    friend class TestUtil;

public:

	/**
     * SET CURLオプション.
     */

    NbHttpOptions &SslCert(const std::string &);
    NbHttpOptions &SslCertType(const std::string &);
    NbHttpOptions &SslCertPasswd(const std::string &);
    NbHttpOptions &SslKey(const std::string &);
    NbHttpOptions &SslKeyType(const std::string &);
    NbHttpOptions &SslKeyPasswd(const std::string &);
    NbHttpOptions &SslVerifyPeer(bool);
    NbHttpOptions &CaInfo(const std::string &);
    NbHttpOptions &CaPath(const std::string &);

private:
    typedef std::map<CURLoption, std::shared_ptr<curlpp::OptionBase>> options;

    options GetOptions() const;
    options options_;
};

}  // namespace necbaas
#endif  // NECBAAS_NBHTTPOPTIONS_H
