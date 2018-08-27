/*
 * Copyright (C) 2017 NEC Corporation
 */

#ifndef NECBAAS_NBQUERY_H
#define NECBAAS_NBQUERY_H

#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include <json/json.h>
#include "necbaas/nb_json_object.h"
#include "necbaas/nb_json_array.h"

namespace necbaas {

/**
 * 参照する DB の指定
 */
enum class NbReadPreference {
    PRIMARY,             /*!< プライマリから読み込む（デフォルト）*/
    SECONDARY_PREFERRED, /*!< セカンダリ優先で読み込む            */
};

/**
 * @class NbQuery nb_query.h "necbaas/nb_query.h"
 * オブジェクト検索クラス.
 * クエリ検索を実行する場合に使用する。
 *
 * <b>本クラスのインスタンスはスレッドセーフではない</b>
 */
class NbQuery {
   public:
    /**
     * コンストラクタ.
     */
    NbQuery();

    /**
     * デストラクタ.
     */
    ~NbQuery();

    /**
     * 一致条件を追加する.
     * valueに使用できる型は以下<br>
     * int, int64_t, double, bool, std::string, NbJsonObject, NbJsonArray<br>
     * keyが空文字の場合は無視する。
     * @param[in]   key     キー
     * @param[in]   value   値
     * @return      this
     */
    template <typename T>
    NbQuery &EqualTo(const std::string &key, const T &value) {
        if (!key.empty()) {
            if (typeid(T) == typeid(NbJsonObject)) {
                return AddSimpleOp("$eq", key, value);
            } else {
                return AddSimpleOp("", key, value);
            }
        }
        return *this;
    }

    /**
     * 小なり条件を追加する.
     * valueに使用できる型は以下<br>
     * int, int64_t, double, bool, std::string, NbJsonObject, NbJsonArray<br>
     * keyが空文字の場合は無視する。
     * @param[in]   key     キー
     * @param[in]   value   値
     * @return      this
     */
    template <typename T>
    NbQuery &LessThan(const std::string &key, const T &value) {
        return AddSimpleOp("$lt", key, value);
    }

    /**
     * 小なりまたは等しいを追加する.
     * valueに使用できる型は以下<br>
     * int, int64_t, double, bool, std::string, NbJsonObject, NbJsonArray<br>
     * keyが空文字の場合は無視する。
     * @param[in]   key     キー
     * @param[in]   value   値
     * @return      this
     */
    template <typename T>
    NbQuery &LessThanOrEqual(const std::string &key, const T &value) {
        return AddSimpleOp("$lte", key, value);
    }

    /**
     * 大なり条件を追加する.
     * valueに使用できる型は以下<br>
     * int, int64_t, double, bool, std::string, NbJsonObject, NbJsonArray<br>
     * keyが空文字の場合は無視する。
     * @param[in]   key     キー
     * @param[in]   value   値
     * @return      this
     */
    template <typename T>
    NbQuery &GreaterThan(const std::string &key, const T &value) {
        return AddSimpleOp("$gt", key, value);
    }

    /**
     * 大なりまたは等しいを追加する.
     * valueに使用できる型は以下<br>
     * int, int64_t, double, bool, std::string, NbJsonObject, NbJsonArray<br>
     * keyが空文字の場合は無視する。
     * @param[in]   key     キー
     * @param[in]   value   値
     * @return      this
     */
    template <typename T>
    NbQuery &GreaterThanOrEqual(const std::string &key, const T &value) {
        return AddSimpleOp("$gte", key, value);
    }

    /**
     * 不一致条件を追加する.
     * valueに使用できる型は以下<br>
     * int, int64_t, double, bool, std::string, NbJsonObject, NbJsonArray<br>
     * keyが空文字の場合は無視する。
     * @param[in]   key     キー
     * @param[in]   value   値
     * @return      this
     */
    template <typename T>
    NbQuery &NotEquals(const std::string &key, const T &value) {
        return AddSimpleOp("$ne", key, value);
    }

    /**
     * array に指定された値のいずれかと一致する条件を追加する.
     * keyが空文字の場合は無視する。
     * @param[in]   key     キー
     * @param[in]   array   Json配列
     * @return      this
     */
    NbQuery &In(const std::string &key, const NbJsonArray &array);

    /**
     * array に指定された値がすべて合致する条件を追加する.
     * keyが空文字の場合は無視する。
     * @param[in]   key     キー
     * @param[in]   array   Json配列
     * @return      this
     */
    NbQuery &All(const std::string &key, const NbJsonArray &array);

    /**
     * 正規表現一致条件を追加する.
     * オプション文字列には以下の文字の組み合わせを指定できる。<br>
     * - i: 大文字小文字を区別しない
     * - m: 複数行にマッチする
     * - x: 拡張正規表現を使用する
     * - s: '.' が改行に一致する
     * key, regexpが空文字の場合は無視する。
     * @param[in]   key     キー
     * @param[in]   regexp  正規表現
     * @param[in]   options オプション
     * @return      this
     */
    NbQuery &Regex(const std::string &key, const std::string &regexp, const std::string &options = "");

