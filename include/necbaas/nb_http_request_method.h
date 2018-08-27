/*
 * Copyright (C) 2017 NEC Corporation
 */

#ifndef NECBAAS_NBHTTPREQUESTMETHOD_H
#define NECBAAS_NBHTTPREQUESTMETHOD_H

namespace necbaas {

/**
 * HTTPリクエストメソッド.
 */
enum class NbHttpRequestMethod {
    HTTP_REQUEST_TYPE_GET,    /*!< GET    */
    HTTP_REQUEST_TYPE_POST,   /*!< POST   */
    HTTP_REQUEST_TYPE_PUT,    /*!< PUT    */
    HTTP_REQUEST_TYPE_DELETE, /*!< DELETE */
};
}  // namespace necbaas

#endif  // NECBAAS_NBHTTPREQUESTMETHOD_H
