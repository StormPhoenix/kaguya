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
    namespace tracer {
        class Camera;
    }
}

namespace kaguya {
    namespace core {

        class AreaLight;

        class Light;

        using kaguya::tracer::Camera;

        /**
        * Ray 与 Hittable 的交点记录
        */
        class Interaction {
        public:
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
            // 击中物体的 ID
            long long id = -1;

            Interaction() {}

            Interaction(const Vector3 &point, const Vector3 &direction, const Vector3 &normal, double step) :
                    point(point), direction(direction), normal(normal), step(step) {}

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
        };

        /**
         * 击中点为表面类型
         */
        class SurfaceInteraction : public Interaction {
        public:
            SurfaceInteraction() : Interaction() {}

            SurfaceInteraction(const Vector3 &point, const Vector3 &direction, const Vector3 &normal,
                               double step, double u = 0, double v = 0) :
                    Interaction(point, direction, normal, step), u(u), v(v) {}

            // 击中点纹理坐标
            double u;
            double v;
            // 如果被击中物体是 AreaLight，则这一项应该被赋值
            std::shared_ptr<AreaLight> areaLight = nullptr;
        };

        class VolumeInteraction : public Interaction {
        public:
            VolumeInteraction() : Interaction() {}
        };

        class StartEndInteraction : public Interaction {
        public:
            union {
                const Camera *camera;
                const Light *light;
            };

            StartEndInteraction() : Interaction() {}

            StartEndInteraction(const Camera *camera) : camera(camera) {}

            StartEndInteraction(const Light *light) : light(light) {}

        };
    }
}

#endif //KAGUYA_INTERACTION_H
