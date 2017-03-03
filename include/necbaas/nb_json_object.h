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

#ifndef NECBAAS_NBJSONOBJECT_H
#define NECBAAS_NBJSONOBJECT_H

#include <string>
#include <json/json.h>
#include "necbaas/nb_json_type.h"

namespace necbaas {

// 相互参照のため
class NbJsonArray;

/**
 * @class NbJsonObject nb_json_object.h "necbaas/nb_json_object.h"
 * Jsonオブジェクト.
 * Json形式のKey-Valueストアを提供する。<br>
 * データの取得は、Valueの型に対応したGetメソッドを使用する。<br>
 * データの設定は、添え字演算子[]かValueの型に対応したPutメソッドを使用する。<br>
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbJsonObject {
   public:
    /**
     * コンストラクタ.
     */
    NbJsonObject();

    /**
     * コンストラクタ.
     * Json文字列をParseしてNbJsonObjectを生成する。<br>
     * Json Parseに失敗した場合は、空のJsonオブジェクトが生成される。
     * @param[in]   json_string    Json文字列
     */
    explicit NbJsonObject(const std::string &json_string);

    /**
     * コンストラクタ.
     * Json文字列をParseしてNbJsonObjectを生成する。<br>
     * Json Parseに失敗した場合は、空のJsonオブジェクトが生成される。
     * @param[in]   json_char    Json文字列
     */
    explicit NbJsonObject(const std::vector<char> &json_char);

    /**
     * デストラクタ.
     */
    virtual ~NbJsonObject();

    /**
     * 全データセット.
     * Json文字列をParseしてNbJsonObject内のデータを再構築する。元々存在していたデータは破棄される。<br>
     * Json Parseに失敗した場合は、空のJsonオブジェクトが生成される。
     * @param[in]   json    Json文字列
     */
    bool PutAll(const std::string &json);

    /**
     * キーセット取得.
     * @return      キーセット一覧
     */
    std::vector<std::string> GetKeySet() const;

    /**
     * 整数値取得.
     * Keyに対応するValueをint型で取得する。<br>
     * 浮動小数点や桁あふれが発生した場合は不定値となる。<br>
     * Keyに対応するValueが存在しない、Valueが数値型でない場合は、default_valueを返却する
     * @param[in]   key             Key
     * @param[in]   default_value   取得できなかったときに返す値
     * @return      Value
     */
    int GetInt(const std::string &key, int default_value = 0) const;

    /**
     * 64bit整数値取得.
     * Keyに対応するValueをint64_t型で取得する。<br>
     * 浮動小数点は丸められる。<br>
     * Keyに対応するValueが存在しない、Valueが数値型でない場合は、default_valueを返却する
     * @param[in]   key             Key
     * @param[in]   default_value   取得できなかったときに返す値
     * @return      Value
     */
    int64_t GetInt64(const std::string &key, int64_t default_value = 0) const;

    /**
     * 浮動小数点値取得.
     * Keyに対応するValueをdouble型で取得する。<br>
     * Keyに対応するValueが存在しない、Valueが数値型でない場合は、default_valueを返却する
     * @param[in]   key             Key
     * @param[in]   default_value   取得できなかったときに返す値
     * @return      Value
     */
    double GetDouble(const std::string &key, double default_value = 0.0) const;

    /**
     * 真偽値取得.
     * Keyに対応するValueをbool型で取得する。<br>
     * Keyに対応するValueが存在しない、Valueが真偽値型でない場合は、default_valueを返却する
     * @param[in]   key             Key
     * @param[in]   default_value   取得できなかったときに返す値
     * @return      Value
     */
    bool GetBoolean(const std::string &key, bool default_value = false) const;

    /**
     * 文字列取得.
     * Keyに対応するValueをstd::string型で取得する。<br>
     * Keyに対応するValueが存在しない、Valueが文字列型でない場合は、default_valueを返却する
     * @param[in]   key             Key
     * @param[in]   default_value   取得できなかったときに返す値
     * @return      Value
     */
    std::string GetString(const std::string &key, const std::string default_value = "") const;

