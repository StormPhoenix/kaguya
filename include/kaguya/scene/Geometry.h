//
// Created by Storm Phoenix on 2020/12/16.
//

#ifndef KAGUYA_GEOMETRY_H
#define KAGUYA_GEOMETRY_H

#include <kaguya/material/Material.h>
#include <kaguya/scene/Shape.h>
#include <kaguya/core/medium/MediumBound.h>

namespace kaguya {
    namespace scene {

        using kaguya::material::Material;
        using kaguya::core::medium::MediumBound;

        class Geometry final : public Shape {
        public:
            Geometry(const std::shared_ptr<Shape> shape,
                     const std::shared_ptr<Material> _material,
                     const MediumBound &mediumBoundary);

            bool insect(Ray &ray, SurfaceInteraction &si, double stepMin, double stepMax) override;

            const AABB &boundingBox() const override;

        private:
            const std::shared_ptr<Shape> _shape;
            const std::shared_ptr<Material> _material;
            MediumBound _mediumBoundary;
        };

    }
}

#endif //KAGUYA_GEOMETRY_H
