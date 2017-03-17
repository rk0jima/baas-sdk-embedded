#ifndef NECBAAS_TESTUTIL_H
#define NECBAAS_TESTUTIL_H

#include "necbaas/internal/nb_http_request_factory.h"

namespace necbaas {
// テストユーティリティクラス
// 試験対象クラスのフレンドクラスとなり、Privateメンバへのアクセスを可能とする
class TestUtil {
  public:
    // NbHttpRequestFactory
    static const std::string &NbHttpRequestFactory_GetEndPointUrl(const NbHttpRequestFactory &factory) {
        return factory.end_point_url_;
    }

    static const std::string &NbHttpRequestFactory_GetTenantId(const NbHttpRequestFactory &factory) {
        return factory.tenant_id_;
    }

    static const std::string &NbHttpRequestFactory_GetAppId(const NbHttpRequestFactory &factory) {
        return factory.app_id_;
    }

    static const std::string &NbHttpRequestFactory_GetAppKey(const NbHttpRequestFactory &factory) {
        return factory.app_key_;
    }

    static const std::string &NbHttpRequestFactory_GetSessionToken(const NbHttpRequestFactory &factory) {
        return factory.session_token_;
    }

    static const std::string &NbHttpRequestFactory_GetProxy(const NbHttpRequestFactory &factory) {
        return factory.proxy_;
    }
};
}//namespace necbaas

#endif //NECBAAS_TESTUTIL_H
