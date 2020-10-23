//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_SPOTLIGHT_H
#define KAGUYA_SPOTLIGHT_H

#include <kaguya/core/light/Light.h>

namespace kaguya {
    namespace core {

        class SpotLight : public Light {
        public:
            virtual Spectrum sampleRay(
                    const Interaction &eye,
                    Vector3 *wi, double *pdf,
                    VisibilityTester *visibilityTester) override;

            SpotLight(const Vector3 eye, const Vector3 dir, double fallOffRange, double totalRange);
        };

    }
}

#endif //KAGUYA_SPOTLIGHT_H
