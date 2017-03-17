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

#ifndef NECBAAS_NBUSER_H
#define NECBAAS_NBUSER_H

#include <string>
#include <memory>
#include <ctime>
#include "necbaas/nb_service.h"
#include "necbaas/nb_result.h"
#include "necbaas/nb_json_object.h"
#include "necbaas/internal/nb_user_entity.h"

namespace necbaas {

/**
 * @class NbUser nb_user.h "necbaas/nb_user.h"
 * ユーザクラス.
 * ユーザ情報の管理、認証を行うクラス.<br>
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbUser {
   public:
    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>コンストラクタ.</p>
     */
    NbUser();

    /**
     * コンストラクタ.
     * @param[in]   service    サービスインスタンス
     */
    explicit NbUser(const std::shared_ptr<NbService> &service);

    /**
     * デストラクタ.
     */
    ~NbUser();

    /**
     * ログイン(ユーザ名).
     * Nebulaサービスへログインを行う。<br>
     * NbServiceインスタンスに対して1ログインのみ有効である。<br>
     * ログインに成功した場合は、ログアウトするか
     * 次のログイン処理が成功するまでログイン状態が維持される<br>
     * ユーザネームまたはパスワードが空文字の場合はパラメータエラーを返す。
     * @param[in]   service     サービスインスタンス
     * @param[in]   username    ユーザ名
     * @param[in]   password    パスワード
     * @param[in]   timeout     RESTタイムアウト(秒)
     * @return      処理結果
     */
    static NbResult<NbUser> LoginWithUsername(const std::shared_ptr<NbService> &service, const std::string &username,
                                              const std::string &password, int timeout = kRestTimeoutDefault);
    /**
     * ログイン(E-mail).
     * Nebulaサービスへログインを行う。<br>
     * NbServiceインスタンスに対して1ログインのみ有効である。<br>
     * ログインに成功した場合は、ログアウトするか
     * 次のログイン処理が成功するまでログイン状態が維持される。<br>
     * E-mailアドレスまたはパスワードが空文字の場合はパラメータエラーを返す。
     * @param[in]   service     サービスインスタンス
     * @param[in]   email       E-mailアドレス
     * @param[in]   password    パスワード
     * @param[in]   timeout     RESTタイムアウト(秒)
     * @return      処理結果
     */
    static NbResult<NbUser> LoginWithEmail(const std::shared_ptr<NbService> &service, const std::string &email,
                                           const std::string &password, int timeout = kRestTimeoutDefault);

    /**
     * ログアウト.
     * セッショントークンを破棄し、ログイン状態を解除する。<br>
     * 未ログインの場合は、未ログインエラーを返す。
     * @param[in]   service     サービスインスタンス
     * @param[in]   timeout     RESTタイムアウト(秒)
     * @return      処理結果。成功時はNbUserのユーザIDのみ有効。
     */
    static NbResult<NbUser> Logout(const std::shared_ptr<NbService> &service, int timeout = kRestTimeoutDefault);

    /**
     * ログイン状態確認.
     * ログイン状態の確認を行う。<br>
     * セッショントークンの有効期限が切れている場合も未ログインとなる。
     * @param[in]   service     サービスインスタンス
     * @return  確認結果
     * @retval  true    ログイン中
     * @retval  false   未ログイン
     */
    static bool IsLoggedIn(const std::shared_ptr<NbService> &service);

    /**
     * ログイン有効期限取得.
     * ログインの有効期限を取得する。<br>
     * 未ログイン状態の場合は、0 が返る。
     * @param[in]   service     サービスインスタンス
     * @return  ログイン有効期限（UNIX Time）
     */
    static std::time_t GetSessionTokenExpiration(const std::shared_ptr<NbService> &service);

    /**
     * セッショントークン取得.
     * ログイン中のセッショントークンを取得する。<br>
     * 未ログイン状態の場合は、空文字が返る。
     * @param[in]   service     サービスインスタンス
     * @return  セッショントークン
     */
    static const std::string GetSessionToken(const std::shared_ptr<NbService> &service);

