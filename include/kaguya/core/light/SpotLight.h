//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_SPOTLIGHT_H
#define KAGUYA_SPOTLIGHT_H

#include <kaguya/core/light/Light.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/core/Transform.h>

namespace kaguya {
    namespace core {

        using namespace transform;

        class SpotLight : public Light {
        public:
            static std::shared_ptr<SpotLight>
            buildSpotLight(const Vector3F &center, const Vector3F &dir, const Spectrum &intensity,
                           const MediumInterface &mediumBoundary) {
                std::shared_ptr<SpotLight> light = std::make_shared<SpotLight>(center, dir, intensity, mediumBoundary,
                                                                               15, 20);
                return light;
            }

            virtual Spectrum sampleLi(const Interaction &eye,
                                      Vector3F *wi, Float *pdf,
                                      Sampler *sampler,
                                      VisibilityTester *visibilityTester) override;

            virtual Float pdfLi(const Interaction &eye, const Vector3F &dir) override;

            SpotLight(const Spectrum &intensity, Transform::Ptr lightToWorld, const MediumInterface &mediumBoundary,
                      Float fallOffRange = 30, Float totalRange = 45);

            /**
             * 聚光灯
             * @param eye 聚光灯位置
             * @param dir 聚光灯朝向
             * @param intensity 聚光灯光强
             * @param fallOffRange 聚光灯高光区半角
             * @param totalRange 聚光灯所有区域半角
             */
            SpotLight(const Vector3F eye, const Vector3F dir, Spectrum intensity, const MediumInterface &mediumBoundary,
                      Float fallOffRange = 30, Float totalRange = 45);

            virtual Spectrum sampleLe(Ray *ray, Normal3F *normal, Float *pdfPos, Float *pdfDir,
                                      Sampler *sampler) override;

            virtual void pdfLe(const Ray &ray, const Vector3F &normal,
                               Float *pdfPos, Float *pdfDir) const override;

        private:
            Spectrum fallOffWeight(const Vector3F &wo);

        private:
            Vector3F _center;
            Vector3F _dir;
            Float _cosFallOffRange;
            Float _cosTotalRange;
            Transform::Ptr _lightToWorld;
            const Spectrum _intensity;
        };

    }
}

#endif //KAGUYA_SPOTLIGHT_H
