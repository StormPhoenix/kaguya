//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_AREALIGHT_H
#define KAGUYA_AREALIGHT_H

#include <kaguya/core/light/Light.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/scene/Shape.h>

namespace kaguya {
    namespace core {

        using kaguya::scene::Shape;

        class AreaLight : public Light {
        public:
            /**
             * Area light，sample radiance from shape surface
             * @param intensity
             * @param shape
             */
            AreaLight(const Spectrum &intensity, std::shared_ptr<Shape> shape, LightType type, const MediumBound &mediumBoundary);

            /**
             * 计算 AreaLight 表面某一点 point，方向为 wo 的亮度。
             * @param point
             * @param wo
             * @return
             */
            virtual Spectrum lightRadiance(const Interaction &interaction, const Vector3 &wo) const = 0;

            virtual Spectrum sampleFromLight(const Interaction &eye, Vector3 *wi, double *pdf,
                                             const Sampler1D *sampler1D,
                                             VisibilityTester *visibilityTester) override;

            virtual double sampleFromLightPdf(const Interaction &eye, const Vector3 &dir) override;

        protected:
            const Spectrum _intensity;
            std::shared_ptr<Shape> _shape;
        };

    }
}

#endif //KAGUYA_AREALIGHT_H