    /**
     * フィールドの存在条件を追加する.
     * keyが空文字の場合は無視する。
     * @param[in]   key     キー
     * @return      this
     */
    NbQuery &Exists(const std::string &key);

    /**
     * フィールドの非存在条件を追加する.
     * keyが空文字の場合は無視する。
     * @param[in]   key     キー
     * @return      this
     */
    NbQuery &NotExists(const std::string &key);

    /**
     * 指定したキーの条件を反転(not)する.
     * keyが空文字の場合は無視する。
     * @param[in]   key     キー
     * @return      this
     */
    NbQuery &Not(const std::string &key);

    /**
     * OR条件を生成する.
     * 現在設定されている検索条件と引数の検索条件とのOR条件を生成する。<br>
     * queriesが空の場合は無視する。
     * @param[in]   queries     クエリ
     * @return      this
     */
    NbQuery &Or(const std::vector<NbQuery> &queries);

    /**
     * AND条件を生成する.
     * 現在設定されている検索条件と引数の検索条件とのAND条件を生成する。<br>
     * queriesが空の場合は無視する。
     * @param[in]   queries     クエリ
     * @return      this
     */
    NbQuery &And(const std::vector<NbQuery> &queries);

    /**
     * ソート条件を設定する.
     * デフォルトは昇順。逆順にする場合はキー名の先頭に "-" を付加。<br>
     * @code
       // 例） key1昇順、key2降順でソートする場合
       std::vector<std::string> order_by{"key1", "-key2"};
       query.OrderBy(order_by);
     * @endcode
     * orderが空、orderリストの中に空文字が含まれている場合は全てのソート条件を設定しない。
     * @param[in]   order     ソートキー
     * @return      this
     */
    NbQuery &OrderBy(const std::vector<std::string> &order);

    /**
     * Skip値を設定する.
     * 負数を指定した場合は、リクエストパラメータに設定されない。
     * @param[in]   skip     検索スキップ数
     * @return      this
     */
    NbQuery &Skip(int skip);

    /**
     * Limit値を設定する.
     * 設定可能な値は -1 から 100 の整数値。範囲外の値が設定された場合は、リクエストパラメータに設定されない。
     * @param[in]   limit     検索上限数
     * @return      this
     */
    NbQuery &Limit(int limit);

    /**
     * 削除マークされたデータを読み込むフラグを設定する.
     * @param[in]   delete_mark    削除マークされたデータを読み込むフラグ
     * @return      this
     */
    NbQuery &DeleteMark(bool delete_mark);

    /**
     * プロジェクションを設定する.
     * 取得したいフィールド名を列挙する。<br>
     * フィールド名と列挙・抑制フラグのペアのmapで設定する。<br>
     * 列挙・抑制を混在させることはできない。例外として、_id のみを抑制することは可能。
     * @code
        // 例1) name のみを含める場合
        map["name"] = true;
        query.Projection(map);

        // 例2) address のみを除外する場合
        map["address"] = false;
        query.Projection(map);

        // 例3) name を含め、_id を除外する場合
        map["name"] = true;
        map["_id"] = false;
        query.Projection(map);
     * @endcode
     * projectionが空、projectionマップの中に空文字が含まれている場合は全てのプロジェクションが設定されない。
     * @param[in]   limit     検索上限数
     * @return      this
     */
    NbQuery &Projection(const std::map<std::string, bool> &projection);

    /**
     * 参照するDBの指定を設定する.
     * @param[in]   read_preference    参照するDBの指定
     * @return      this
     */
    NbQuery &ReadPreference(NbReadPreference read_preference);

    /**
     * クエリタイムアウト（ミリ秒）設定する.
     * 0以下が設定された場合には、リクエストパラメータに設定されない。
     * @param[in]   timeout    クエリタイムアウト（ミリ秒）
     * @return      this
     */
    NbQuery &Timeout(int timeout);

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>検索条件を文字列に変換する.</p>
     * @return      変換後の文字列
     */
    std::string GetConditionsString() const;

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>ソート順序を文字列に変換する.</p>
     * @return      変換後の文字列
     */
    std::string GetOrderString() const;

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>スキップカウントを文字列に変換する.</p>
     * @return      変換後の文字列
     */
    std::string GetSkipString() const;

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>検索上限数を文字列に変換する.</p>
     * @return      変換後の文字列
     */
    std::string GetLimitString() const;

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>削除マークされたデータを読み込む条件を文字列に変換する.</p>
     * @return      変換後の文字列
     */
    std::string GetDeleteMarkString() const;

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>プロジェクションを文字列に変換する.</p>
     * @return      変換後の文字列
     */
    std::string GetProjectionString() const;

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>参照するDBの指定を文字列に変換する.</p>
     * @return      変換後の文字列
     */
    std::string GetReadPreferenceString() const;

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>クエリタイムアウト（ミリ秒）を文字列に変換する.</p>
     * @return      変換後の文字列
     */
    std::string GetTimeoutString() const;

