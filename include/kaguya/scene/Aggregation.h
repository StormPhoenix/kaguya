//
// Created by Storm Phoenix on 2020/12/21.
//

#ifndef KAGUYA_AGGREGATION_H
#define KAGUYA_AGGREGATION_H

#include <kaguya/scene/Geometry.h>

#include <vector>

namespace kaguya {
    namespace scene {

        class Aggregation {
        public:
            virtual std::vector<std::shared_ptr<Shape>> aggregation() = 0;
        };

    }
}

#endif //KAGUYA_AGGREGATION_H
