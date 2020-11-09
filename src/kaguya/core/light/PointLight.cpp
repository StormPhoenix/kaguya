//
// Created by Storm Phoenix on 2020/10/23.
//

#include <kaguya/core/light/PointLight.h>

namespace kaguya {
    namespace core {

        PointLight::PointLight(const Vector3 &center, const Spectrum &intensity) :
                Light(DELTA_POSITION), _center(center), _intensity(intensity) {}

        Spectrum PointLight::sampleFromLight(const Interaction &eye,
                                             Vector3 *wi, double *pdf,
                                             VisibilityTester *visibilityTester) {
            (*wi) = NORMALIZE(_center - eye.point);
            (*pdf) = 1;

            Vector3 samplePoint = _center;
            Vector3 sampleDir = NORMALIZE(_center - eye.point);
            Vector3 sampleNormal = -sampleDir;
            double step = LENGTH(_center - eye.point);
            Interaction interaction = Interaction(samplePoint, sampleDir, sampleNormal, step);

            (*visibilityTester) = VisibilityTester(eye, interaction);
            return _intensity / std::pow(LENGTH(_center - eye.point), 2);
        }

        double PointLight::sampleFromLightPdf(const Interaction &eye, const Vector3 &dir) {
            return 0.0;
        }

        Spectrum PointLight::randomLightRay(Ray *ray, Vector3 *normal, double *pdfPos, double *pdfDir) {
            // 采样射线
            Vector3 rayDir = sphereUniformSampling();
            ray->setOrigin(_center);
            ray->setDirection(rayDir);

            *normal = rayDir;
            *pdfPos = 1.0;
            *pdfDir = INV_4PI;
            return _intensity;
        }

        void PointLight::randomLightRayPdf(const Ray &ray, const Vector3 &normal,
                                           double *pdfPos, double *pdfDir) const {
            (*pdfPos) = 0.0;
            (*pdfDir) = INV_4PI;
        }

        std::shared_ptr<PointLight> PointLight::buildPointLight(const Vector3 &center, const Spectrum &intensity) {
            std::shared_ptr<PointLight> light =
                    std::make_shared<PointLight>(center, intensity);
            return light;
        }

    }
}