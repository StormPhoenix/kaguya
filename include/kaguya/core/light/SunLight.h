//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_SUNLIGHT_H
#define KAGUYA_SUNLIGHT_H

#include <kaguya/Common.h>
#include <kaguya/core/Transform.h>
#include <kaguya/core/light/Light.h>

namespace RENDER_NAMESPACE {
    namespace core {
        using namespace transform;

        class SunLight : public Light {
        public:
            SunLight(const Spectrum &intensity, const Vector3F &direction);

            virtual Spectrum sampleLi(const Interaction &eye, Vector3F *wi, Float *pdf,
                                      Sampler *sampler, VisibilityTester *visibilityTester) override;

            virtual Float pdfLi(const Interaction &eye, const Vector3F &dir) override;

            virtual Spectrum
            sampleLe(Ray *ray, Normal3F *normal, Float *pdfPos, Float *pdfDir, Sampler *sampler) override;

            virtual void pdfLe(const Ray &ray, const Vector3F &normal, Float *pdfPos, Float *pdfDir) const override;

            virtual void worldBound(const std::shared_ptr<Scene> scene) override;

        private:
            const Spectrum L;
            const Vector3F _direction;
            Float _worldRadius = 20000.0;
            Point3F _worldCenter;
        };
    }
}

#endif //KAGUYA_SUNLIGHT_H
