//
// Created by Storm Phoenix on 2020/12/20.
//

#ifndef KAGUYA_BOX_H
#define KAGUYA_BOX_H

#include <kaguya/core/Transform.h>
#include <kaguya/core/medium/Medium.h>
#include <kaguya/material/Material.h>
#include <kaguya/scene/Aggregation.h>
#include <kaguya/scene/Geometry.h>

#include <map>

namespace kaguya {
    namespace scene {

        using core::transform::Transform;
        using kaguya::core::medium::Medium;
        using kaguya::material::Material;

        class Box : public Aggregation {
        public:
            Box(const std::shared_ptr<Material> material = nullptr,
                const std::shared_ptr<Medium> inside = nullptr,
                const std::shared_ptr<Medium> outside = nullptr,
                const std::shared_ptr<Transform> transform = nullptr);

            virtual const AABB &boundingBox() const override;

            virtual const std::vector<std::shared_ptr<Intersectable>> aggregation() const override;

        private:
            void build();

        private:
            // transform matrix
            std::shared_ptr<Transform> _transform;
            const std::shared_ptr<Material> _material;
            const std::shared_ptr<Medium> _inside;
            const std::shared_ptr<Medium> _outside;
            std::vector<std::shared_ptr<Intersectable>> _objects;

            AABB _aabb;
        };

    }
}

#endif //KAGUYA_BOX_H
