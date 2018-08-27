/*
 * Copyright (C) 2017 NEC Corporation
 */

#include "necbaas/internal/nb_rest_executor_pool.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

using std::string;

NbRestExecutorPool::NbRestExecutorPool(int http_connection_max) : creatable_num_(http_connection_max) {}

NbRestExecutorPool::~NbRestExecutorPool() {
    NbRestExecutor *rest_executor;
    while (!idle_stack_.empty()) {
        rest_executor = idle_stack_.top();
        idle_stack_.pop();
        delete rest_executor;
    }
}

NbRestExecutor *NbRestExecutorPool::PopRestExecutor() {
    std::lock_guard<std::mutex> lock(stack_mutex_);
    NbRestExecutor *rest_executor = nullptr;
    if (idle_stack_.empty()) {
        if (creatable_num_ > 0) {
            rest_executor = new NbRestExecutor();
            --creatable_num_;
        } else {
            NBLOG(ERROR) << "HTTP Connection Over";
        }
    } else {
        rest_executor = idle_stack_.top();
        idle_stack_.pop();
    }

    return rest_executor;
}

void NbRestExecutorPool::PushRestExecutor(NbRestExecutor *rest_executor) {
    std::lock_guard<std::mutex> lock(stack_mutex_);
    idle_stack_.push(rest_executor);
}
}  // namespace necbaas
