/*
 * Copyright (C) 2017 NEC Corporation
 */

#ifndef NECBAAS_NBRESULT_H
#define NECBAAS_NBRESULT_H

#include <string>
#include "necbaas/nb_rest_error.h"
#include "necbaas/nb_result_code.h"

namespace necbaas {

/**
 * @class NbResult nb_result.h "necbaas/nb_result.h"
 * REST API処理結果クラス.
 * REST APIの処理結果の格納用として使用される.<br>
 * 本クラス内で有効なデータは、以下で判定する.
 * - IsSuccess()がtrueの場合   : GetSuccessData()で処理成功データが取得可能
 * - IsRestError()がtrueの場合 : GetRestError()でRESTエラーデータが取得可能
 * - IsFatalError()がtrueの場合: GetResultCode()でエラー要因を確認
 */
template <typename T>
class NbResult {
   public:
    /**
     * コンストラクタ.
     */
    NbResult() {};

    /**
     * コンストラクタ.
     * @param[in]   result_code    処理結果コード
     */
    explicit NbResult(NbResultCode result_code) : result_code_(result_code) {};

    /**
     * デストラクタ.
     */
    ~NbResult() {};

    /**
     * 処理結果成功判定.
     * @return  判定結果
     * @retval  true    処理成功(ステータスコードが200台)
     * @retval  false   処理失敗
     */
    bool IsSuccess() const {
        return (result_code_ == NbResultCode::NB_OK);
    };

    /**
     * RESTエラー判定.
     * @return  判定結果
     * @retval  true    RESTのステータスコードが200台以外
     * @retval  false   ステータスコードが200台、REST未実行
     */
    bool IsRestError() const {
        return (result_code_ == NbResultCode::NB_ERROR_RESPONSE);
    };

    /**
     * 処理結果エラー判定.
     * @return  判定結果
     * @retval  true    REST実行前のエラー、通信エラー等が発生
     * @retval  false   REST処理成功
     */
    bool IsFatalError() const {
        return (!IsSuccess() && !IsRestError());
    };

    /**
     * 処理結果コード取得.
     * @return  処理結果コード
     */
    NbResultCode GetResultCode() const {
        return result_code_;
    };

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>処理結果コード設定.</p>
     * @param[in]   result_code     処理結果コード
     */
    void SetResultCode(NbResultCode result_code) {
        result_code_ = result_code;
    };

    /**
     * 処理成功データ取得.
     * @return  処理成功データ
     */
    const T &GetSuccessData() const {
        return success_data_;
    };

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>処理成功データ設定.</p>
     * @param[in]   success_data     処理成功データ
     */
    void SetSuccessData(const T &success_data) {
        success_data_ = success_data;
    };

    /**
     * RESTエラーデータ取得.
     * @return  RESTエラーデータ
     */
    const NbRestError &GetRestError() const {
        return rest_error_;
    };

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>RESTエラーデータデータ設定.</p>
     * @param[in]   rest_error     RESTエラーデータ
     */
    void SetRestError(const NbRestError &rest_error) {
        rest_error_ = rest_error;
    };

   private:
    NbResultCode result_code_{NbResultCode::NB_FATAL}; /*!< 処理結果コード */
    T success_data_;                                   /*!< 処理成功データ */
    NbRestError rest_error_;                           /*!< RESTエラーデータ */
};
}  // namespace necbaas
#endif  // NECBAAS_NBRESULT_H
