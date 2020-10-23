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

        class AreaLight;

        /**
        * Ray 与 Hittable 的交点记录
        */
        typedef struct Interaction {
            // 击中光线方向
            Vector3 direction;
            // 击中点
            Vector3 point;
            // 击中点法线方向，发现永远指向物体表面外侧
            Vector3 normal;
            // 击中射线步长
            double step;
            // 击中材质种类
            std::shared_ptr<kaguya::material::Material> material = nullptr;
            // 击中点纹理坐标
            double u;
            double v;
            // 击中物体的 ID
            long long id = -1;
            // TODO 应该做一个 SurfaceInteraction 类型了
            // 如果被击中物体是 AreaLight，则这一项应该被赋值
            std::shared_ptr<AreaLight> areaLight = nullptr;

            /**
             * TODO delete
             * 设置击中位置处的法线
             * @param outwardNormal
             * @param hitDirection
             */
            void setOutwardNormal(const Vector3 &outwardNormal, const Vector3 &hitDirection) {
                direction = hitDirection;
                normal = outwardNormal;
            }
        } Interaction;
    }
}

#endif //KAGUYA_INTERACTION_H