    /**
     * <b>[内部処理用]</b>
     * @internal
     * <p>検索条取得.</p>
     * @return      Jsonデータ
     */
    const Json::Value &GetConditions() const;

   private:
    const static int kLimitDefault; /*!< limitのデフォルト値                */

    Json::Value conditions_;                   /*!< 検索条件                           */
    std::vector<std::string> order_{};         /*!< ソート順序                         */
    int skip_{0};                              /*!< スキップカウント                   */
    int limit_{kLimitDefault};                 /*!< 検索上限数                         */
    bool delete_mark_{false};                  /*!< 削除マークされたデータを読み込む   */
    std::map<std::string, bool> projection_{}; /*!< プロジェクション                   */
    NbReadPreference read_preference_{NbReadPreference::PRIMARY}; /*!< 参照するDBの指定 */
    int timeout_{0}; /*!< クエリタイムアウト（ミリ秒）       */

    /**
     * 検索条件を設定する(組込み型用).
     * @param[in]   op        演算子(空文字:一致条件)
     * @param[in]   key       キー
     * @param[in]   value     値
     * @return      this
     */
    template <typename T>
    NbQuery &AddSimpleOpPrimitive(const std::string &op, const std::string &key, const T &value) {
        if (!key.empty()) {
            if (op.empty()) {
                conditions_[key] = value;
            } else {
                Json::Value json;
                json[op] = value;
                conditions_[key] = json;
            }
        }
        return *this;
    }

    /**
     * 検索条件を設定する(NbJson型用).
     * @param[in]   op        演算子(空文字:一致条件)
     * @param[in]   key       キー
     * @param[in]   value     値
     * @return      this
     */
    template <typename T>
    NbQuery &AddSimpleOpJson(const std::string &op, const std::string &key, const T &value) {
        if (!key.empty()) {
            if (op.empty()) {
                conditions_[key] = value.GetSubstitutableValue();
            } else {
                Json::Value json;
                json[op] = value.GetSubstitutableValue();
                conditions_[key] = json;
            }
        }
        return *this;
    }

    /**
     * 検索条件を設定する(int型).
     * @param[in]   op        演算子(空文字:一致条件)
     * @param[in]   key       キー
     * @param[in]   value     値
     * @return      this
     */
    NbQuery &AddSimpleOp(const std::string &op, const std::string &key, int value);

    /**
     * 検索条件を設定する(int64_t型).
     * @param[in]   op        演算子(空文字:一致条件)
     * @param[in]   key       キー
     * @param[in]   value     値
     * @return      this
     */
    NbQuery &AddSimpleOp(const std::string &op, const std::string &key, int64_t value);

    /**
     * 検索条件を設定する(double型).
     * @param[in]   op        演算子(空文字:一致条件)
     * @param[in]   key       キー
     * @param[in]   value     値
     * @return      this
     */
    NbQuery &AddSimpleOp(const std::string &op, const std::string &key, double value);

    /**
     * 検索条件を設定する(boolean型).
     * @param[in]   op        演算子(空文字:一致条件)
     * @param[in]   key       キー
     * @param[in]   value     値
     * @return      this
     */
    NbQuery &AddSimpleOp(const std::string &op, const std::string &key, bool value);

    /**
     * 検索条件を設定する(std::string型).
     * @param[in]   op        演算子(空文字:一致条件)
     * @param[in]   key       キー
     * @param[in]   value     値
     * @return      this
     */
    NbQuery &AddSimpleOp(const std::string &op, const std::string &key, const std::string &value);

    /**
     * 検索条件を設定する(NbJsonObject型).
     * @param[in]   op        演算子(空文字:一致条件)
     * @param[in]   key       キー
     * @param[in]   value     値
     * @return      this
     */
    NbQuery &AddSimpleOp(const std::string &op, const std::string &key, const NbJsonObject &value);

    /**
     * 検索条件を設定する(NbJsonArray型).
     * @param[in]   op        演算子(空文字:一致条件)
     * @param[in]   key       キー
     * @param[in]   value     値
     * @return      this
     */
    NbQuery &AddSimpleOp(const std::string &op, const std::string &key, const NbJsonArray &value);

    /**
     * 検索条件を連結する.
     * @param[in]   op        連結演算子
     * @param[in]   queries   クエリ
     * @return      this
     */
    NbQuery &ConcatQueries(const std::string &op, const std::vector<NbQuery> &queries);
};
}  // namespace necbaas
#endif  // NECBAAS_NBQUERY_H
