//
// Created by Storm Phoenix on 2020/10/23.
//

#include <kaguya/core/light/PointLight.h>

namespace kaguya {
    namespace core {

        PointLight::PointLight(const Vector3d &center, const Spectrum &intensity, const MediumBound &mediumBoundary) :
                Light(DELTA_POSITION, mediumBoundary), _center(center), _intensity(intensity) {
            assert(mediumBoundary.inside() == mediumBoundary.outside());
        }

        Spectrum PointLight::sampleFromLight(const Interaction &eye,
                                             Vector3d *wi, double *pdf,
                                             Sampler *sampler1D,
                                             VisibilityTester *visibilityTester) {
            (*wi) = NORMALIZE(_center - eye.getPoint());
            (*pdf) = 1;

            Vector3d samplePoint = _center;
            Vector3d sampleDir = NORMALIZE(_center - eye.getPoint());
            Vector3d sampleNormal = -sampleDir;
            Interaction interaction = Interaction(samplePoint, sampleDir, sampleNormal, _mediumBoundary);

            (*visibilityTester) = VisibilityTester(eye, interaction);
            return _intensity / std::pow(LENGTH(_center - eye.getPoint()), 2);
        }

        double PointLight::sampleFromLightPdf(const Interaction &eye, const Vector3d &dir) {
            return 0.0;
        }

        Spectrum PointLight::randomLightRay(Ray *ray, Vector3d *normal, double *pdfPos, double *pdfDir,
                                            Sampler *sampler1D) {
            // 采样射线
            Vector3d rayDir = math::sphereUniformSampling(sampler1D);
            (*ray) = Ray(_center, NORMALIZE(rayDir), _mediumBoundary.inside());

            *normal = rayDir;
            *pdfPos = 1.0;
            *pdfDir = math::INV_4PI;
            return _intensity;
        }

        void PointLight::randomLightRayPdf(const Ray &ray, const Vector3d &normal,
                                           double *pdfPos, double *pdfDir) const {
            (*pdfPos) = 0.0;
            (*pdfDir) = math::INV_4PI;
        }

        std::shared_ptr<PointLight> PointLight::buildPointLight(const Vector3d &center, const Spectrum &intensity,
                                                                const MediumBound mediumBoundary) {
            std::shared_ptr<PointLight> light =
                    std::make_shared<PointLight>(center, intensity, mediumBoundary);
            return light;
        }

    }
}