/*
 * Copyright (C) 2017 NEC Corporation
 */

#ifndef NECBAAS_NBJSONTYPE_H
#define NECBAAS_NBJSONTYPE_H

namespace necbaas {

/**
 * Json値のタイプ.
 */
enum class NbJsonType {
    NB_JSON_NULL,    /*!< NULL         */
    NB_JSON_NUMBER,  /*!< 数値         */
    NB_JSON_BOOLEAN, /*!< 真偽値       */
    NB_JSON_STRING,  /*!< 文字列       */
    NB_JSON_OBJECT,  /*!< オブジェクト */
    NB_JSON_ARRAY,   /*!< 配列         */
};
}  // namespace necbaas

#endif  // NECBAAS_NBJSONTYPE_H
