//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_BVH_H
#define KAGUYA_BVH_H

#include <kaguya/scene/Hittable.h>
#include <kaguya/scene/accumulation/AABB.h>

#include <memory>

namespace kaguya {
    namespace scene {
        namespace acc {

            class BVH : public Hittable {
            public:
                BVH() {}

                BVH(std::shared_ptr<Hittable> object);

                BVH(std::vector<std::shared_ptr<Hittable>> &objects);

                BVH(std::vector<std::shared_ptr<Hittable>> &objects, size_t start, size_t end);

                const AABB &boundingBox() const override;

                bool hit(const Ray &ray, HitRecord &hitRecord, double stepMin, double stepMax) override;

            protected:
                /**
                 * 构建 BVH 树
                 */
                void build(std::vector<std::shared_ptr<Hittable>> &objects, size_t start, size_t end);

            private:
                /**
                 * 合并包围盒
                 * @param leftBox
                 * @param rightBox
                 */
                void mergeBoundingBox(const AABB &leftBox, const AABB &rightBox);

            protected:
                // 左子树
                std::shared_ptr<Hittable> _left = nullptr;
                // 右子树
                std::shared_ptr<Hittable> _right = nullptr;
                // 包围盒
                AABB _aabb;
                // BVH 构建完毕标志
                bool _isValid = false;
            };

        }
    }
}

#endif //KAGUYA_BVH_H
