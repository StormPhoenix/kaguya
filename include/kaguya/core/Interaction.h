//
// Created by Storm Phoenix on 2020/10/16.
//

#ifndef KAGUYA_INTERACTION_H
#define KAGUYA_INTERACTION_H

#include <kaguya/core/Core.h>

namespace kaguya {
    namespace material {
        class Material;
    }
}

namespace kaguya {
    namespace core {


        /**
        * Ray 与 Hittable 的交点记录
        */
        typedef struct Interaction {
            // 击中光线方向
            Vector3 direction;
            // 击中点
            Vector3 point;
            // 击中点法线方向
            Vector3 normal;
            // 击中射线步长
            double step;
            // 击中材质种类
            std::shared_ptr<kaguya::material::Material> material = nullptr;
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
        } Interaction;
    }
}

#endif //KAGUYA_INTERACTION_H
