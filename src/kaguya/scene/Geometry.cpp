//
// Created by Storm Phoenix on 2020/12/16.
//

#include <kaguya/core/medium/MediumBound.h>
#include <kaguya/scene/Geometry.h>

namespace kaguya {
    namespace scene {

        using kaguya::core::medium::MediumBound;

        Geometry::Geometry(const std::shared_ptr<Shape> shape,
                           const std::shared_ptr<Material> material,
                           const std::shared_ptr<Medium> inside,
                           const std::shared_ptr<Medium> outside) :
                _shape(shape), _material(material), _inside(inside), _outside(outside) {
            assert(_shape != nullptr);
        }

        bool Geometry::insect(Ray &ray, SurfaceInteraction &si, double stepMin, double stepMax) {
            // proxy
            bool ret = _shape->insect(ray, si, stepMin, stepMax);
            if (ret) {
                // medium
                si.setMediumBoundary(MediumBound(_inside.get(), _outside.get()));
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