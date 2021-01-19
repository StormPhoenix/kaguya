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
            static std::shared_ptr<PointLight> buildPointLight(const Vector3F &center, const Spectrum &intensity, const MediumBound mediumBoundary);

            /**
             * 点光源
             * @param center 光源位置
             * @param intensity 光源处，单位立体角的光通量
             */
            PointLight(const Vector3F &center, const Spectrum &intensity, const MediumBound &mediumBoundary);

            virtual Spectrum sampleFromLight(const Interaction &eye,
                                             Vector3F *wi, Float *pdf,
                                             Sampler *sampler,
                                             VisibilityTester *visibilityTester) override;

            virtual Float sampleFromLightPdf(const Interaction &eye, const Vector3F &dir) override;

            virtual Spectrum randomLightRay(Ray *ray, Vector3F *normal, Float *pdfPos, Float *pdfDir,
                                            Sampler *sampler) override;

            virtual void randomLightRayPdf(const Ray &ray, const Vector3F &normal,
                                           Float *pdfPos, Float *pdfDir) const override;

        private:
            // 单位立体角的光强
            const Spectrum _intensity;
            const Vector3F _center;
        };

    }
}

#endif //KAGUYA_POINTLIGHT_H
