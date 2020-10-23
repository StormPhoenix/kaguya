//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_AREALIGHT_H
#define KAGUYA_AREALIGHT_H

#include <kaguya/core/light/Light.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

#include <kaguya/scene/ShapeSampler.h>

namespace kaguya {
    namespace core {

        using kaguya::scene::ShapeSampler;

        class AreaLight : public Light {
        public:
            /**
             * 区域光，从 ShapeSampler 上面采样点进行辐射
             * @param intensity
             * @param shapeSampler
             */
            AreaLight(const Spectrum &intensity, std::shared_ptr<ShapeSampler> shapeSampler, LightType type);

            /**
             * 计算 AreaLight 表面某一点 point，方向为 wo 的亮度。
             * @param point
             * @param wo
             * @return
             */
            virtual Spectrum luminance(const Interaction &interaction, const Vector3 &wo) = 0;

            virtual Spectrum sampleRay(
                    const Interaction &eye,
                    Vector3 *wi, double *pdf,
                    VisibilityTester *visibilityTester) override;

            virtual double sampleRayPdf(const Interaction &eye, const Vector3 &dir) override;

        protected:
            const Spectrum _intensity;
            std::shared_ptr<ShapeSampler> _shapeSampler;
        };

    }
}

#endif //KAGUYA_AREALIGHT_H
