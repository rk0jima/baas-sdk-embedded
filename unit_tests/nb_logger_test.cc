#include "gtest/gtest.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;
using std::vector;

//NbLogger::NbLogger
TEST(NbLogger, NbLogger) {
    // コンストラクタはプロセス起動時に動いているが、テスト順に依存させないため、
    // コンストラクタを再度起動する
    NbLogger logger;
    
    el::Logger* l = el::Loggers::getLogger(LOGGER_ID);

#ifdef NB_DEBUG
    bool init_enabled = true;
#else
    bool init_enabled = false;
#endif
    EXPECT_EQ(init_enabled, NbLogger::IsDebugLogEnabled());
    EXPECT_EQ(init_enabled, NbLogger::IsErrorLogEnabled());
    EXPECT_EQ(init_enabled, NbLogger::IsRestLogEnabled());
    EXPECT_FALSE(l->typedConfigurations()->toFile(el::Level::Global));
    EXPECT_TRUE(l->typedConfigurations()->toStandardOutput(el::Level::Global));
    EXPECT_EQ(3, l->typedConfigurations()->millisecondsWidth(el::Level::Global).m_width);
    EXPECT_FALSE(l->typedConfigurations()->performanceTracking(el::Level::Global));
}

//NbLogger ログ設定・読出
TEST(NbLogger, Settings) {
    vector<bool> flags{true, false, true};
    for (auto flag : flags) {
        NbLogger::SetDebugLogEnabled(flag);
        EXPECT_EQ(flag, NbLogger::IsDebugLogEnabled());

        NbLogger::SetErrorLogEnabled(flag);
        EXPECT_EQ(flag, NbLogger::IsErrorLogEnabled());

        NbLogger::SetRestLogEnabled(flag);
        EXPECT_EQ(flag, NbLogger::IsRestLogEnabled());
    }
}
} //namespace necbaas
