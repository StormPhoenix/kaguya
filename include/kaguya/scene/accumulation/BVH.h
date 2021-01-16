//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_BVH_H
#define KAGUYA_BVH_H

#include <kaguya/scene/Aggregation.h>
#include <kaguya/scene/Geometry.h>
#include <kaguya/scene/accumulation/AABB.h>

#include <memory>

namespace kaguya {
    namespace scene {
        namespace acc {

            class BVH : public Aggregation {
            public:
                BVH() {}

                BVH(std::shared_ptr<Intersectable> object);

                BVH(std::vector<std::shared_ptr<Intersectable>> &objects);

                BVH(std::vector<std::shared_ptr<Intersectable>> &objects, size_t start, size_t end);

                const AABB &boundingBox() const override;

                bool intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const override;

                virtual const std::vector<std::shared_ptr<Intersectable>> aggregation() const override;

            protected:
                /**
                 * 构建 BVH 树
                 */
                void build(std::vector<std::shared_ptr<Intersectable>> &objects, size_t start, size_t end);

            protected:
                // 左子树
                std::shared_ptr<Intersectable> _left = nullptr;
                // 右子树
                std::shared_ptr<Intersectable> _right = nullptr;
                // aggregation
                std::vector<std::shared_ptr<Intersectable>> _objects;
                // 包围盒
                AABB _aabb;
                // BVH 构建完毕标志
                bool _isValid = false;
            };

        }
    }
}

#endif //KAGUYA_BVH_H
