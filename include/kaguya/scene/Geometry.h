//
// Created by Storm Phoenix on 2020/12/16.
//

#ifndef KAGUYA_GEOMETRY_H
#define KAGUYA_GEOMETRY_H

#include <kaguya/core/medium/Medium.h>
#include <kaguya/material/Material.h>
#include <kaguya/scene/Shape.h>

namespace kaguya {
    namespace scene {

        using kaguya::material::Material;
        using kaguya::core::medium::Medium;

        class Geometry final : public Shape {
        public:
            Geometry(const std::shared_ptr<Shape> shape,
                     const std::shared_ptr<Material> material,
                     const std::shared_ptr<Medium> inside = nullptr,
                     const std::shared_ptr<Medium> outside = nullptr);

            bool insect(Ray &ray, SurfaceInteraction &si, double stepMin, double stepMax) override;

            const AABB &boundingBox() const override;

        private:
            const std::shared_ptr<Shape> _shape;
            const std::shared_ptr<Material> _material;
            const std::shared_ptr<Medium> _inside;
            const std::shared_ptr<Medium> _outside;
        };

    }
}

#endif //KAGUYA_GEOMETRY_H
