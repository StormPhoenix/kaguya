//
// Created by Storm Phoenix on 2020/10/2.
//

#ifndef KAGUYA_BOX_H
#define KAGUYA_BOX_H

#include <kaguya/scene/Hittable.h>

namespace kaguya {
    namespace scene {

        class Box : public Hittable {
        public:
            /**
             * 六面体盒子
             */
            Box();
        };
    }
}

#endif //KAGUYA_BOX_H
