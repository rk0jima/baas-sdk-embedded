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

#ifndef NECBAAS_NBOBJECTBUCKET_H
#define NECBAAS_NBOBJECTBUCKET_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "necbaas/nb_service.h"
#include "necbaas/nb_result.h"
#include "necbaas/nb_object.h"
#include "necbaas/nb_query.h"

namespace necbaas {

/**
 * @class NbObjectBucket nb_object_bucket.h "necbaas/nb_object_bucket.h"
 * オブジェクトバケット.
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbObjectBucket {
   public:
    /**
     * コンストラクタ.
     * @param[in]   service        サービスインスタンス
     * @param[in]   bucket_name    バケット名
     */
    NbObjectBucket(const std::shared_ptr<NbService> &service, const std::string &bucket_name);

    /**
     * デストラクタ.
     */
    ~NbObjectBucket();

    /**
     * オブジェクトID検索.
     * オブジェクトIDが空文字の場合は、パラメータエラーを返す。<br>
     * インスタンスに設定されているバケット名が空文字の場合は、バケット名エラーを返す。<br>
     * delete_markがtrueの場合、削除マークされたデータも読み込まれる。
     * @param[in]   object_id     オブジェクトID
     * @param[in]   delete_mark   削除マークされたデータを読み込む
     * @return      処理結果
     */
    NbResult<NbObject> GetObject(const std::string &object_id, bool delete_mark = false);

    /**
     * オブジェクトのクエリ.
     * インスタンスに設定されているバケット名が空文字の場合は、バケット名エラーを返す。<br>
     * 条件に合致した全件数を取得する場合は、countパラメータに値設定用アドレスを設定する。<br>
     * クエリに成功した場合、countに件数が設定される。
     * @param[in]   query       検索条件
     * @param[out]  count       件数取得
     * @return      処理結果
     */
    NbResult<std::vector<NbObject>> Query(const NbQuery &query, int *count = nullptr);

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
     * オブジェクト生成する.
     * @return      新規オブジェクト
     */
    NbObject NewObject() const;

    /**
     * バケット名取得.
     * @return      バケット名
     */
    const std::string &GetBucketName() const;

   private:
    std::shared_ptr<NbService> service_; /*!< サービスインスタンス   */
    int timeout_{kRestTimeoutDefault};   /*!< RESTタイムアウト(秒)   */
    std::string bucket_name_;            /*!< バケット名             */

    /**
     * リクエストパラメータ取得.
     * クエリからリクエストパラメータを生成する。
     * @param[in]   query       検索条件
     * @param[out]  count       件数取得
     * @return      リクエストパラメータ
     */
    std::multimap<std::string, std::string> GetParams(const NbQuery &query, int *count) const;
};
}  // namespace necbaas
#endif  // NECBAAS_NBOBJECTBUCKET_H
