//
// Created by Storm Phoenix on 2020/10/22.
//

#include <kaguya/core/light/DiffuseAreaLight.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace core {

        std::shared_ptr<AreaLight> DiffuseAreaLight::buildDiffuseAreaLight(const Spectrum &intensity,
                                                                           std::shared_ptr<ShapeSampler> shapeSampler,
                                                                           bool singleSide) {
            std::shared_ptr<AreaLight> light =
                    std::make_shared<DiffuseAreaLight>(intensity, shapeSampler, singleSide);
            shapeSampler->setAreaLight(light);
            return light;
        }

        DiffuseAreaLight::DiffuseAreaLight(const Spectrum &intensity,
                                           std::shared_ptr<ShapeSampler> shapeSampler,
                                           bool singleSide) :
                AreaLight(intensity, shapeSampler, AREA), _singleSide(singleSide) {}

        Spectrum DiffuseAreaLight::luminance(const Interaction &interaction, const Vector3 &wo) {
            double cosine = DOT(interaction.normal, wo);
            return (!_singleSide || cosine > 0) ? _intensity : Spectrum(0.0);

            /*
            if (!singleSide) {
                return _intensity;
            }

            if (cosine > 0) {
                // 同一侧
                return _intensity;
            } else {
                // 非同一侧
                return Spectrum(0.0);
            }
             */
        }

    }
}