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
            buildSpotLight(const Vector3 &center, const Vector3 &dir, const Spectrum &intensity,
                           const MediumBound &mediumBoundary) {
                std::shared_ptr<SpotLight> light = std::make_shared<SpotLight>(center, dir, intensity, mediumBoundary,
                                                                               15, 20);
                return light;
            }

            virtual Spectrum sampleFromLight(const Interaction &eye,
                                             Vector3 *wi, double *pdf,
                                             const Sampler1D *sampler1D,
                                             VisibilityTester *visibilityTester) override;

            virtual double sampleFromLightPdf(const Interaction &eye, const Vector3 &dir) override;

            /**
             * 聚光灯
             * @param eye 聚光灯位置
             * @param dir 聚光灯朝向
             * @param intensity 聚光灯光强
             * @param fallOffRange 聚光灯高光区半角
             * @param totalRange 聚光灯所有区域半角
             */
            SpotLight(const Vector3 eye, const Vector3 dir, Spectrum intensity, const MediumBound &mediumBoundary,
                      double fallOffRange = 30, double totalRange = 45);

            virtual Spectrum randomLightRay(Ray *ray, Vector3 *normal, double *pdfPos, double *pdfDir,
                                            const Sampler1D *sampler1D) override;

            virtual void randomLightRayPdf(const Ray &ray, const Vector3 &normal,
                                           double *pdfPos, double *pdfDir) const override;

        private:
            Spectrum fallOffWeight(const Vector3 &wo);

        private:
            const Vector3 _center;
            const Vector3 _dir;
            double _cosFallOffRange;
            double _cosTotalRange;
            const Spectrum _intensity;
        };

    }
}

#endif //KAGUYA_SPOTLIGHT_H
