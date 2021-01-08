//
// Created by Storm Phoenix on 2020/10/29.
//

#include <kaguya/core/Interaction.h>
#include <kaguya/material/Material.h>
#include <kaguya/tracer/Camera.h>

namespace kaguya {
    namespace core {

        Vector3d offsetOrigin(const Vector3d &origin, const Vector3d &error,
                              const Vector3d &normal, const Vector3d &direction) {
            double dist = DOT(ABS(normal), error);
            Vector3d offset = dist * normal;
            if (DOT(normal, direction) < 0) {
                offset = -offset;
            }

            Vector3d o = origin + offset;
            for (int i = 0; i < 3; i++) {
                if (offset[i] > 0) {
                    o[i] = math::doubleUp(o[i]);
                } else if (offset[i] < 0) {
                    o[i] = math::doubleDown(o[i]);
                }
            }
            return o;
        }

        using kaguya::material::Material;

        Interaction::Interaction(const Vector3d &point,
                                 const Vector3d &direction, const Vector3d &normal,
                                 const MediumBound &mediumBoundary, Material *material) :
                _point(point), _direction(direction), _normal(normal),
                _mediumBoundary(mediumBoundary), _material(material) {}


        Ray Interaction::sendRay(const Vector3d &dir) const {
            // check whether the ray direction is point to outside or inside
            const medium::Medium *medium = (DOT(dir, _normal) > 0 ?
                                            _mediumBoundary.outside() : _mediumBoundary.inside());
            Vector3d origin = offsetOrigin(_point, _error, _normal, dir);
            return Ray(origin, NORMALIZE(dir), medium);
        }

        Ray Interaction::sendRayTo(const Interaction &it) const {
            // check whether the ray direction is point to outside or inside

            Vector3d origin = offsetOrigin(_point, _error, _normal, it.getPoint() - _point);
            Vector3d target = offsetOrigin(it.getPoint(), it.getError(), it.getNormal(), origin - it.getPoint());
            const Vector3d dir = (target - origin);
            const medium::Medium *medium = (DOT(dir, _normal) > 0 ?
                                            _mediumBoundary.outside() : _mediumBoundary.inside());

            Ray ray = Ray(origin, dir, medium);
            ray.setStep(1. - math::shadowEpsilon);
            return ray;
        }

        MediumInteraction::MediumInteraction(const Vector3d &point, const Vector3d &direction,
                                             const Medium *medium, const PhaseFunction *phase) :
                Interaction(point, direction, Vector3d(0), medium),
                _medium(medium), _phase(phase) {}

        SurfaceInteraction::SurfaceInteraction(const Vector3d &point, const Vector3d &direction,
                                               const Vector3d &normal,
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
                                                 const Vector3d &p, const Vector3d &dir, const Vector3d &n) :
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