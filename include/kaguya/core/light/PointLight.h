//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_POINTLIGHT_H
#define KAGUYA_POINTLIGHT_H

#include <kaguya/core/light/Light.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace core {

        class PointLight : public Light {
        public:
            /**
             * 构建点光源
             * @param center
             * @param intensity
             * @return
             */
            static std::shared_ptr<PointLight> buildPointLight(const Vector3 &center, const Spectrum &intensity,  const MediumBound mediumBoundary);

            /**
             * 点光源
             * @param center 光源位置
             * @param intensity 光源处，单位立体角的光通量
             */
            PointLight(const Vector3 &center, const Spectrum &intensity, const MediumBound &mediumBoundary);

            virtual Spectrum sampleFromLight(const Interaction &eye,
                                             Vector3 *wi, double *pdf,
                                             const Sampler *sampler1D,
                                             VisibilityTester *visibilityTester) override;

            virtual double sampleFromLightPdf(const Interaction &eye, const Vector3 &dir) override;

            virtual Spectrum randomLightRay(Ray *ray, Vector3 *normal, double *pdfPos, double *pdfDir,
                                            const Sampler *sampler1D) override;

            virtual void randomLightRayPdf(const Ray &ray, const Vector3 &normal,
                                           double *pdfPos, double *pdfDir) const override;

        private:
            // 单位立体角的光强
            const Spectrum _intensity;
            const Vector3 _center;
        };

    }
}

#endif //KAGUYA_POINTLIGHT_H
