/*
 * Copyright (C) 2017 NEC Corporation
 */

#ifndef NECBAAS_NBUSERENTITY_H
#define NECBAAS_NBUSERENTITY_H

#include <string>
#include <list>
#include <ctime>
#include "necbaas/nb_json_object.h"
#include "necbaas/nb_json_array.h"

namespace necbaas {

/**
 * @class NbUserEntity nb_user_entity.h "necbaas/internal/nb_user_entity.h"
 * ユーザ情報格納用クラス.
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbUserEntity {
  public:
    /**
     * コンストラクタ.
     */
    NbUserEntity();

    /**
     * コンストラクタ.
     * @param[in]   json         Jsonオブジェクト
     */
    explicit NbUserEntity(const NbJsonObject &json);

    /**
     * デストラクタ.
     */
    ~NbUserEntity();

    /**
     * ユーザ情報設定.
     * @param[in]   json         Jsonオブジェクト
     */
    void Set(NbJsonObject json);

    /**
     * Jsonオブジェクトに変換.
     * @return      Jsonオブジェクト
     */
    NbJsonObject ToJsonObject() const;

    /**
     * ユーザID取得.
     * @return      ユーザID
     */
    const std::string &GetId() const;

    /**
     * ユーザ名取得.
     * @return      ユーザ名
     */
    const std::string &GetUserName() const;

    /**
     * E-mail取得.
     * @return      E-mail
     */
    const std::string &GetEmail() const;

    /**
     * オプション取得.
     * @return      オプション
     */
    const NbJsonObject &GetOptions() const;

    /**
     * グループ一覧取得.
     * @return      グループ一覧
     */
    std::vector<std::string> GetGroups() const;

    /**
     * 作成日時取得.
     * @return     作成日時(UTC)
     */
    std::tm GetCreatedTime() const;

    /**
     * 更新日時取得.
     * @return     更新日時(UTC)
     */
    std::tm GetUpdatedTime() const;
  private:
    std::string id_{};            /*!< ユーザID */
    std::string username_{};      /*!< ユーザ名*/
    std::string email_{};         /*!< E-mailアドレス */
    NbJsonObject options_;        /*!< オプション情報 */
    NbJsonArray groups_;          /*!< ユーザが所属するグループ一覧 */
    std::string created_time_{};  /*!< ユーザの作成日時 */
    std::string updated_time_{};  /*!< ユーザの更新日時 */
};
} //namespace necbaas
#endif //NECBAAS_NBUSERENTITY_H
