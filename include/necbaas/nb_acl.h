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

#ifndef NECBAAS_NBACL_H
#define NECBAAS_NBACL_H

#include <string>
#include <list>
#include <map>
#include "necbaas/nb_acl_base.h"
#include "necbaas/nb_user.h"

namespace necbaas {

/**
 * @class NbAcl nb_acl.h "necbaas/nb_acl.h"
 * ACLクラス.
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbAcl : public NbAclBase {
   public:
    /**
     * anonymousアクセス(R/W/Admin)可能な ACL を生成する.
     * @return      ACL
     */
    static NbAcl CreateAclForAnonymous();

    /**
     * Authenticatedアクセス(R/W/Admin)可能な ACL を生成する.
     * @return      ACL
     */
    static NbAcl CreateAclForAuthenticated();

    /**
     * 特定ユーザのみがアクセス可能な ACL を生成する.
     * ユーザIDが空文字の場合は初期状態のACLが返る。
     * @param[in]   user        ユーザ
     * @return      ACL
     */
    static NbAcl CreateAclForUser(const NbUser &user);

    /**
     * R/W/Adminが同一のACLを生成する.
     * ユーザIDまたはグループ名が空文字の場合は初期状態のACLが返る。
     * @param[in]   entry       ユーザIDまたはグループ名
     * @return      ACL
     */
    static NbAcl CreateAclFor(const std::string &entry);

    /**
     * R/W/Adminが同一のACLを生成する.
     * リスト内の空文字は無視され、空文字でないもののみ設定される。<br>
     * リストが空、リスト内が全て空文字の場合は初期状態のACLが返る。<br>
     * ユーザID/グループ名が重複している場合は１つにまとめられる。
     * @param[in]   entry       ユーザID/グループ名のリスト
     * @return      ACL
     */
    static NbAcl CreateAclFor(std::list<std::string> entries);

    /**
     * コンストラクタ.
     */
    NbAcl();

    /**
     * コンストラクタ.
     * JsonオブジェクトからACLを構築する。
     * @param[in]   json     Jsonオブジェクト
     */
    explicit NbAcl(const NbJsonObject &json);

    /**
     * デストラクタ.
     */
    ~NbAcl();

    /**
     * Admin権限が付加されたユーザ・グループの一覧を取得する.
     * @return      Adminのリスト
     */
    const std::list<std::string> &GetAdmin() const;

    /**
     * オーナのユーザIDを取得する.
     * @return      オーナのユーザID
     */
    const std::string &GetOwner() const;

    /**
     * admin権限を付加するユーザ・グループの一覧を設定する.
     * リスト内の空文字は無視され、空文字でないもののみ設定される。<br>
     * ユーザID/グループ名が重複している場合は１つにまとめられる。
     * @param[in]   admin       admin権限を付加するユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    void SetAdmin(const std::list<std::string> &admin);

    /**
     * オーナのユーザIDを設定する.
     * @param[in]   owner       オーナのユーザID
     */
    void SetOwner(const std::string &owner);

    /**
     * JsonからACLをセットする.
     * @param[in]   json        Jsonオブジェクト
     */
    void SetJson(NbJsonObject json) override;

    /**
     * Jsonオブジェクトに変換する.
     * @param[in]   json        Jsonオブジェクト
     */
    NbJsonObject ToJsonObject() const;

   private:
    std::list<std::string> admin_; /*!< admin */
    std::string owner_;            /*!< owner */

    /**
     * ACL Permissionに対応するユーザ・グループの一覧を取得する.
     * @param[in]   permission        ACL Permission
     * @return      ユーザID/グループ名のリスト
     */
    std::list<std::string> &GetListByPermission(NbAclPermission permission) override;
};
}  // namespace necbaas

#endif  // NECBAAS_NBACL_H
