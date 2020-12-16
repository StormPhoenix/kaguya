//
// Created by Storm Phoenix on 2020/12/16.
//

#include <kaguya/scene/Geometry.h>

namespace kaguya {
    namespace scene {
        Geometry::Geometry(const std::shared_ptr<Shape> shape,
                           const std::shared_ptr<Material> material,
                           const MediumBound &mediumBoundary) :
                _shape(shape), _material(material), _mediumBoundary(mediumBoundary) {
            assert(_shape != nullptr);
        }

        bool Geometry::insect(Ray &ray, SurfaceInteraction &si, double stepMin, double stepMax) {
            // proxy
            bool ret = _shape->insect(ray, si, stepMin, stepMax);
            if (ret) {
                // medium
                si.setMediumBoundary(_mediumBoundary);
                // material
                _material != nullptr ? si.setMaterial(_material.get()) : si.setMaterial(nullptr);
                // TODO area light
            }
            return ret;
        }

        const AABB &Geometry::boundingBox() const {
            return _shape->boundingBox();
        }
    }
}