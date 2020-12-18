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
                           const std::shared_ptr<Medium> outside,
                           const std::shared_ptr<AreaLight> areaLight) :
                _shape(shape), _material(material), _inside(inside), _outside(outside), _areaLight(areaLight) {
            assert(_shape != nullptr);
        }

        bool Geometry::insect(Ray &ray, SurfaceInteraction &si, double stepMin, double stepMax) const {
            // proxy
            bool ret = _shape->insect(ray, si, stepMin, stepMax);
            if (ret) {
                // medium
                si.setMediumBoundary(MediumBound(_inside.get(), _outside.get()));
                // material
                _material != nullptr ? si.setMaterial(_material.get()) : si.setMaterial(nullptr);
                // area light
                _areaLight != nullptr ? si.setAreaLight(_areaLight.get()) : si.setAreaLight(nullptr);
            }
            return ret;
        }

        double Geometry::area() const {
            return _shape->area();
        }

        SurfaceInteraction Geometry::sampleSurfacePoint(const Sampler1D *sampler1D) const {
            return _shape->sampleSurfacePoint(sampler1D);
        }

        double Geometry::surfacePointPdf(const SurfaceInteraction &si) const {
            return _shape->surfacePointPdf(si);
        }

        SurfaceInteraction Geometry::sampleSurfaceInteraction(const Interaction &eye,
                                                              const Sampler1D *sampler1D) const {
            return _shape->sampleSurfaceInteraction(eye, sampler1D);
        }

        double Geometry::surfaceInteractionPdf(const Interaction &eye, const Vector3 &dir) const {
            return _shape->surfaceInteractionPdf(eye, dir);
        }

        const AABB &Geometry::boundingBox() const {
            return _shape->boundingBox();
        }

        void Geometry::setId(long long id) {
            _shape->setId(id);
        }
    }
}