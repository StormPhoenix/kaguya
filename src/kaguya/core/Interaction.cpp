//
// Created by Storm Phoenix on 2020/10/29.
//

#include <kaguya/core/Interaction.h>
#include <kaguya/tracer/Camera.h>

namespace RENDER_NAMESPACE {
    namespace core {

        Vector3F offsetOrigin(const Vector3F &origin, const Vector3F &error,
                              const Vector3F &normal, const Vector3F &direction) {
            Float dist = 2.0 * DOT(ABS(normal), error);
            Vector3F offset = dist * normal;
            if (DOT(normal, direction) < 0) {
                offset = -offset;
            }

            Point3F o = origin + offset;
            for (int i = 0; i < 3; i++) {
                if (offset[i] > 0) {
                    o[i] = math::floatUp(o[i]);
                } else if (offset[i] < 0) {
                    o[i] = math::floatDown(o[i]);
                }
            }
            return o;
        }

        using material::Material;

        Interaction::Interaction(const Vector3F &point,
                                 const Vector3F &direction, const Vector3F &normal,
                                 const MediumInterface &mediumBoundary, Material material) :
                point(point), direction(direction), normal(normal),
                _mediumBoundary(mediumBoundary), _material(material) {}


        Ray Interaction::sendRay(const Vector3F &dir) const {
            Vector3F origin = offsetOrigin(point, error, normal, dir);
            return Ray(origin, NORMALIZE(dir), getMedium(dir));
        }

        Ray Interaction::sendRayTo(const Point3F &target) const {
            Vector3F origin = offsetOrigin(point, error, normal, target - point);
            const Vector3F dir = (target - origin);

            Ray ray = Ray(origin, dir, getMedium(dir));
            ray.setStep(1. - math::shadowEpsilon);
            return ray;
        }

        Ray Interaction::sendRayTo(const Interaction &it) const {
            // check whether the ray direction is point to outside or inside
            Vector3F origin = offsetOrigin(point, error, normal, it.point - point);
            Vector3F target = offsetOrigin(it.point, it.error, it.normal, origin - it.point);
            const Vector3F dir = (target - origin);

            Ray ray = Ray(origin, dir, getMedium(dir));
            ray.setStep(1. - math::shadowEpsilon);
            return ray;
        }

        const Medium *Interaction::getMedium(const Vector3F &dir) const {
            // check whether the ray direction is point to outside or inside
            return (DOT(dir, normal) > 0 ?
                    _mediumBoundary.outside() : _mediumBoundary.inside());
        }

        MediumInteraction::MediumInteraction(const Vector3F &point, const Vector3F &direction,
                                             const Medium *medium, const PhaseFunction *phase) :
                Interaction(point, direction, Vector3F(0), medium),
                _medium(medium), _phase(phase) {}

        SurfaceInteraction::SurfaceInteraction(const Vector3F &point, const Vector3F &direction,
                                               const Vector3F &normal,
                                               MediumInterface &mediumBoundary,
                                               Float u, Float v, Material material) :
                Interaction(point, direction, normal, mediumBoundary, material),
                u(u), v(v) {}

        const AreaLight *SurfaceInteraction::getAreaLight() const {
            return _areaLight;
        }

        void SurfaceInteraction::setAreaLight(AreaLight *areaLight) {
            _areaLight = areaLight;
        }

        void SurfaceInteraction::buildScatteringFunction(MemoryAllocator &allocator, TransportMode mode) {
            assert(!_material.nullable());
            _material.evaluateBSDF(*this, allocator, mode);
        }

        StartEndInteraction::StartEndInteraction(const Camera *camera,
                                                 const Ray &ray) : camera(camera) {
            assert(camera != nullptr);
            point = ray.getOrigin();
            direction = ray.getDirection();
            wo = -NORMALIZE(direction);
            normal = rendering.normal = camera->getFront();
        }

        StartEndInteraction::StartEndInteraction(const Light *light,
                                                 const Interaction &interaction) :
                light(light), Interaction(interaction) {
        }

        StartEndInteraction::StartEndInteraction(const Light *light,
                                                 const Vector3F &p, const Vector3F &dir, const Vector3F &n) :
                light(light) {
            point = p;
            direction = dir;
            wo = -NORMALIZE(direction);
            normal = rendering.normal = n;
        }

        bool MediumInteraction::isValid() const {
            return _phase != nullptr;
        }

    }
}