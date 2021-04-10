//
// Created by Storm Phoenix on 2020/12/16.
//

#include <kaguya/core/medium/MediumBoundary.h>
#include <kaguya/scene/Geometry.h>

namespace kaguya {
    namespace scene {

        using kaguya::core::medium::MediumBoundary;

        Geometry::Geometry(const std::shared_ptr<meta::Shape> shape,
                           const std::shared_ptr<Material> material,
                           const std::shared_ptr<Medium> inside,
                           const std::shared_ptr<Medium> outside,
                           const std::shared_ptr<AreaLight> areaLight) :
                _shape(shape), _material(material), _inside(inside), _outside(outside), _areaLight(areaLight) {
            assert(_shape != nullptr);
        }

        bool Geometry::intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const {
            // proxy
            bool ret = _shape->intersect(ray, si, minStep, maxStep);
            if (ret) {
                // medium
                si.setMediumBoundary(MediumBoundary(_inside.get(), _outside.get()));
                // material
                _material != nullptr ? si.setMaterial(_material.get()) : si.setMaterial(nullptr);
                // area light
                _areaLight != nullptr ? si.setAreaLight(_areaLight.get()) : si.setAreaLight(nullptr);
                // geometry
                si.setGeometry(this);
            }
            return ret;
        }

        const AABB &Geometry::boundingBox() const {
            return _shape->bound();
        }

        void Geometry::setAreaLight(const std::shared_ptr<AreaLight> areaLight) {
            _areaLight = areaLight;
        }

        const std::shared_ptr<meta::Shape> Geometry::getShape() const {
            return _shape;
        }
    }
}