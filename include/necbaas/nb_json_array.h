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

#ifndef NECBAAS_NBJSONARRAY_H
#define NECBAAS_NBJSONARRAY_H

#include <string>
#include <vector>
#include <limits>
#include <json/json.h>
#include "necbaas/nb_json_type.h"
#include "necbaas/internal/nb_logger.h"

namespace necbaas {

// 相互参照のため
class NbJsonObject;

/**
 * @class NbJsonArray nb_json_array.h "necbaas/nb_json_array.h"
 * Json配列.
 * Json形式の配列を提供する。<br>
 * データの取得は、データ型に対応したGetメソッドを使用する。<br>
 * データの設定は、添え字演算子[]かデータ型に対応したPutメソッドを使用する。<br>
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbJsonArray {
   public:
    /**
     * コンストラクタ.
     */
    NbJsonArray();

    /**
     * コンストラクタ.
     * Json文字列をParseしてNbJsonArrayを生成する。<br>
     * Json Parseに失敗した場合は、空のJson配列が生成される。
     * @param[in]   json_string    Json文字列
     */
    explicit NbJsonArray(const std::string &json_string);

    /**
     * デストラクタ.
     */
    ~NbJsonArray();

    /**
     * 全データセット.
     * Json文字列をParseしてNbJsonArray内のデータを再構築する。元々存在していたデータは破棄される。<br>
     * Json Parseに失敗した場合は、空のJson配列が生成される。
     * @param[in]   json    Json文字列
     */
    void PutAll(const std::string &json);

    /**
     * 全データセット（リスト形式）.
     * Json配列に設定するデータ型が全て同じ場合に使用可能。<br>
     * STLコンテナからNbJsonArray内のデータを再構築する。元々存在していたデータは破棄される。<br>
     * 入力データとして使用可能な型は以下の通り。
     * - コンテナ : std::vector, std::list
     * - 要素     : int, unsigned int, int64_t, uint64_t, double, bool, std::string
     * @param[in]   container    STLコンテナ
     */
    template <typename T>
    void PutAllList(const T &container) {
        value_.resize(container.size());
        int i = 0;
        for (auto container_value : container) {
            value_[i] = container_value;
            ++i;
        }
    };

    /**
     * 整数値取得.
     * Indexに対応するValueをint型で取得する。<br>
     * 浮動小数点は丸められる。<br>
     * 以下の場合は、default_valueを返却する。<br>
     * - Indexに対応するValueが存在しない
     * - Valueが数値型でない
     * - Valueがintの範囲外 
     * @param[in]   index           Index
     * @param[in]   default_value   取得できなかったときに返す値
     * @return      Value
     */
    int GetInt(unsigned int index, int default_value = 0) const;

    /**
     * 64bit整数値取得.
     * Indexに対応するValueをint64_t型で取得する。<br>
     * 浮動小数点は丸められる。<br>
     * 以下の場合は、default_valueを返却する。<br>
     * - Indexに対応するValueが存在しない
     * - Valueが数値型でない
     * - Valueがint64_tの範囲外 
     * @param[in]   index           Index
     * @param[in]   default_value   取得できなかったときに返す値
     * @return      Value
     */
    int64_t GetInt64(unsigned int index, int64_t default_value = 0) const;

    /**
     * 浮動小数点値取得.
     * Indexに対応するValueをdouble型で取得する。<br>
     * 以下の場合は、default_valueを返却する。<br>
     * - Indexに対応するValueが存在しない
     * - Valueが数値型でない
     * @param[in]   index           Index
     * @param[in]   default_value   取得できなかったときに返す値
     * @return      Value
     */
    double GetDouble(unsigned int index, double default_value = 0.0) const;

    /**
     * 真偽値取得.
     * Indexに対応するValueをbool型で取得する。<br>
     * 以下の場合は、default_valueを返却する。<br>
     * - Indexに対応するValueが存在しない
     * - Valueが真偽値型でない
     * @param[in]   index           Index
     * @param[in]   default_value   取得できなかったときに返す値
     * @return      Value
     */
    bool GetBoolean(unsigned int index, bool default_value = false) const;

