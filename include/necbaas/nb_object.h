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

#ifndef NECBAAS_NBOBJECT_H
#define NECBAAS_NBOBJECT_H

#include <string>
#include <memory>
#include "necbaas/nb_service.h"
#include "necbaas/nb_acl.h"
#include "necbaas/nb_json_object.h"

namespace necbaas {

/**
 * @class NbObject nb_object.h "necbaas/nb_object.h"
 * オブジェクト管理クラス.
 * オブジェクトストレージに格納される Json オブジェクトデータ1件を表現する。<br>
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbObject : public NbJsonObject {
   public:
    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>コンストラクタ.</p>
     */
    NbObject();

    /**
     * コンストラクタ.
     * @param[in]   service        サービスインスタンス
     * @param[in]   bucket_name    バケット名
     */
    NbObject(const std::shared_ptr<NbService> &service, const std::string &bucket_name);

    /**
     * デストラクタ.
     */
    virtual ~NbObject();

    /**
     * オブジェクトを部分更新する.
     * 部分更新用Jsonオブジェクトが空の場合は、パラメータエラーを返す。<br>
     * インスタンスに設定されているバケット名、オブジェクトIDが空文字の場合は、エラーを返す。<br>
     * aclがtrueの場合は、インスタンスに設定されているデータを使用して更新する。<br>
     * createdAtは、自インスタンスに設定されているデータを使用して更新する。空文字の場合は更新しない。<br>
     * 部分更新用Jsonオブジェクトに予約名（_id, createdAt, updatedAt, ACL, etag,
     * _deleted）が使用されている場合は削除する。
     * @param[in]   json        部分更新用Jsonオブジェクト
     * @param[in]   acl         ACL更新フラグ
     * @return      処理結果
     */
    NbResult<NbObject> PartUpdateObject(const NbJsonObject &json, bool acl = false);

    /**
     * オブジェクトを削除する.
     * インスタンスに設定されているバケット名、オブジェクトIDが空文字の場合は、エラーを返す。<br>
     * インスタンスにETagが設定されている場合、ETagがリクエストパラメータに設定される。<br>
     * delete_markがtureの場合、削除マークのみを行う。
     * @param[in]   delete_mark   削除マークのみを行うフラグ
     * @return      処理結果
     */
    NbResult<NbObject> DeleteObject(bool delete_mark = false);

    /**
     * オブジェクトを保存する.
     * インスタンスに設定されているバケット名が空文字の場合は、エラーを返す。<br>
     * インスタンスに設定されているオブジェクトIDが空文字の場合は新規作成、空文字でない場合は対象IDを完全上書きする。<br>
     * <br><b>新規作成の場合</b><br>
     * 作成用のJsonオブジェクトに予約名（_id, createdAt, updatedAt, ACL, etag,
     * _deleted）が使用されている場合は削除する。<br>
     * aclがtrueの場合、インスタンスに設定されているデータを使用する。<br>
     * <br><b>完全上書きの場合</b><br>
     * 作成用のJsonオブジェクトに予約名（_id, createdAt, updatedAt, ACL, etag,
     * _deleted）が使用されている場合は削除する。<br>
     * aclはフラグに関係なく、インスタンスに設定されているデータを使用して更新する。<br>
     * createdAtは、自インスタンスに設定されているデータを使用して更新する。空文字の場合は更新しない。<br>
     * インスタンスにETagが設定されている場合、ETagがリクエストパラメータに設定される。
     * @param[in]   acl         ACL更新フラグ
     * @return      処理結果
     */
    NbResult<NbObject> Save(bool acl = false);

    /**
     * RESTタイムアウト取得.
     * @return      タイムアウト(秒)
     */
    int GetTimeout() const;

    /**
     * RESTタイムアウト設定.
     * RESTのタイムアウト値(秒)を設定する。<br>
     * 0以下の値が設定された場合は、デフォルト値(60秒)が設定される。
     * @param[in]   timeout        タイムアウト(秒)
     */
    void SetTimeout(int timeout);

    /**
     * バケット名取得.
     * @return      バケット名
     */
    const std::string &GetBucketName() const;

    /**
     * オブジェクトID取得.
     * @return      オブジェクトID
     */
    const std::string &GetObjectId() const;

    /**
     * オブジェクトID設定.
     * @param[in]   object_id        オブジェクトID
     */
    void SetObjectId(const std::string &object_id);

    /**
     * オブジェクトの作成日時取得.
     * std::tm の tm_year が 0 の場合は無効データ。
     * @return      オブジェクトの作成日時(UTC)
     */
    const std::tm GetCreatedTime() const;

    /**
     * オブジェクトの更新日時取得.
     * std::tm の tm_year が 0 の場合は無効データ。
     * @return      オブジェクトの更新日時(UTC)
     */
    const std::tm GetUpdatedTime() const;

    /**
     * オブジェクトの作成日時設定.
     * std::tm の tm_year が 0 の場合は無効データを設定する。
     * @param[in]   created_time        オブジェクトの作成日時(UTC)
     */
    void SetCreatedTime(const std::tm &created_time);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>オブジェクトの作成日時設定(文字列).</p>
     * @param[in]   created_time        オブジェクトの作成日時(UTC)
     */
    void SetCreatedTime(const std::string &created_time);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>オブジェクトの更新日時設定(文字列).</p>
     * @param[in]   created_time        オブジェクトの更新日時(UTC)
     */
    void SetUpdatedTime(const std::string &updated_time);

    /**
     * ETag取得.
     * @return      ETag
     */
    const std::string &GetETag() const;

    /**
     * ETag設定.
     * @param[in]   etag     ETag
     */
    void SetETag(const std::string &etag);

    /**
     * ACL取得.
     * @return      ACL
     */
    const NbAcl &GetAcl() const;

    /**
     * ACL設定.
     * @param[in]   acl      ACL
     */
    void SetAcl(const NbAcl &acl);

    /**
     * 削除マーク付与確認.
     * @return          削除マーク
     * @retval  true    削除マークあり
     * @retval  false   削除マークなし
     */
    bool IsDeleteMark() const;

    /**
     * オブジェクトデータ設定.
     * Jsonオブジェクトを設定する。
     * @param[in]   json      Jsonオブジェクト
     */
    void SetObjectData(const NbJsonObject &json);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>全データ設定.</p>
     * 予約名を含めたJsonオブジェクトを設定する。REST結果からJsonオブジェクトを作成する際に使用される。
     * @return      ACL
     */
    void SetCurrentParam(const NbJsonObject &json);

   private:
    std::shared_ptr<NbService> service_; /*!< サービスインスタンス   */
    int timeout_{kRestTimeoutDefault};   /*!< RESTタイムアウト(秒)   */
    std::string bucket_name_{};          /*!< バケット名             */
    std::string object_id_{};            /*!< オブジェクトID         */
    std::string created_time_{};         /*!< オブジェクトの作成日時 */
    std::string updated_time_{};         /*!< オブジェクトの更新日時 */
    std::string etag_{};                 /*!< ETag                   */
    NbAcl acl_{};                        /*!< ACL                    */
    bool deleted_{false};                /*!< 削除マーク             */

    /**
     * 予約名フィールド削除.
     * @param[in]   json      Jsonオブジェクト
     */
    static void RemoveReservationFields(NbJsonObject *json);
};
}  // namespace necbaas
#endif  // NECBAAS_NBOBJECT_H
