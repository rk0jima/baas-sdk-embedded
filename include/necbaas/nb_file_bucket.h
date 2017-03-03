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

#ifndef NECBAAS_NBFILEBUCKET_H
#define NECBAAS_NBFILEBUCKET_H

#include <string>
#include <memory>
#include "necbaas/nb_service.h"
#include "necbaas/nb_file_metadata.h"

namespace necbaas {

/**
 * @class NbFileBucket nb_file_bucket.h "necbaas/nb_file_bucket.h"
 * ファイルバケット.
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbFileBucket {
   public:
    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>コンストラクタ.</p>
     */
    NbFileBucket();

    /**
     * コンストラクタ.
     * @param[in]   service        サービスインスタンス
     * @param[in]   bucket_name    バケット名
     */
    NbFileBucket(const std::shared_ptr<NbService> &service, const std::string &bucket_name);

    /**
     * デストラクタ.
     */
    ~NbFileBucket();

    /**
     * ファイルダウンロード.
     * file_pathにファイルが存在する場合、上書き保存となる。<br>
     * ダウンロード中にエラーを検出した場合、その時点までのデータが保存されたファイルが作成される。<br>
     * file_name, file_pathが空文字の場合、パラメータ不正のエラーを返す。<br>
     * bucket_nameが空文字の場合、バケット名不正のエラーを返す。
     * @param[in]   file_name     ダウンロードするファイルの名前
     * @param[in]   file_path     ダウンロードしたファイルの保存先
     * @return      処理結果
     */
    NbResult<int> DownloadFile(const std::string &file_name, const std::string &file_path);

    /**
     * ファイルの新規アップロード.
     * file_name, file_path, content_typeが空文字の場合、パラメータ不正のエラーを返す。<br>
     * bucket_nameが空文字の場合、バケット名不正のエラーを返す。<br>
     * 明示的にACLを設定しない場合はオーバーロードメソッドを使用すること。
     * @param[in]   file_name     アップロードするファイルの名前
     * @param[in]   file_path     アップロードするローカルファイルのパス
     * @param[in]   content_type  Content-Type
     * @param[in]   acl           ACL
     * @param[in]   cache_disable キャッシュ禁止フラグ
     * @return      処理結果
     */
    NbResult<NbFileMetadata> UploadNewFile(const std::string &file_name, const std::string &file_path,
                                           const std::string &content_type, const NbAcl &acl,
                                           bool cache_disable = false);

    /**
     * ファイルの新規アップロード(ACLなし).
     * file_name, file_path, content_typeが空文字の場合、パラメータ不正のエラーを返す。<br>
     * bucket_nameが空文字の場合、バケット名不正のエラーを返す。
     * @param[in]   file_name     アップロードするファイルの名前
     * @param[in]   file_path     アップロードするローカルファイルのパス
     * @param[in]   content_type  Content-Type
     * @param[in]   cache_disable キャッシュ禁止フラグ
     * @return      処理結果
     */
    NbResult<NbFileMetadata> UploadNewFile(const std::string &file_name, const std::string &file_path,
                                           const std::string &content_type, bool cache_disable = false);

    /**
     * ファイルの更新アップロード.
     * metadata内に設定されたfile_name等の情報を使用してファイルアップロードを行う。<br>
     * metadata内の file_name, file_path, content_typeが空文字の場合、パラメータ不正のエラーを返す。<br>
     * bucket_nameが空文字の場合、バケット名不正のエラーを返す。<br>
     * @param[in]   file_path     アップロードするローカルファイルのパス
     * @param[in]   metadata      メタデータ
     * @return      処理結果
     */
    NbResult<NbFileMetadata> UploadUpdateFile(const std::string &file_path, const NbFileMetadata &metadata);

    /**
     * ファイルの更新アップロード.
     * file_name, file_path, content_typeが空文字の場合、パラメータ不正のエラーを返す。<br>
     * bucket_nameが空文字の場合、バケット名不正のエラーを返す。<br>
     * content_type, meta_etag, file_etagを設定しない場合は、空文字を設定すること。
     * @param[in]   file_name     アップロードするファイルの名前
     * @param[in]   file_path     アップロードするローカルファイルのパス
     * @param[in]   content_type  Content-Type
     * @param[in]   meta_etag     メタデータのETag
     * @param[in]   file_etag     ファイル本体のデータのETag
     * @return      処理結果
     */
    NbResult<NbFileMetadata> UploadUpdateFile(const std::string &file_name, const std::string &file_path,
                                              const std::string &content_type, const std::string &meta_etag,
                                              const std::string &file_etag);

    /**
     * ファイル削除.
     * metadata内に設定されたfile_name等の情報を使用してファイル削除を行う。<br>
     * metadata内の file_name が空文字の場合、パラメータ不正のエラーを返す。<br>
     * bucket_nameが空文字の場合、バケット名不正のエラーを返す。
     * @param[in]   metadata      メタデータ
     * @param[in]   delete_mark   削除マークのみを行う
     * @return      処理結果
     */
    NbResult<NbJsonObject> DeleteFile(const NbFileMetadata &metadata, bool delete_mark = false);

    /**
     * ファイル削除.
     * file_nameが空文字の場合、パラメータ不正のエラーを返す。<br>
     * bucket_nameが空文字の場合、バケット名不正のエラーを返す。<br>
     * meta_etag, file_etagを設定しない場合は、空文字を設定すること。
     * @param[in]   metadata      メタデータ
     * @param[in]   delete_mark   削除マークのみを行う
     * @return      処理結果
     */
    NbResult<NbJsonObject> DeleteFile(const std::string &file_name, const std::string &meta_etag,
                                      const std::string &file_etag, bool delete_mark = false);

    /**
      * ファイル一覧取得.
      * bucket_nameが空文字の場合、バケット名不正のエラーを返す。
      * @param[in]   published     公開済みファイルのメタデータ一覧の取得用のフラグ
      * @param[in]   delete_mark   削除マークされたデータを読み込む
      * @return      処理結果
      */
    NbResult<std::vector<NbFileMetadata>> GetFiles(bool published = false, bool deleteMark = false);

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

   private:
    std::shared_ptr<NbService> service_; /*!< サービスインスタンス   */
    int timeout_{kRestTimeoutDefault};   /*!< RESTタイムアウト(秒)   */
    std::string bucket_name_;            /*!< バケット名             */

    /**
     * ファイルの新規アップロード(内部).
     * file_name, file_path, content_typeが空文字の場合、パラメータ不正のエラーを返す。<br>
     * bucket_nameが空文字の場合、バケット名不正のエラーを返す。<br>
     * ACLを設定しない場合は空文字を設定する。
     * @param[in]   file_name     アップロードするファイルの名前
     * @param[in]   file_path     アップロードするローカルファイルのパス
     * @param[in]   content_type  Content-Type
     * @param[in]   acl           ACL
     * @param[in]   cache_disable キャッシュ禁止フラグ
     * @return      処理結果
     */
    NbResult<NbFileMetadata> UploadNewFile(const std::string &file_name, const std::string &file_path,
                                           const std::string &content_type, const std::string &acl, bool cache_disable);
};
}  // namespace necbaas
#endif  // NECBAAS_NBFILEBUCKET_H
