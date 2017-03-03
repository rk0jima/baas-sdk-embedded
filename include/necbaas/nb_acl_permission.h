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

#ifndef NECBAAS_NBACLPERMISSION_H
#define NECBAAS_NBACLPERMISSION_H

namespace necbaas {

/**
 * ACLで使用する権限種別の列挙型.
 */
enum class NbAclPermission {
    READ,   /*!< read権限("r")      */
    WRITE,  /*!< write権限("w")     */
    CREATE, /*!< create権限("c")    */
    UPDATE, /*!< update権限("u")    */
    DELETE, /*!< delete権限("d")    */
    ADMIN,  /*!< admin権限("admin") */
};
}  // namespace necbaas

#endif  // NECBAAS_NBACLPERMISSION_H
