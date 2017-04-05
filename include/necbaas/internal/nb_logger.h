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

#define ELPP_THREAD_SAFE
#define ELPP_STL_LOGGING
#define ELPP_NO_LOG_TO_FILE
#define ELPP_NO_LOG_TO_FILE

#include "necbaas/internal/easylogging++.h"

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
    /**
     * ロギング設定（デバッグログ）.
     * プロセス内の全サービスに対して設定される。<br>
     * default設定: Debug版: 有効、Release版: 無効
     * @param[in]   flag    true:有効／false:無効
     */
    static void SetDebugLogEnabled(bool flag);

    /**
     * ロギング設定（エラーログ）.
     * プロセス内の全サービスに対して設定される。<br>
     * default設定: Debug版: 有効、Release版: 無効
     * @param[in]   flag    true:有効／false:無効
     */
    static void SetErrorLogEnabled(bool flag);

    /**
     * ロギング設定（RESTログ）.
     * プロセス内の全サービスに対して設定される。<br>
     * default設定: Debug版: 有効、Release版: 無効
     * @param[in]   flag    true:有効／false:無効
     */
    static void SetRestLogEnabled(bool flag);

    /**
     * ロギング設定確認（デバッグログ）.
     * @return  設定値
     * @retval  true       有効 
     * @retval  false      無効
     */
    static bool IsDebugLogEnabled();

    /**
     * ロギング設定確認（エラーログ）.
     * @return  設定値
     * @retval  true       有効
     * @retval  false      無効
     */
    static bool IsErrorLogEnabled();

    /**
     * ロギング設定確認（RESTログ）.
     * @return  設定値
     * @retval  true       有効
     * @retval  false      無効
     */
    static bool IsRestLogEnabled();

    /**
     * コンストラクタ.
     */
    NbLogger();

    /**
     * デストラクタ.
     */
    ~NbLogger();

    el::Configurations  log_config_;    /*!< ログコンフィグ */
};
} //namespace necbaas

#endif //NECBAAS_NBLOGGER_H
