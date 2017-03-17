#include "gtest/gtest.h"
#include "necbaas/internal/nb_rest_executor_pool.h"

namespace necbaas {

using std::string;
using std::vector;

// 共通テスト
void NbRestExecutorPoolTest(NbRestExecutorPool &executor_pool, int http_connection_max) {
    vector<NbRestExecutor*> executor(http_connection_max);

    for (int i = 0; i < http_connection_max; ++i) {
        executor[i] = executor_pool.PopRestExecutor();
        EXPECT_NE(nullptr, executor[i]);
    }

    //Connection Max
    EXPECT_EQ(nullptr, executor_pool.PopRestExecutor());

    //一つ戻すと取得可能に
    executor_pool.PushRestExecutor(executor[0]);
    EXPECT_EQ(executor[0], executor_pool.PopRestExecutor());

    //全部戻す
    for (int i = 0; i < http_connection_max; ++i) {
        executor_pool.PushRestExecutor(executor[i]);
    }

    //再度取り出す
    for (int i = 0; i < http_connection_max; ++i) {
        // スタックなので、最後に戻したものから取得される
        EXPECT_EQ(executor[http_connection_max - 1 - i], executor_pool.PopRestExecutor());
    }

    //全部戻す(デストラクタ確認用)
    for (int i = 0; i < http_connection_max; ++i) {
        executor_pool.PushRestExecutor(executor[i]);
    }
}

//NbRestExecutorPool スタック操作（最大接続数デフォルト）
TEST(NbRestExecutorPool, NbRestExecutorPoolDefault) {
    NbRestExecutorPool executor_pool;
    NbRestExecutorPoolTest(executor_pool, 20);
}

//NbRestExecutorPool スタック操作（最大接続数10）
TEST(NbRestExecutorPool, NbRestExecutorPool10) {
    NbRestExecutorPool executor_pool(10);
    NbRestExecutorPoolTest(executor_pool, 10);
}

//NbRestExecutorPool デストラクタ（空スタック）
TEST(NbRestExecutorPool, NbRestExecutorPoolEmpty) {
    NbRestExecutorPool *executor_pool = new NbRestExecutorPool();
    delete executor_pool;
}

} //namespace necbaas
