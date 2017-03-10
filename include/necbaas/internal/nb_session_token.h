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

#ifndef NECBAAS_NBSESSIONTOKEN_H
#define NECBAAS_NBSESSIONTOKEN_H

#include <string>
#include "necbaas/internal/nb_user_entity.h"

namespace necbaas {

/**
 * @class NbSessionToken nb_session_token.h "necbaas/internal/nb_session_token.h"
 * セッショントークン.
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbSessionToken {
  public:
    /**
     * 有効期限切れ確認.
     * @param[in]   expire_at       有効期限
     * @return  処理結果
     * @retval  true    有効期限切れ
     * @retval  false   有効期限内
     */
    static bool IsExpired(std::time_t expire_at);

    /**
     * コンストラクタ.
     */
    NbSessionToken();

    /**
     * コンストラクタ.
     * @param[in]   session_token   セッショントークン文字列
     * @param[in]   expire_at       有効期限
     */
    NbSessionToken(const std::string &session_token, long expire_at);

    /**
     * デストラクタ.
     */
    ~NbSessionToken();

    /**
     * セッショントークン設定.
     * @param[in]   session_token   セッショントークン文字列
     * @param[in]   expire_at       有効期限
     */
    void SetSessionToken(const std::string &session_token, long expire_at);

    /**
     * セッショントークン取得.
     * @return      セッショントークン文字列
     */
    const std::string &GetSessionToken() const;

    /**
     * 有効期限取得.
     * @return      有効期限
     */
    std::time_t GetExpireAt() const;

    /**
     * ユーザ情報設定.
     * @param[in]   entity   ユーザ情報
     */
    void SetSessionUserEntity(const NbUserEntity &entity);

    /**
     * ユーザ情報取得.
     * @return      ユーザ情報
     */
    const NbUserEntity &GetSessionUserEntity() const;

    /**
     * セッショントークン破棄.
     */
    void ClearSessionToken();
  private:
    std::string session_token_{};        /*!< セッショントークン文字列 */
    std::time_t expire_at_{0};           /*!< 有効期限                 */
    NbUserEntity session_user_entity_;   /*!< ユーザ情報               */
};
} //namespace necbaas
#endif //NECBAAS_NBSESSIONTOKEN_H
