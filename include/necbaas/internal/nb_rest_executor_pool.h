/*
 * Copyright (C) 2017 NEC Corporation
 */

#ifndef NECBAAS_NBRESTEXECUTORPOOL_H
#define NECBAAS_NBRESTEXECUTORPOOL_H

#include <string>
#include <list>
#include <stack>
#include <mutex>
#include "necbaas/internal/nb_rest_executor.h"
#include "necbaas/internal/nb_constants.h"

namespace necbaas {

/**
 * @class NbRestExecutorPool nb_rest_executor_pool.h "necbaas/internal/nb_rest_executor_pool.h"
 * RestExecutorプール.
 * RestExecutorの払い出し、返却処理を提供する。
 * 同時に払い出し可能なRestExecutorの上限を設け、上限に達した場合は払い出しが失敗する。
 */
class NbRestExecutorPool {
  public:
    /**
     * コンストラクタ.
     * @param[in]   http_connection_max       HTTP同時接続数最大値
     */
    explicit NbRestExecutorPool(int http_connection_max = kHttpConnectionMax);

    /**
     * デストラクタ.
     */
    ~NbRestExecutorPool();

    /**
     * RestExecutorの払い出し
     * @return      RestExecutor
     * @retval      nullptr以外  払い出し成功
     * @retval      nullptr      HTTP同時接続数オーバー
     */
    NbRestExecutor *PopRestExecutor();

    /**
     * RestExecutorの返却.
     * @param[in]   rest_executor    RestExecutor
     */
    void PushRestExecutor(NbRestExecutor *rest_executor);

    // コピーとムーブを禁止
    NbRestExecutorPool(NbRestExecutorPool const&) = delete;
    NbRestExecutorPool& operator =(NbRestExecutorPool const&) = delete;
    NbRestExecutorPool(NbRestExecutorPool&&) = delete;
    NbRestExecutorPool& operator =(NbRestExecutorPool&&) = delete;

  protected:
    std::stack<NbRestExecutor*> idle_stack_;   /*!< 使用可能RestExecutorのスタック */
    int creatable_num_;                        /*!< 生成可能RestExecutorの残数 */
    std::mutex stack_mutex_;                   /*!< スタック用Mutex */
};
} //namespace necbaas

#endif //NECBAAS_NBRESTEXECUTORPOOL_H
