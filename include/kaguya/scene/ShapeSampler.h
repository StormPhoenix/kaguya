//
// Created by Storm Phoenix on 2020/10/9.
//

#ifndef KAGUYA_SHAPESAMPLER_H
#define KAGUYA_SHAPESAMPLER_H


#include <kaguya/math/Math.hpp>
#include <kaguya/tracer/Ray.h>
#include <kaguya/scene/Shape.h>

namespace kaguya {
    namespace core {

        class AreaLight;

    }
}

namespace kaguya {
    namespace scene {

        using kaguya::tracer::Ray;
        using kaguya::core::AreaLight;

        /**
         * TODO ShapeSampler 和 Shape 合并
         * 实现对场景中物体的采样功能
         */
        class ShapeSampler : public Shape {
        public:
            /**
             * Shape 的表面积
             * @return
             */
            virtual double area() = 0;

            /**
             * 在 Shape 表面进行均匀采样
             * @return
             */
            virtual SurfaceInteraction sampleSurfacePoint() = 0;

            /**
             * 计算surface上做均匀采样的概率
             * @param point 采样点
             * @return
             */
            virtual double surfacePointPdf(SurfaceInteraction &point);

            /**
             * 从 eye 处采样射线与 Shape 相交，返回交点
             * @param eye
             * @return
             */
            virtual SurfaceInteraction sampleRayIntersection(const Interaction &eye);

            /**
             * 计算从 eye 出发，沿 dir 方向击中 Shape 上的点的概率
             * @param eye
             * @param dir
             * @param pdf
             * @return
             */
            virtual double rayPdf(const Interaction &eye, const Vector3 &dir);

            void setAreaLight(std::shared_ptr<AreaLight> areaLight) {
                _areaLight = areaLight;
            }

            std::shared_ptr<AreaLight> getAreaLight() {
                return _areaLight;
            }

        protected:
            std::shared_ptr<AreaLight> _areaLight = nullptr;
        };
    }
}

#endif //KAGUYA_SHAPESAMPLER_H
