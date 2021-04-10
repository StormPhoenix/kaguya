//
// Created by Storm Phoenix on 2020/10/23.
//

#include <kaguya/core/light/PointLight.h>

namespace kaguya {
    namespace core {

        PointLight::PointLight(const Vector3F &center, const Spectrum &intensity, const MediumBoundary &mediumBoundary) :
                Light(DELTA_POSITION, mediumBoundary), _center(center), _intensity(intensity) {
            assert(mediumBoundary.inside() == mediumBoundary.outside());
        }

        Spectrum PointLight::sampleLi(const Interaction &eye,
                                      Vector3F *wi, Float *pdf,
                                      Sampler *sampler,
                                      VisibilityTester *visibilityTester) {
            (*wi) = NORMALIZE(_center - eye.point);
            (*pdf) = 1;

            Vector3F samplePoint = _center;
            Vector3F sampleDir = NORMALIZE(_center - eye.point);
            Vector3F sampleNormal = -sampleDir;
            Interaction interaction = Interaction(samplePoint, sampleDir, sampleNormal, _mediumBoundary);

            (*visibilityTester) = VisibilityTester(eye, interaction);
            return _intensity / std::pow(LENGTH(_center - eye.point), 2);
        }

        Float PointLight::pdfLi(const Interaction &eye, const Vector3F &dir) {
            return 0.0;
        }

        Spectrum PointLight::sampleLe(Ray *ray, Vector3F *normal, Float *pdfPos, Float *pdfDir,
                                      Sampler *sampler) {
            // 采样射线
            Vector3F rayDir = math::sampling::sphereUniformSampling(sampler);
            (*ray) = Ray(_center, NORMALIZE(rayDir), _mediumBoundary.inside());

            *normal = rayDir;
            *pdfPos = 1.0;
            *pdfDir = math::INV_4PI;
            return _intensity;
        }

        void PointLight::pdfLe(const Ray &ray, const Vector3F &normal,
                               Float *pdfPos, Float *pdfDir) const {
            (*pdfPos) = 0.0;
            (*pdfDir) = math::INV_4PI;
        }

        std::shared_ptr<PointLight> PointLight::buildPointLight(const Vector3F &center, const Spectrum &intensity,
                                                                const MediumBoundary mediumBoundary) {
            std::shared_ptr<PointLight> light =
                    std::make_shared<PointLight>(center, intensity, mediumBoundary);
            return light;
        }

    }
}