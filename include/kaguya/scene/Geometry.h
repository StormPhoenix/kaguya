//
// Created by Storm Phoenix on 2020/12/16.
//

#ifndef KAGUYA_GEOMETRY_H
#define KAGUYA_GEOMETRY_H

#include <kaguya/core/light/AreaLight.h>
#include <kaguya/core/medium/Medium.h>
#include <kaguya/material/Material.h>
#include <kaguya/scene/Shape.h>

namespace kaguya {
    namespace scene {

        using kaguya::core::AreaLight;
        using kaguya::material::Material;
        using kaguya::core::medium::Medium;

        class Geometry final : public Shape {
        public:
            Geometry(const std::shared_ptr<Shape> shape,
                     const std::shared_ptr<Material> material,
                     const std::shared_ptr<Medium> inside = nullptr,
                     const std::shared_ptr<Medium> outside = nullptr,
                     const std::shared_ptr<AreaLight> areaLight = nullptr);

            virtual bool insect(Ray &ray, SurfaceInteraction &si, double stepMin, double stepMax) const override;

            virtual double area() const override;

            virtual SurfaceInteraction sampleSurfacePoint(const Sampler1D *sampler1D) const override;

            virtual double surfacePointPdf(const SurfaceInteraction &si) const override;

            virtual SurfaceInteraction sampleSurfaceInteraction(
                    const Interaction &eye,
                    const Sampler1D *sampler1D) const override;

            virtual double surfaceInteractionPdf(const Interaction &eye, const Vector3 &dir) const override;

            virtual const AABB &boundingBox() const override;

            virtual void setId(long long id) override;

            void setAreaLight(const std::shared_ptr<AreaLight> areaLight) {
                _areaLight = areaLight;
            }

        private:
            const std::shared_ptr<Shape> _shape;
            const std::shared_ptr<Material> _material;
            const std::shared_ptr<Medium> _inside;
            const std::shared_ptr<Medium> _outside;
            std::shared_ptr<AreaLight> _areaLight = nullptr;
        };

    }
}

#endif //KAGUYA_GEOMETRY_H
