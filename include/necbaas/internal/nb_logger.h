/*
 * COPYRIGHT (C) 2017 NEC CORPORATION
 *
 * ALL RIGHTS RESERVED BY NEC CORPORATION,  THIS PROGRAM
 * MUST BE USED SOLELY FOR THE PURPOSE FOR WHICH IT WAS
 * FURNISHED BY NEC CORPORATION,  NO PART OF THIS PROGRAM
 * MAY BE REPRODUCED OR DISCLOSED TO OTHERS,  IN ANY FORM
 * WITHOUT THE PRIOR WRITTEN PERMISSION OF NEC CORPORATION.
 *
 * NEC CONFIDENTIAL AND PROPRIETARY
 */

#ifndef NECBAAS_NBLOGGER_H
#define NECBAAS_NBLOGGER_H

#include "easylogging++.h"

namespace necbaas {

//NBLOGのdefine定義のため、定数定義にdefineを使用する
#define LOGGER_ID "necbaas"                   /*!< Logger ID */
#define NBLOG(level) CLOG(level, LOGGER_ID)   /*!< LOG関数   */

/**
 * @class NbLogger nb_logger.h "necbaas/nb_logger.h"
 * ロギングクラス.
 */
class NbLogger {
  public:

    static void SetDebugLogEnabled(bool flag);
    static void SetErrorLogEnabled(bool flag);
    static void SetRestLogEnabled(bool flag);
    static bool IsDebugLogEnabled();
    static bool IsErrorLogEnabled();
    static bool IsRestLogEnabled();

    /**
     * コンストラクタ.
     */
    NbLogger();

    /**
     * デストラクタ.
     */
    ~NbLogger();

    el::Configurations  log_config_;
};
} //namespace necbaas

#endif //NECBAAS_NBLOGGER_H
