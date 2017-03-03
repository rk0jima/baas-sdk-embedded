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

#ifndef NECBAAS_NBRESTERROR_h
#define NECBAAS_NBRESTERROR_h

#include <string>

namespace necbaas {

/**
 * @struct NbRestError nb_rest_error.h "necbaas/nb_rest_error.h"
 * RESTエラー構造体.
 * RESTがエラー（ステータスコードが200以外）の場合のエラー情報通知用.<br>
 * reasonにはレスポンスのボディ部が文字列で設定される.
 */
struct NbRestError {
    int status_code;    /*!< ステータスコード */
    std::string reason; /*!< エラー理由       */
};
}  // namespace necbaas
#endif  // NECBAAS_NBRESTERROR_h
