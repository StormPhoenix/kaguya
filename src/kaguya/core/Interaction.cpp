//
// Created by Storm Phoenix on 2020/10/29.
//

#include <kaguya/core/Interaction.h>
#include <kaguya/material/Material.h>
#include <kaguya/tracer/Camera.h>

namespace kaguya {
    namespace core {

        using kaguya::material::Material;

        Interaction::Interaction(const Vector3 &point,
                                 const Vector3 &direction, const Vector3 &normal, double step,
                                 Material *material) :
                _point(point), _direction(direction), _normal(normal),
                _step(step), _material(material) {}


        SurfaceInteraction::SurfaceInteraction(const Vector3 &point, const Vector3 &direction,
                                               const Vector3 &normal, double step, double u, double v,
                                               Material *material) :
                Interaction(point, direction, normal, step, material),
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
            return phase != nullptr;
        }

    }
}