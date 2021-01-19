//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_SPOTLIGHT_H
#define KAGUYA_SPOTLIGHT_H

#include <kaguya/core/light/Light.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace core {

        class SpotLight : public Light {
        public:
            static std::shared_ptr<SpotLight>
            buildSpotLight(const Vector3F &center, const Vector3F &dir, const Spectrum &intensity,
                           const MediumBound &mediumBoundary) {
                std::shared_ptr<SpotLight> light = std::make_shared<SpotLight>(center, dir, intensity, mediumBoundary,
                                                                               15, 20);
                return light;
            }

            virtual Spectrum sampleFromLight(const Interaction &eye,
                                             Vector3F *wi, Float *pdf,
                                             Sampler *sampler,
                                             VisibilityTester *visibilityTester) override;

            virtual Float sampleFromLightPdf(const Interaction &eye, const Vector3F &dir) override;

            /**
             * 聚光灯
             * @param eye 聚光灯位置
             * @param dir 聚光灯朝向
             * @param intensity 聚光灯光强
             * @param fallOffRange 聚光灯高光区半角
             * @param totalRange 聚光灯所有区域半角
             */
            SpotLight(const Vector3F eye, const Vector3F dir, Spectrum intensity, const MediumBound &mediumBoundary,
                      Float fallOffRange = 30, Float totalRange = 45);

            virtual Spectrum randomLightRay(Ray *ray, Vector3F *normal, Float *pdfPos, Float *pdfDir,
                                            Sampler *sampler) override;

            virtual void randomLightRayPdf(const Ray &ray, const Vector3F &normal,
                                           Float *pdfPos, Float *pdfDir) const override;

        private:
            Spectrum fallOffWeight(const Vector3F &wo);

        private:
            const Vector3F _center;
            const Vector3F _dir;
            Float _cosFallOffRange;
            Float _cosTotalRange;
            const Spectrum _intensity;
        };

    }
}

#endif //KAGUYA_SPOTLIGHT_H
