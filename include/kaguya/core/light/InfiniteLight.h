//
// Created by Storm Phoenix on 2021/5/18.
//

#ifndef KAGUYA_INFINITELIGHT_H
#define KAGUYA_INFINITELIGHT_H

#include <kaguya/core/light/Light.h>

namespace RENDER_NAMESPACE {
    namespace core {
        using scene::Scene;

        class InfiniteLight : public Light {
        public:
            typedef std::shared_ptr<InfiniteLight> Ptr;

            InfiniteLight(LightType type, const MediumInterface &mediumInterface) :
                    Light(type, mediumInterface) {}

            virtual void worldBound(const std::shared_ptr<Scene> scene) = 0;
        };
    }
}


#endif //KAGUYA_INFINITELIGHT_H
