#ifndef NECBAAS_FTCLOUDFN_H
#define NECBAAS_FTCLOUDFN_H

#include "necbaas/nb_service.h"

namespace necbaas {
namespace FTCloudfn {
// CloudFunction登録
void RegisterCloudfnDefault(std::shared_ptr<NbService> service);

// CloudFunction削除
void DeleteCloudfnDefault(std::shared_ptr<NbService> service);
}
}

#endif //NECBAAS_FTCLOUDFN_H
