#include "necbaas/nb_user.h"
#include "necbaas/internal/nb_logger.h"

using namespace necbaas;

int main() {
    NbService::SetDebugLogEnabled(true);
    NbService::SetErrorLogEnabled(true);
    NbService::SetRestLogEnabled(true);

    std::shared_ptr<NbService> service = NbService::CreateService(
                                      "http://xx.xx.xx.xx:8080/api/",
                                      "58980feb2909b20f69a18463",
                                      "589810262909b20f69a18467",
                                      "IrqJVO5bzj0Mcbx6g415MI3F5idp2CkcNqLAGOwy",
                                      "");

    NbResult<NbUser> result = NbUser::LoginWithUsername(service, "testuser", "test1234");
    NBLOG(DEBUG) << "result: " << (int)result.GetResultCode();

    if (result.IsSuccess()) {
        NBLOG(DEBUG) << "username: " << result.GetSuccessData().GetUserName();
        NBLOG(DEBUG) << "email   : " << result.GetSuccessData().GetEmail();
        NBLOG(DEBUG) << "userid  : " << result.GetSuccessData().GetUserId();
        NBLOG(DEBUG) << "options : " << result.GetSuccessData().GetOptions().ToJsonString();
        for (auto group : result.GetSuccessData().GetGroups()) {
            NBLOG(DEBUG) << "groups  : " << group;
        }
    } else if (result.IsRestError()) {
        NBLOG(DEBUG) << "statuscode: " << result.GetRestError().status_code;
        NBLOG(DEBUG) << "reason    : " << result.GetRestError().reason;
    } else {
        NBLOG(DEBUG) << "fatal: " << (int)result.GetResultCode();
    }

    result = NbUser::Logout(service);

    NBLOG(DEBUG) << "result: " << (int)result.GetResultCode();

    if (result.IsSuccess()) {
        NBLOG(DEBUG) << "username: " << result.GetSuccessData().GetUserName();
        NBLOG(DEBUG) << "email   : " << result.GetSuccessData().GetEmail();
        NBLOG(DEBUG) << "userid  : " << result.GetSuccessData().GetUserId();
        NBLOG(DEBUG) << "options : " << result.GetSuccessData().GetOptions().ToJsonString();
        for (auto group : result.GetSuccessData().GetGroups()) {
            NBLOG(DEBUG) << "groups  : " << group;
        }
    } else if (result.IsRestError()) {
        NBLOG(DEBUG) << "statuscode: " << result.GetRestError().status_code;
        NBLOG(DEBUG) << "reason    : " << result.GetRestError().reason;
    } else {
        NBLOG(DEBUG) << "fatal: " << (int)result.GetResultCode();
    }

    NBLOG(DEBUG) << "main OUT";
}

