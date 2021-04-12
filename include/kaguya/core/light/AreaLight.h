//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_AREALIGHT_H
#define KAGUYA_AREALIGHT_H

#include <kaguya/core/light/Light.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/scene/Geometry.h>

namespace kaguya {

    namespace scene {
        class Geometry;
    }

    namespace core {

        using scene::Geometry;

        class AreaLight : public Light {
        public:
            /**
             * Area light，sample1d radiance from shape surface
             * @param intensity
             * @param shape
             */
            AreaLight(const Spectrum &intensity, std::shared_ptr<Geometry> shape, LightType type, const MediumBoundary &mediumBoundary);

            /**
             * 计算 AreaLight 表面某一点 point，方向为 wo 的亮度。
             * @param point
             * @param wo
             * @return
             */
            virtual Spectrum L(const Interaction &interaction, const Vector3F &wo) const = 0;

            virtual Spectrum sampleLi(const Interaction &eye, Vector3F *wi, Float *pdf,
                                      Sampler *sampler,
                                      VisibilityTester *visibilityTester) override;

            virtual Float pdfLi(const Interaction &eye, const Vector3F &dir) override;

        protected:
            const Spectrum _intensity;
            std::shared_ptr<Geometry> _geometry;
        };

    }
}

#endif //KAGUYA_AREALIGHT_H
