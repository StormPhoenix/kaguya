//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SHAPE_H
#define KAGUYA_SHAPE_H

#include <kaguya/core/Interaction.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/scene/accumulation/AABB.h>

namespace kaguya {
    namespace material {
        class Material;
    }
}

namespace kaguya {
    namespace tracer {
        class Ray;
    }
}

namespace kaguya {
    namespace scene {

        using kaguya::material::Material;
        using kaguya::scene::acc::AABB;
        using kaguya::tracer::Ray;
        using kaguya::core::SurfaceInteraction;
        using kaguya::core::Interaction;

        class Shape {
        public:
            /**
             * 击中判定
             * @param ray
             * @param hitRecord
             * @param stepMin 射线步长最小值
             * @param stepMax 射线步长最大值
             * @return
             */
            virtual bool insect(const Ray &ray, SurfaceInteraction &hitRecord, double stepMin, double stepMax) = 0;

            /**
             * 计算 AxisAlignBoundingBox
             * @return
             */
            virtual const AABB &boundingBox() const = 0;

            /**
             * 获取物体的 ID
             * @return
             */
            virtual const long long getId() const {
                return _id;
            }

            /**
             * 设置物体的 ID
             */
            virtual void setId(long long id) {
                _id = id;
            }

        protected:
            long long _id = -1;
        };

    }
}

#endif //KAGUYA_SHAPE_H