    /**
     * 文字列取得.
     * indexに対応するValueをstd::string型で取得する。<br>
     * 以下の場合は、default_valueを返却する。<br>
     * - Indexに対応するValueが存在しない
     * - Valueが文字列型でない
     * @param[in]   index           Index
     * @param[in]   default_value   取得できなかったときに返す値
     * @return      Value
     */
    std::string GetString(unsigned int index, const std::string &default_value = "") const;

    /**
     * Jsonオブジェクト取得.
     * Indexに対応するValueをNbJsonObject型で取得する。<br>
     * 以下の場合は、空のJsonオブジェクトを返却する。<br>
     * - Indexに対応するValueが存在しない
     * - ValueがJsonオブジェクト型でない
     * @param[in]   index           Index
     * @return      Value
     */
    NbJsonObject GetJsonObject(unsigned int index) const;

    /**
     * Json配列取得.
     * Indexに対応するValueをNbJsonArray型で取得する。<br>
     * 以下の場合は、空のJson配列を返却する。<br>
     * - Indexに対応するValueが存在しない
     * - ValueがJson配列型でない
     * @param[in]   index           Index
     * @return      Value
     */
    NbJsonArray GetJsonArray(unsigned int index) const;

    /**
     * 整数値リスト取得.
     * Json配列に設定されたデータ型が全て数値型の場合に使用可能。<br>
     * 浮動小数点は丸められる。<br>
     * 以下の場合は、該当Indexにdefault_valueを設定する。<br>
     * - Valueが数値型でない
     * - Valueがintの範囲外 
     *
     * OUTパラメータとして使用可能な型は以下の通り。
     * - コンテナ : std::vector<int>, std::list<int>
     * @param[out]  out_container   STLコンテナ。nullptrの場合は処理を行わない。
     * @param[in]   default_value   取得できなかったときに設定する値
     */
    template <typename T>
    void GetAllInt(T *out_container, int default_value = 0) const {
        GetAllList(out_container, default_value, &Json::Value::isNumeric, &Json::Value::asInt);
    };

    /**
     * 64bit整数値リスト取得.
     * Json配列に設定されたデータ型が全て数値型の場合に使用可能。<br>
     * 浮動小数点は丸められる。<br>
     * 以下の場合は、該当Indexにdefault_valueを設定する。<br>
     * - Valueが数値型でない
     * - Valueがint64_tの範囲外 
     *
     * OUTパラメータとして使用可能な型は以下の通り。
     * - コンテナ : std::vector<int64_t>, std::list<int64_t>
     * @param[out]  out_container   STLコンテナ。nullptrの場合は処理を行わない。
     * @param[in]   default_value   取得できなかったときに設定する値
     */
    template <typename T>
    void GetAllInt64(T *out_container, int64_t default_value = 0) const {
        GetAllList(out_container, default_value, &Json::Value::isNumeric, &Json::Value::asInt64);
    };

    /**
     * 浮動小数点値リスト取得.
     * Json配列に設定されたデータ型が全て数値型の場合に使用可能。<br>
     * 以下の場合は、該当Indexにdefault_valueを設定する。<br>
     * - Valueが数値型でない
     *
     * OUTパラメータとして使用可能な型は以下の通り。
     * - コンテナ : std::vector<double>, std::list<double>
     * @param[out]  out_container   STLコンテナ。nullptrの場合は処理を行わない。
     * @param[in]   default_value   取得できなかったときに設定する値
     */
    template <typename T>
    void GetAllDouble(T *out_container, double default_value = 0.0) const {
        GetAllList(out_container, default_value, &Json::Value::isNumeric, &Json::Value::asDouble);
    }

    /**
     * 真偽値リスト取得.
     * Json配列に設定されたデータ型が全て真偽値型の場合に使用可能。<br>
     * 以下の場合は、該当Indexにdefault_valueを設定する。<br>
     * - Valueが真偽値型でない
     *
     * OUTパラメータとして使用可能な型は以下の通り。
     * - コンテナ : std::vector<bool>, std::list<bool>
     * @param[out]  out_container   STLコンテナ。nullptrの場合は処理を行わない。
     * @param[in]   default_value   取得できなかったときに設定する値
     */
    template <typename T>
    void GetAllBoolean(T *out_container, bool default_value = false) const {
        GetAllList(out_container, default_value, &Json::Value::isBool, &Json::Value::asBool);
    }

