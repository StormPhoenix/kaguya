//
// Created by Storm Phoenix on 2021/4/16.
//

#ifndef KAGUYA_ENVIRONMENTLIGHT_H
#define KAGUYA_ENVIRONMENTLIGHT_H

#include <kaguya/core/light/Light.h>

namespace kaguya {
    namespace core {
        class EnvironmentLight : public Light {
        public:
            EnvironmentLight();

            virtual Spectrum Le(const Ray &ray) const override;

            virtual Spectrum sampleLi(const Interaction &eye, Vector3F *wi, Float *pdf,
                                      Sampler *sampler, VisibilityTester *visibilityTester) override;

            virtual Float pdfLi(const Interaction &eye, const Vector3F &dir) override;

            virtual Spectrum sampleLe(Ray *ray, Vector3F *normal, Float *pdfPos,
                                      Float *pdfDir, Sampler *sampler) override;

            virtual void pdfLe(const Ray &ray, const Vector3F &normal,
                               Float *pdfPos, Float *pdfDir) const override;


        };
    }
}

#endif //KAGUYA_ENVIRONMENTLIGHT_H