    /**
     * ログインユーザ情報取得.
     * 現在ログインしているユーザの情報を取得する(キャッシュから読み出す)<br>
     * 未ログイン状態の場合は、ユーザ名等が全て空のNbUserオブジェクト
     * @param[in]   service     サービスインスタンス
     * @return  ユーザ情報
     */
    static NbUser GetCurrentUser(const std::shared_ptr<NbService> &service);

    /**
     * ログインユーザのExport.
     * 現在ログインしているユーザの情報をExportする。<br>
     * ログインしていない場合は、空の文字列が返る。
     * @return  Exportデータ
     */
    static std::string ExportCurrentLogin(const std::shared_ptr<NbService> &service);

    /**
     * ログインユーザのImport.
     * 現在ログインしているユーザの情報をimportする。<br>
     * Exportしたデータを入力することで強制的にログイン状態を書き換える。<br>
     * セッショントークンが空文字列の場合は、パラメータエラーを返す。<br>
     * セッショントークンの有効期限が切れている場合は、有効期限切れのエラーを返す。
     * @param[in]   import     Importデータ
     * @return      処理結果
     * @retval      NbResultCode::NB_OK                         Import成功
     * @retval      NbResultCode::NB_ERROR_INVALID_ARGUMENT     パラメータエラー
     * @retval      NbResultCode::NB_ERROR_SESSION_EXPIRED      セッショントークン有効期限切れ
     *
     */
    static NbResultCode ImportCurrentLogin(const std::shared_ptr<NbService> &service, const std::string &import);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>ユーザ情報設定.</p>
     * @param[in]   entity    ユーザ情報
     */
    void SetUserEntity(const NbUserEntity &entity);

    /**
     * ユーザ名取得.
     * @return  ユーザ名
     */
    const std::string &GetUserName() const;

    /**
     * E-mail取得.
     * @return  E-mail
     */
    const std::string &GetEmail() const;

    /**
     * オプション取得.
     * @return  オプション
     */
    const NbJsonObject &GetOptions() const;

    /**
     * 所属グループ取得.
     * @return  グループコンテナ
     */
    std::vector<std::string> GetGroups() const;

    /**
     * ユーザID取得.
     * @return  ユーザID
     */
    const std::string &GetUserId() const;

    /**
     * ユーザ作成日時取得.
     * std::tm の tm_year が 0 の場合は無効データ。
     * @return  ユーザ作成日時(UTC)
     */
    std::tm GetCreatedTime() const;

    /**
     * ユーザ更新日時取得.
     * std::tm の tm_year が 0 の場合は無効データ。
     * @return  ユーザ更新日時(UTC)
     */
    std::tm GetUpdatedTime() const;

   private:
    std::shared_ptr<NbService> service_; /*!< サービスインスタンス */
    NbUserEntity user_entity_;           /*!< ユーザ情報           */

    /**
     * ログイン(共通処理).
     * @param[in]   service     サービスインスタンス
     * @param[in]   username    ユーザ名
     * @param[in]   email       E-mailアドレス
     * @param[in]   password    パスワード
     * @param[in]   timeout     RESTタイムアウト(秒)
     * @return      処理結果
     */
    static NbResult<NbUser> Login(const std::shared_ptr<NbService> &service, const std::string &username,
                                  const std::string &email, const std::string &password, int timeout);

    /**
     * ログインユーザ情報設定.
     * RESTのレスポンスからログイン情報を作成し、サービスに登録する。
     * @param[in]   service     サービスインスタンス
     * @param[in]   json        Jsonオブジェクト形式のログインユーザ情報
     */
    static void SetCurrentUser(const std::shared_ptr<NbService> &service, NbJsonObject json);
};
}  // namespace necbaas
#endif  // NECBAAS_NBUSER_H
