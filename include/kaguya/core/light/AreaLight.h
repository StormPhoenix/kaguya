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
            AreaLight(const Spectrum &intensity, std::shared_ptr<ShapeSampler> shapeSampler, LightType type, const MediumBoundary &mediumBoundary);

            /**
             * 计算 AreaLight 表面某一点 point，方向为 wo 的亮度。
             * @param point
             * @param wo
             * @return
             */
            virtual Spectrum lightRadiance(const Interaction &interaction, const Vector3 &wo) const = 0;

            virtual Spectrum sampleFromLight(const Interaction &eye, Vector3 *wi, double *pdf,
                                             random::Sampler1D *sampler1D,
                                             VisibilityTester *visibilityTester) override;

            virtual double sampleFromLightPdf(const Interaction &eye, const Vector3 &dir) override;

        protected:
            const Spectrum _intensity;
            std::shared_ptr<ShapeSampler> _shapeSampler;
        };

    }
}

#endif //KAGUYA_AREALIGHT_H