    /**
     * Jsonオブジェクト取得.
     * Keyに対応するValueをNbJsonObject型で取得する。<br>
     * Keyに対応するValueが存在しない、ValueがJsonオブジェクト型でない場合は、空のJsonオブジェクトが返却される。
     * @param[in]   key             Key
     * @return      Value
     */
    NbJsonObject GetJsonObject(const std::string &key) const;

    /**
     * Json配列取得.
     * Keyに対応するValueをNbJsonArray型で取得する。<br>
     * Keyに対応するValueが存在しない、ValueがJson配列型でない場合は、空のJson配列が返却される。
     * @param[in]   key             Key
     * @return      Value
     */
    NbJsonArray GetJsonArray(const std::string &key) const;

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
     * 添え字演算子を使用してKeyに対応するValueを設定する。<br>
     * Keyが存在する場合は、Key-Valueセットが更新される。<br>
     * Keyが存在しない場合は、Key-Valueセットが作成される。<br>
     * 設定可能なValueの型は以下
     * - int, unsigned int,
     * - int64_t, uint64_t,
     * - double
     * - bool
     * - std::string
     *
     * NbJsonObject, NbJsonArray の設定は、 PutJsonObject(), PutJsonArray() を使用すること。
     * @code
       json["key1"] = 123;
       json["key2"] = "value";
     * @endcode
     * @param[in]   key             Key
     * @return      代入用内部データへの参照
     */
    Json::Value &operator[](const std::string &key);

    /**
     * Value参照用添え字演算子.
     * 本演算子は未サポート。Valueの取得には、Getメソッドを使用してください。
     */
    const Json::Value &operator[](const std::string &key) const;

    /**
     * Jsonオブジェクト設定.
     * Keyに対応するValueをNbJsonObject型で設定する。<br>
     * Keyが空文字の場合は処理しない。
     * @param[in]   key             Key
     * @param[in]   json_object     Value
     */
    void PutJsonObject(const std::string &key, const NbJsonObject &json_object);

    /**
     * Json配列設定.
     * Keyに対応するValueをNbJsonArray型で設定する。<br>
     * Keyが空文字の場合は処理しない。
     * @param[in]   key             Key
     * @param[in]   json_array      Value
     */
    void PutJsonArray(const std::string &key, const NbJsonArray &json_array);

    /**
     * null設定.
     * Keyに対応するValueにnullを設定する。<br>
     * Keyが空文字の場合は処理しない。
     * @param[in]   key             Key
     */
    void PutNull(const std::string &key);

    /**
     * オブジェクト空確認.
     * Jsonオブジェクトが空かどうかを確認する。
     * @return  確認結果
     * @retval  true    空
     * @retval  false   1以上のKey-Valueセットが存在
     */
    bool IsEmpty() const;

    /**
     * Key存在確認.
     * keyがJsonオブジェクトに存在するか確認する。
     * @return  確認結果
     * @retval  true    存在する
     * @retval  false   存在しない
     */
    bool IsMember(const std::string &key) const;

    /**
     * Jsonタイプ取得.
     * Keyに対応するJsonタイプを取得する。<br>
     * Keyが存在しない場合は、NULL型が返る。
     * @param[in]   key             Key
     * @return      Jsonタイプ
     */
    NbJsonType GetType(const std::string &key) const;

    /**
     * Key削除.
     * Keyに対応するKey-Valueセットを削除する
     * @param[in]   key             Key
     */
    void Remove(const std::string &key);

    /**
     * 全データ削除.
     * 全てのKey-Valueセットを削除する。
     */
    void Clear();

    /**
     * Json文字列変換.
     * 設定されているJsonオブジェクトをJson文字列に変換する。
     * @return  Json文字列
     */
    std::string ToJsonString() const;

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>全データ置換.</p>
     * 全てのKey-Valueセットを置換する。
     * @return      置換データ
     */
    void Replace(const Json::Value &value);

   protected:
    Json::Value value_; /*!< Jsonデータ */
};
}  // namespace necbaas
#endif  // NECBAAS_NBJSONOBJECT_H
