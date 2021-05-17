//
// Created by Storm Phoenix on 2020/10/23.
//

#include <kaguya/core/light/PointLight.h>

namespace kaguya {
    namespace core {

        PointLight::PointLight(const Spectrum &intensity, Transform::Ptr lightToWorld,
                               const MediumInterface &mediumBoundary) :
                Light(DELTA_POSITION, mediumBoundary), _intensity(intensity) {
            ASSERT(mediumBoundary.inside() == mediumBoundary.outside(), "Outside medium must equal to inside medium");
            _lightToWorld = lightToWorld != nullptr ? lightToWorld : std::make_shared<Transform>();
            _center = lightToWorld->transformPoint(Point3F(0));
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
            Interaction interaction = Interaction(samplePoint, sampleDir, sampleNormal, _mediumInterface);

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
            (*ray) = Ray(_center, NORMALIZE(rayDir), _mediumInterface.inside());

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
    }
}