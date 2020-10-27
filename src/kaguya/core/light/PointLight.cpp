//
// Created by Storm Phoenix on 2020/10/23.
//

#include <kaguya/core/light/PointLight.h>

namespace kaguya {
    namespace core {

        PointLight::PointLight(const Vector3 &center, const Spectrum &intensity) :
                Light(DELTA_POSITION), _center(center), _intensity(intensity) {}

        Spectrum PointLight::sampleRay(const Interaction &eye,
                                       Vector3 *wi, double *pdf,
                                       VisibilityTester *visibilityTester) {
            (*wi) = NORMALIZE(_center - eye.point);
            (*pdf) = 1;
            Interaction interaction;
            interaction.point = _center;
            (*visibilityTester) = VisibilityTester(eye, interaction);
            return _intensity / std::pow(LENGTH(_center - eye.point), 2);
        }

        double PointLight::sampleRayPdf(const Interaction &eye, const Vector3 &dir) {
            return 0.0;
        }

        std::shared_ptr<PointLight> PointLight::buildPointLight(const Vector3 &center, const Spectrum &intensity) {
            std::shared_ptr<PointLight> light =
                    std::make_shared<PointLight>(center, intensity);
            return light;
        }

    }
}