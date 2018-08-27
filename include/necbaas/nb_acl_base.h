/*
 * Copyright (C) 2017 NEC Corporation
 */

#ifndef NECBAAS_NBACLBASE_H
#define NECBAAS_NBACLBASE_H

#include <string>
#include <list>
#include "necbaas/nb_acl_permission.h"
#include "necbaas/nb_json_object.h"

namespace necbaas {

/**
 * @class NbAclBase nb_acl_base.h "necbaas/nb_acl_base.h"
 * ACL基底クラス.
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbAclBase {
   public:
    /**
     * コンストラクタ.
     */
    NbAclBase();

    /**
     * デストラクタ.
     */
    virtual ~NbAclBase();

    /**
     * 対象権限にユーザ・グループを追加する.
     * ユーザIDまたはグループ名が空文字の場合は追加しない。
     * @param[in]   permission      ACL Permission
     * @param[in]   entry           ユーザIDまたはグループ名
     */
    void AddEntry(NbAclPermission permission, const std::string &entry);

    /**
     * createが許可されたユーザ・グループの一覧を取得する.
     * @return      createが許可されたユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    const std::list<std::string> &GetCreate() const;

    /**
     * deleteが許可されたユーザ・グループの一覧を取得する.
     * @return      deleteが許可されたユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    const std::list<std::string> &GetDelete() const;

    /**
     * readが許可されたユーザ・グループの一覧を取得する.
     * @return      readが許可されたユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    const std::list<std::string> &GetRead() const;

    /**
     * updateが許可されたユーザ・グループの一覧を取得する.
     * @return      updateが許可されたユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    const std::list<std::string> &GetUpdate() const;

    /**
     * writeが許可されたユーザ・グループの一覧を取得する.
     * @return      writeが許可されたユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    const std::list<std::string> &GetWrite() const;

    /**
     * 対象権限からユーザIDを削除する.
     * ユーザIDまたはグループ名が空文字の場合は処理しない。
     * @param[in]   permission      ACL Permission
     * @param[in]   entry           ユーザIDまたはグループ名
     */
    void RemoveEntry(NbAclPermission permission, const std::string &entry);

    /**
     * createを許可するユーザ・グループの一覧を設定する.
     * リスト内の空文字は無視され、空文字でないもののみ設定される。<br>
     * ユーザID/グループ名が重複している場合は１つにまとめられる。
     * @param[in]   create      createを許可するユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    void SetCreate(const std::list<std::string> &create);

    /**
     * deleteを許可するユーザ・グループの一覧を設定する.
     * リスト内の空文字は無視され、空文字でないもののみ設定される。<br>
     * ユーザID/グループ名が重複している場合は１つにまとめられる。
     * @param[in]   del      deleteを許可するユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    void SetDelete(const std::list<std::string> &del);

    /**
     * readを許可するユーザ・グループの一覧を設定する.
     * リスト内の空文字は無視され、空文字でないもののみ設定される。<br>
     * ユーザID/グループ名が重複している場合は１つにまとめられる。
     * @param[in]   read      readを許可するユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    void SetRead(const std::list<std::string> &read);

    /**
     * updateを許可するユーザ・グループの一覧を設定する.
     * リスト内の空文字は無視され、空文字でないもののみ設定される。<br>
     * ユーザID/グループ名が重複している場合は１つにまとめられる。
     * @param[in]   update      updateを許可するユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    void SetUpdate(const std::list<std::string> &update);

    /**
     * writeを許可するユーザ・グループの一覧を設定する.
     * リスト内の空文字は無視され、空文字でないもののみ設定される。<br>
     * ユーザID/グループ名が重複している場合は１つにまとめられる。
     * @param[in]   write      writeを許可するユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    void SetWrite(const std::list<std::string> &write);

    /**
     * JsonからACLをセットする.
     * @param[in]   json        Jsonオブジェクト
     */
    virtual void SetJson(const NbJsonObject &json);

    /**
     * Jsonオブジェクトに変換する.
     * @return   Jsonオブジェクト
     */
    virtual NbJsonObject ToJsonObject() const;

    /**
     * Json文字列に変換する.
     * @return   Json文字列
     */
    virtual std::string ToJsonString() const;

   protected:
    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>ACLリストから重複アイテムを削除する.</p>
     * 尚、リストはソートされる。
     * @param[out]   acl_list    ユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    void SortUnique(std::list<std::string> *acl_list);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>ACLリストにユーザ・グループの一覧を設定する.</p>
     * リスト内の空文字は無視され、空文字でないもののみ設定される。<br>
     * ユーザID/グループ名が重複している場合は１つにまとめられる。
     * @param[in]    entries     ユーザIDまたはグループ名
     * @param[out]   acl_list    ユーザID/グループ名('g:'プレフィクス付き)のリスト
     */
    void SetEntries(const std::list<std::string> &entries, std::list<std::string> *acl_list);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>パーミッションに対応するACLリストの取得.</p>
     * @param[in]   permission        ACL Permission
     * @return      ACLリスト
     */
    virtual std::list<std::string> &GetListByPermission(NbAclPermission permission);

   private:
    // 重複チェックするためにlistを使用
    std::list<std::string> read_;   /*!< read   */
    std::list<std::string> write_;  /*!< write  */
    std::list<std::string> create_; /*!< create */
    std::list<std::string> update_; /*!< update */
    std::list<std::string> delete_; /*!< delete */
};
}  // namespace necbaas

#endif  // NECBAAS_NBACLBASE_H
