//
// Created by Storm Phoenix on 2020/12/16.
//

#ifndef KAGUYA_GEOMETRY_H
#define KAGUYA_GEOMETRY_H

#include <kaguya/core/medium/Medium.h>
#include <kaguya/material/Material.h>
#include <kaguya/scene/meta/Shape.h>

namespace kaguya {

    namespace core {
        class AreaLight;
    }

    namespace scene {

        using kaguya::core::AreaLight;
        using kaguya::material::Material;
        using kaguya::core::medium::Medium;

        class Geometry final : public Intersectable {
        public:
            Geometry(const std::shared_ptr<meta::Shape> shape,
                     const std::shared_ptr<Material> material,
                     const std::shared_ptr<Medium> inside = nullptr,
                     const std::shared_ptr<Medium> outside = nullptr,
                     const std::shared_ptr<AreaLight> areaLight = nullptr);

            virtual bool intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const override;

            virtual const AABB &boundingBox() const override;

            void setAreaLight(const std::shared_ptr<AreaLight> areaLight);

            const std::shared_ptr<meta::Shape> getShape() const;

        private:
            const std::shared_ptr<meta::Shape> _shape;
            const std::shared_ptr<Material> _material;
            const std::shared_ptr<Medium> _inside;
            const std::shared_ptr<Medium> _outside;
            std::shared_ptr<AreaLight> _areaLight = nullptr;
            long long _id = -1;
        };

    }
}

#endif //KAGUYA_GEOMETRY_H
