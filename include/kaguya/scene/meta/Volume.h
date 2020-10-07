//
// Created by Storm Phoenix on 2020/10/2.
//

#ifndef KAGUYA_VOLUME_H
#define KAGUYA_VOLUME_H

#include <kaguya/scene/Hittable.h>

namespace kaguya {
    namespace scene {

        class Volume : public Hittable {
        public:
            /**
             * 烟雾体
             */
            Volume();
        };

    }
}

#endif //KAGUYA_VOLUME_H