    /**
     * 文字列リスト取得.
     * Json配列に設定されたデータ型が全て文字列型の場合に使用可能。<br>
     * 以下の場合は、該当Indexにdefault_valueを設定する。<br>
     * - Valueが文字列型でない
     *
     * OUTパラメータとして使用可能な型は以下の通り。
     * - コンテナ : std::vector<std::string>, std::list<std::string>
     * @param[out]  out_container   STLコンテナ。nullptrの場合は処理を行わない。
     * @param[in]   default_value   取得できなかったときに設定する値
     */
    template <typename T>
    void GetAllString(T *out_container, const std::string &default_value = "") const {
        GetAllList(out_container, default_value, &Json::Value::isString, &Json::Value::asString);
    };

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>Json::Value取得.</p>
     * 保持しているJson::Value型のJsonデータを取得する。Jsonオブジェクトを代入する際に使用する。
     * @return      Jsonデータ
     */
    const Json::Value &GetSubstitutableValue() const;

    /**
     * Value設定用添え字演算子.
     * 添え字演算子を使用してIndexに対応するValueを設定する。<br>
     * Indexが存在する場合は、Valueが更新される。<br>
     * Indexが存在しない場合は、配列のサイズを拡張し、Valueが作成される。<br>
     * 設定可能なValueの型は以下
     * - int, unsigned int,
     * - int64_t, uint64_t,
     * - double
     * - bool
     * - std::string
     *
     * NbJsonObject, NbJsonArray の設定は、 PutJsonObject(), PutJsonArray() を使用すること。<br>
     *
     * 使用可能な配列の最大サイズは、UINT_MAXである。UINT_MAX-1を超えるIndexを指定した場合は無視される。
     * @code
       json[0] = 123;
       json[10] = "value";
     * @endcode
     * @param[in]   index             Index
     * @return      代入用内部データへの参照
     */
    Json::Value &operator[](unsigned int index);

    /**
     * Value参照用添え字演算子.
     * 本演算子は未サポート。Valueの取得には、Getメソッドを使用してください。
     */
    const Json::Value &operator[](unsigned int index) const;

    /**
     * Jsonオブジェクト設定.
     * Indexに対応するValueをNbJsonObject型で設定する。
     * IndexにUINT_MAXが設定された場合はfalseを返す。
     * @param[in]   index           Index
     * @param[in]   json_object     Value
     * @return  処理結果
     * @retval  true    処理成功
     * @retval  false   処理失敗
     */
    bool PutJsonObject(unsigned int index, const NbJsonObject &json_object);

    /**
     * Json配列設定.
     * Indexに対応するValueをNbJsonArray型で設定する。
     * IndexにUINT_MAXが設定された場合はfalseを返す。
     * @param[in]   index           Index
     * @param[in]   json_array      Value
     * @return  処理結果
     * @retval  true    処理成功
     * @retval  false   処理失敗
     */
    bool PutJsonArray(unsigned int index, const NbJsonArray &json_array);

    /**
     * null設定.
     * Indexに対応するValueにnullを設定する。
     * IndexにUINT_MAXが設定された場合はfalseを返す。
     * @param[in]   index           Index
     * @return  処理結果
     * @retval  true    処理成功
     * @retval  false   処理失敗
     */
    bool PutNull(unsigned int index);

    /**
     * 配列末尾に追加.
     * 配列の末尾にValueを設定する。
     * 設定可能なValueの型は以下
     * - int, unsigned int,
     * - int64_t, uint64_t,
     * - double
     * - bool
     * - std::string
     *
     * NbJsonObject, NbJsonArray の設定は、 AppendJsonObject(), AppendJsonArray() を使用すること。<br>
     * <br>既に配列のサイズが最大値に達している場合はfalseを返す。
     * @param[in]   value      Value
     * @return  処理結果
     * @retval  true    処理成功
     * @retval  false   処理失敗
     */
    template <typename T>
    bool Append(T value) {
        if (value_.size() == std::numeric_limits<unsigned int>::max()) {
            // UINTの最大値を超える場合は処理しない
            return false;
        }
        value_[value_.size()] = value;
        return true;
    }

