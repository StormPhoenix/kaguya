//
// Created by Storm Phoenix on 2020/10/29.
//

#include <kaguya/core/Interaction.h>
#include <kaguya/material/Material.h>
#include <kaguya/tracer/Camera.h>

namespace kaguya {
    namespace core {

        Vector3 offsetOrigin(const Vector3 &origin, const Vector3 &error,
                             const Vector3 &normal, const Vector3 &direction) {
            double dist = DOT(ABS(normal), error);
            Vector3 offset = dist * normal;
            if (DOT(normal, direction) < 0) {
                offset = -offset;
            }

            Vector3 o = origin + offset;

            for (int i = 0; i < 3; i++) {
                if (offset[i] > 0) {
                    o[i] = doubleUp(o[i]);
                } else if (offset[i] < 0) {
                    o[i] = doubleDown(o[i]);
                }
            }

            return o;

            // Round offset point _po_ away from _p_
//            for (int i = 0; i < 3; ++i) {
//                if (offset[i] > 0)
//                    po[i] = NextFloatUp(po[i]);
//                else if (offset[i] < 0)
//                    po[i] = NextFloatDown(po[i]);
//            }
//            return po;
        }

        using kaguya::material::Material;

        Interaction::Interaction(const Vector3 &point,
                                 const Vector3 &direction, const Vector3 &normal,
                                 const MediumBound &mediumBoundary, Material *material) :
                _point(point), _direction(direction), _normal(normal),
                _mediumBoundary(mediumBoundary), _material(material) {}


        Ray Interaction::sendRay(const Vector3 &dir) const {
            // check whether the ray direction is point to outside or inside
            const medium::Medium *medium = (DOT(dir, _normal) > 0 ?
                                            _mediumBoundary.outside() : _mediumBoundary.inside());
            Vector3 origin = offsetOrigin(_point, _error, _normal, dir);
            return Ray(origin, NORMALIZE(dir), medium);
        }

        Ray Interaction::sendRayTo(const Interaction &it) const {
            // check whether the ray direction is point to outside or inside

            Vector3 origin = offsetOrigin(_point, _error, _normal, it.getPoint() - _point);
            Vector3 target = offsetOrigin(it.getPoint(), it.getError(), it.getNormal(), origin - it.getPoint());
            const Vector3 dir = (target - origin);
            const medium::Medium *medium = (DOT(dir, _normal) > 0 ?
                                            _mediumBoundary.outside() : _mediumBoundary.inside());

            // TODO delete
//            double step = LENGTH(dir);
//            assert(step > 0);

            Ray ray = Ray(origin, dir, medium);
            ray.setStep(1. - shadowEpsilon);
            return ray;
        }

        MediumInteraction::MediumInteraction(const Vector3 &point, const Vector3 &direction,
                                             const Medium *medium, const PhaseFunction *phase) :
                Interaction(point, direction, Vector3(0), medium),
                _medium(medium), _phase(phase) {}

        SurfaceInteraction::SurfaceInteraction(const Vector3 &point, const Vector3 &direction,
                                               const Vector3 &normal,
                                               MediumBound &mediumBoundary,
                                               double u, double v, Material *material) :
                Interaction(point, direction, normal, mediumBoundary, material),
                _u(u), _v(v) {}


        BSDF *SurfaceInteraction::buildBSDF(MemoryArena &memoryArena, TransportMode mode) {
            assert(_material != nullptr);
            _bsdf = _material->bsdf(*this, memoryArena, mode);
            return _bsdf;
        }

        StartEndInteraction::StartEndInteraction(const Camera *camera,
                                                 const Ray &ray) : camera(camera) {
            assert(camera != nullptr);
            _point = ray.getOrigin();
            _direction = ray.getDirection();
            _normal = camera->getFront();
        }

        StartEndInteraction::StartEndInteraction(const Light *light,
                                                 const Interaction &interaction) :
                light(light), Interaction(interaction) {
        }

        StartEndInteraction::StartEndInteraction(const Light *light,
                                                 const Vector3 &p, const Vector3 &dir, const Vector3 &n) :
                light(light) {
            _point = p;
            _direction = dir;
            _normal = n;
        }

        bool MediumInteraction::isValid() const {
            return _phase != nullptr;
        }

    }
}