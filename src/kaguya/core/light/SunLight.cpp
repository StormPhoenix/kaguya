//
// Created by Storm Phoenix on 2021/5/18.
//

#include <kaguya/scene/Scene.h>
#include <kaguya/core/light/SunLight.h>
#include <kaguya/scene/accumulation/AABB.h>

namespace RENDER_NAMESPACE {
    namespace core {
        using scene::acc::AABB;

        SunLight::SunLight(const Spectrum &intensity, const Vector3F &direction) :
                Light(DELTA_DIRECTION, MediumInterface()), L(intensity), _direction(direction) {}

        Spectrum SunLight::sampleLi(const Interaction &eye, Vector3F *wi, Float *pdf,
                                    Sampler *sampler, VisibilityTester *visibilityTester) {
            if (wi != nullptr) {
                (*wi) = -_direction;
            }

            if (pdf != nullptr) {
                (*pdf) = 1.0;
            }

            if (visibilityTester != nullptr) {
                Point3F infinitePoint = eye.point + (-_direction * Float(2.0 * _worldRadius));
                (*visibilityTester) = VisibilityTester(eye, Interaction(infinitePoint, -_direction, _direction,
                                                                        _mediumInterface));
            }
            return L;
        }

        Float SunLight::pdfLi(const Interaction &eye, const Vector3F &dir) {
            return 0.0f;
        }

        Spectrum SunLight::sampleLe(Ray *ray, Normal3F *normal, Float *pdfPos, Float *pdfDir, Sampler *sampler) {
            Vector3F vx, vz;
            math::tangentSpace(_direction, &vx, &vz);
            Vector2F diskSample = math::sampling::diskUniformSampling(sampler, _worldRadius);
            Point3F rayOrigin = _worldCenter + _worldRadius * (diskSample.x * vx + diskSample.y * vz) +
                                (-_direction * _worldRadius);
            Vector3F rayDir = _direction;
            (*ray) = Ray(rayOrigin, rayDir, _mediumInterface.inside());
            (*normal) = rayDir;
            (*pdfPos) = 1.0 / (math::PI * _worldRadius * _worldRadius);
            (*pdfDir) = 1.0;
            return L;
        }

        void SunLight::pdfLe(const Ray &ray, const Vector3F &normal, Float *pdfPos, Float *pdfDir) const {
            (*pdfPos) = 1.0 / (math::PI * _worldRadius * _worldRadius);
            (*pdfDir) = 0;
        }

        void SunLight::worldBound(const std::shared_ptr<Scene> scene) {
            const AABB &bound = scene->getWorld()->boundingBox();
            _worldRadius = 0.5 * LENGTH(bound.maxPos() - bound.minPos());
            _worldCenter = (bound.maxPos() + bound.minPos()) / Float(2.0);
        }
    }
}