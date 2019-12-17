#include "gtest/gtest.h"
#include "ft_data.h"
#include "ft_util.h"
#include "necbaas/nb_user.h"

using std::string;
using std::vector;
using std::shared_ptr;

namespace necbaas {

namespace FTUtil {
string CreateUser(shared_ptr<NbService> service, const string &user_name, const string &email, const string &password, const NbJsonObject &options);
void CreateGroup(shared_ptr<NbService> service, const string &group);
void UpdateGroup(shared_ptr<NbService> service, const string &group, const NbJsonObject &user_list);
}

namespace ssl {

class NbHttpOptionsFT : public ::testing::Test {
  protected:
    static void SetUpTestCase() {
        NbJsonObject users;
        NbJsonArray user_list;
        auto master_service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
        master_service->SetHttpOptions(NbHttpOptions()
                                           .SslVerifyPeer(false)
                                           .SslCert(FTUtil::MakeFilePath(kSslCertFile))
                                           .SslKey(FTUtil::MakeFilePath(kSslKeyFile)));

        user_list.Append(FTUtil::CreateUser(master_service, kUserName, kEmail, kPassword, kOptions));
        users.PutJsonArray("users", user_list);
        FTUtil::CreateGroup(master_service, "group1");
        FTUtil::UpdateGroup(master_service, "group1", users);
    }
    static void TearDownTestCase() {}

    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(NbHttpOptionsFT, ClientAuthSslTest) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    service->SetHttpOptions(NbHttpOptions()
        .SslVerifyPeer(false)
        .SslCert(FTUtil::MakeFilePath(kSslCertFile))
        .SslKey(FTUtil::MakeFilePath(kSslKeyFile)));

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);
    // 通信成功でＯＫとする
    ASSERT_TRUE(result.IsSuccess());
    NbUser::Logout(service);
}

TEST_F(NbHttpOptionsFT, ClientAuthWrongCertTypeSslTest) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    service->SetHttpOptions(NbHttpOptions()
                                .SslVerifyPeer(false)
                                .SslCert(FTUtil::MakeFilePath(kSslCertFile))
                                .SslKey(FTUtil::MakeFilePath(kSslKeyFile))
                                .SslCertType("DER"));

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);
    ASSERT_FALSE(result.IsSuccess());
    NbUser::Logout(service);
}

TEST_F(NbHttpOptionsFT, VerifyPeerSslTest) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    service->SetHttpOptions(NbHttpOptions()
                                .SslVerifyPeer(false)
                                .SslCert(FTUtil::MakeFilePath(kSslCertFile))
                                .SslKey(FTUtil::MakeFilePath(kSslKeyFile)));

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);
    ASSERT_TRUE(result.IsSuccess());
    NbUser::Logout(service);

    service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    service->SetHttpOptions(NbHttpOptions()
                                .SslVerifyPeer(true)
                                .SslCert(FTUtil::MakeFilePath(kSslCertFile))
                                .SslKey(FTUtil::MakeFilePath(kSslKeyFile)));
    result = NbUser::LoginWithUsername(service, kUserName, kPassword);
    ASSERT_FALSE(result.IsSuccess());
    NbUser::Logout(service);
}

TEST_F(NbHttpOptionsFT, CaInfoSslTest) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    service->SetHttpOptions(NbHttpOptions()
                                .SslVerifyPeer(true)
                                .CaInfo(FTUtil::MakeFilePath(kSslCaCertFile))
                                .SslCert(FTUtil::MakeFilePath(kSslCertFile))
                                .SslKey(FTUtil::MakeFilePath(kSslKeyFile)));

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);
    // 通信成功でＯＫとする
    ASSERT_TRUE(result.IsSuccess());
    NbUser::Logout(service);
}

TEST_F(NbHttpOptionsFT, CaPathSslTest) {
    shared_ptr<NbService> service = NbService::CreateService(kEndPointUrl, kTenantId, kAppId, kAppKey, kProxy);
    service->SetHttpOptions(NbHttpOptions()
                                .SslVerifyPeer(true)
                                .CaPath(FTUtil::MakeFilePath(kSslCaCertDir))
                                .SslCert(FTUtil::MakeFilePath(kSslCertFile))
                                .SslKey(FTUtil::MakeFilePath(kSslKeyFile)));

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, kUserName, kPassword);
    // 通信成功でＯＫとする
    ASSERT_TRUE(result.IsSuccess());
    NbUser::Logout(service);
}

} //namespace ssl
} //namespace necbaas