    /**
     * 配列末尾に追加(Jsonオブジェクト).
     * 配列の末尾にJsonオブジェクトを設定する。<br>
     * 既に配列のサイズが最大値に達している場合はfalseを返す。
     * @param[in]   json_object      Value
     * @return  処理結果
     * @retval  true    処理成功
     * @retval  false   処理失敗
     */
    bool AppendJsonObject(const NbJsonObject &json_object);

    /**
     * 配列末尾に追加(Json配列).
     * 配列の末尾にJson配列を設定する。<br>
     * 既に配列のサイズが最大値に達している場合はfalseを返す。
     * @param[in]   json_array      Value
     * @return  処理結果
     * @retval  true    処理成功
     * @retval  false   処理失敗
     */
    bool AppendJsonArray(const NbJsonArray &json_array);

    /**
     * 配列末尾に追加(null).
     * 配列の末尾にnullを設定する。<br>
     * 既に配列のサイズが最大値に達している場合はfalseを返す。
     * @return  処理結果
     * @retval  true    処理成功
     * @retval  false   処理失敗
     */
    bool AppendNull();

    /**
     * Json配列サイズ取得.
     * Json配列のサイズを取得する。
     * @return      Json配列サイズ
     */
    unsigned int GetSize() const;

    /**
     * オブジェクト空確認.
     * Json配列が空かどうかを確認する。
     * @return  確認結果
     * @retval  true    空
     * @retval  false   Json配列にValueが存在
     */
    bool IsEmpty() const;

    /**
     * Jsonタイプ取得.
     * Indexに対応するJsonタイプを取得する。
     * @param[in]   index             Index
     * @return      Jsonタイプ
     */
    NbJsonType GetType(unsigned int index) const;

    /**
     * Index削除.
     * Indexに対応するValueを削除する。
     * 削除されたIndex以降のデータは詰めて再設定される。
     * @param[in]   index             Index
     */
    void Remove(unsigned int index);

    /**
     * 全データ削除.
     * 全ての配列を削除する。
     */
    void Clear();

    /**
     * Json文字列変換.
     * 設定されているJson配列をJson文字列に変換する。
     * @return  Json文字列
     */
    std::string ToJsonString() const;

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>全データ置換.</p>
     * 全てのJson配列を置換する。
     * @param[in]   value   置換データ
     */
    void Replace(const Json::Value &value);

    /**
     * ==演算子.
     */
    bool operator==(const NbJsonArray &other) const;

   private:
    Json::Value value_; /*!< Jsonデータ */

    /**
     * リスト形式データ取得.
     * Json配列に設定されたデータ型が全て同じ型の場合に使用可能。<br>
     * Indexに対応するValueが指定の型でない場合は、default_valueを設定する。<br>
     * OUTパラメータとして使用可能な型は以下の通り。
     * - コンテナ : std::vector, std::list
     * - 要素     : int, int64_t, double, bool, std::string
     * @param[out]  out_container  STLコンテナ。nullptrの場合は処理を行わない。
     * @param[in]   default_value   取得できなかったときに設定する値
     * @param[in]   *is_type        型チェック用関数ポインタ
     * @param[in]   *as_type        型変換用関数ポインタ
     */
    template <typename T, typename U>
    void GetAllList(T *out_container, const U &default_value, bool (Json::Value::*is_type)(void) const,
                    U (Json::Value::*as_type)(void) const) const {
        if (out_container == nullptr) {
            return;
        }
        out_container->clear();
        for (unsigned int i = 0; i < GetSize(); ++i) {
            if ((value_[i].*is_type)()) {
                try {
                    out_container->push_back((value_[i].*as_type)());
                }
                catch (const Json::LogicError &ex) {
                    NBLOG(ERROR) << ex.what();
                    out_container->push_back(default_value);
                }
            } else {
                out_container->push_back(default_value);
            }
        }
    };
};
}  // namespace necbaas
#endif  // NECBAAS_NBJSONARRAY_H
