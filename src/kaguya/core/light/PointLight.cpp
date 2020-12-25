//
// Created by Storm Phoenix on 2020/10/23.
//

#include <kaguya/core/light/PointLight.h>

namespace kaguya {
    namespace core {

        PointLight::PointLight(const Vector3 &center, const Spectrum &intensity, const MediumBound &mediumBoundary) :
                Light(DELTA_POSITION, mediumBoundary), _center(center), _intensity(intensity) {
            assert(mediumBoundary.inside() == mediumBoundary.outside());
        }

        Spectrum PointLight::sampleFromLight(const Interaction &eye,
                                             Vector3 *wi, double *pdf,
                                             const Sampler1D *sampler1D,
                                             VisibilityTester *visibilityTester) {
            (*wi) = NORMALIZE(_center - eye.getPoint());
            (*pdf) = 1;

            Vector3 samplePoint = _center;
            Vector3 sampleDir = NORMALIZE(_center - eye.getPoint());
            Vector3 sampleNormal = -sampleDir;
            Interaction interaction = Interaction(samplePoint, sampleDir, sampleNormal, _mediumBoundary);

            (*visibilityTester) = VisibilityTester(eye, interaction);
            return _intensity / std::pow(LENGTH(_center - eye.getPoint()), 2);
        }

        double PointLight::sampleFromLightPdf(const Interaction &eye, const Vector3 &dir) {
            return 0.0;
        }

        Spectrum PointLight::randomLightRay(Ray *ray, Vector3 *normal, double *pdfPos, double *pdfDir,
                                            const Sampler1D *sampler1D) {
            // 采样射线
            Vector3 rayDir = math::sphereUniformSampling(sampler1D);
            (*ray) = Ray(_center, NORMALIZE(rayDir), _mediumBoundary.inside());

            *normal = rayDir;
            *pdfPos = 1.0;
            *pdfDir = math::INV_4PI;
            return _intensity;
        }

        void PointLight::randomLightRayPdf(const Ray &ray, const Vector3 &normal,
                                           double *pdfPos, double *pdfDir) const {
            (*pdfPos) = 0.0;
            (*pdfDir) = math::INV_4PI;
        }

        std::shared_ptr<PointLight> PointLight::buildPointLight(const Vector3 &center, const Spectrum &intensity,
                                                                const MediumBound mediumBoundary) {
            std::shared_ptr<PointLight> light =
                    std::make_shared<PointLight>(center, intensity, mediumBoundary);
            return light;
        }

    }
}