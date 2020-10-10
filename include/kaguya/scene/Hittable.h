//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_HITTABLE_H
#define KAGUYA_HITTABLE_H

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

        typedef struct HitRecord {
            // 击中光线方向
            Vector3 direction;
            // 击中点
            Vector3 point;
            // 击中点法线方向
            Vector3 normal;
            // 击中射线步长
            double step;
            // 击中材质种类
            std::shared_ptr<Material> material = nullptr;
            // 击中位置是否是外侧
            bool isFrontFace;
            // 击中点纹理坐标
            double u;
            double v;
            // 击中物体的 ID
            long long id = -1;

            /**
             * 设置击中位置处的法线
             * @param outwardNormal
             * @param hitDirection
             */
            void setOutwardNormal(const Vector3 &outwardNormal, const Vector3 &hitDirection) {
                direction = hitDirection;
                isFrontFace = DOT(outwardNormal, direction) < 0;
                normal = isFrontFace ? NORMALIZE(outwardNormal) : -NORMALIZE(outwardNormal);
            }
        } HitRecord;

        class Hittable {
        public:
            /**
             * 击中判定
             * @param ray
             * @param hitRecord
             * @param stepMin 射线步长最小值
             * @param stepMax 射线步长最大值
             * @return
             */
            virtual bool hit(const Ray &ray, HitRecord &hitRecord, double stepMin, double stepMax) = 0;

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

#endif //KAGUYA_HITTABLE_H
