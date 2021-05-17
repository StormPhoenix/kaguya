//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_POINTLIGHT_H
#define KAGUYA_POINTLIGHT_H

#include <kaguya/core/light/Light.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/core/Transform.h>

namespace kaguya {
    namespace core {

        using namespace transform;

        class PointLight : public Light {
        public:
            /**
             * 点光源
             * @param center 光源位置
             * @param intensity 光源处，单位立体角的光通量
             */
            PointLight(const Spectrum &intensity, Transform::Ptr lightToWorld, const MediumInterface &mediumBoundary);

            virtual Spectrum sampleLi(const Interaction &eye,
                                      Vector3F *wi, Float *pdf,
                                      Sampler *sampler,
                                      VisibilityTester *visibilityTester) override;

            virtual Float pdfLi(const Interaction &eye, const Vector3F &dir) override;

            virtual Spectrum sampleLe(Ray *ray, Vector3F *normal, Float *pdfPos, Float *pdfDir,
                                      Sampler *sampler) override;

            virtual void pdfLe(const Ray &ray, const Vector3F &normal,
                               Float *pdfPos, Float *pdfDir) const override;

        private:
            // 单位立体角的光强
            Spectrum _intensity;
            Vector3F _center;
            Transform::Ptr _lightToWorld;
        };

    }
}

#endif //KAGUYA_POINTLIGHT_H
