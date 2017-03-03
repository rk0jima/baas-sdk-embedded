//
// Created by inoue on 16/12/07.
//

#include <gtest/gtest.h>

#include "json/json.h"

#include "necbaas/NbService.h"
#include "necbaas/internal/NbRestExecutor.h"
#include "necbaas/NbLogger.h"
#include "necbaas/internal/NbHttpRequestBuilder.h"


namespace necbaas {

    class SampleTest : public ::testing::Test {
    protected:


        virtual void SetUp()
        {
            printf("TestFix::SetUp\n");
            necbaas::NbService::init();
            mService =
                    new necbaas::NbService("https://v5beta.antcloud.biz/api/",
                                                 "58465d43241a450d2fb9cf8a", "58465da8241a450d2fb9cf8e","bgxFzSc7WLNDWf0xx6OTWAFN46Tv1QXkoa6vmzyL");

        }
        virtual void TearDown()
        {
            printf("TestFix::TearDown\n");
            necbaas::NbService::disable();

            delete mService;
        }

        necbaas::NbService *mService;

    };

    TEST_F(SampleTest, Sample1Test) {
        // post create new object
        std::string apiPath = "objects";
        apiPath += "/testObjBucket";

        Json::Value writeRoot;
        writeRoot["test"] = "test value";
        Json::FastWriter writer;
        auto bodyJson = writer.write(writeRoot);

        necbaas::internal::NbHttpRequestBuilder
                reqBuilder(mService->getMEndpointUrl(), mService->getMTenantId(), mService->getMAppId(), mService->getMAppKey(),
                           "");
        const std::unique_ptr<necbaas::internal::NbHttpRequest> req = reqBuilder.post(apiPath).body(bodyJson).build();

        necbaas::internal::NbRestExecutor executor;
        executor.executeJsonRequest(*req);

    }

    TEST_F(SampleTest, Sample2Test) {
    }


}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
