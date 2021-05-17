//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_DIRECTLIGHT_H
#define KAGUYA_DIRECTLIGHT_H

#include <kaguya/Common.h>
#include <kaguya/core/Transform.h>
#include <kaguya/core/light/Light.h>

namespace RENDER_NAMESPACE {
    namespace core {
        using namespace transform;

        class DirectLight : public Light {
        public:
            DirectLight(const Spectrum &intensity, Transform::Ptr lightToWorld, const Vector3F &direction);

            virtual Spectrum sampleLi(const Interaction &eye, Vector3F *wi, Float *pdf,
                                      Sampler *sampler, VisibilityTester *visibilityTester) override;

            virtual Float pdfLi(const Interaction &eye, const Vector3F &dir) override;

        private:
            const Spectrum L;
            Transform::Ptr _lightToWorld;
            const Vector3F _direction;
            Float _worldRadius = 20000.0;
        };
    }
}

#endif //KAGUYA_DIRECTLIGHT_H
