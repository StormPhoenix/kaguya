//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_MATERIAL_H
#define KAGUYA_MATERIAL_H

#include <kaguya/core/Interaction.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/tracer/Ray.h>
#include <kaguya/scene/Shape.h>


// TODO Material 移动到 core
namespace kaguya {
    namespace material {

        using kaguya::tracer::Ray;
        using kaguya::core::Interaction;
        using kaguya::core::Spectrum;
        using kaguya::core::BSDF;

        /**
         * 物体材质
         */
        class Material {
        public:
            /**
             * 计算材质的 bsdf
             * @param insect ray 与 shape 的相交点
             */
            virtual std::shared_ptr<BSDF> bsdf(Interaction &insect) = 0;

            /**
             * TODO delete
             * @return
             */
            virtual bool isLight() {
                return false;
            }

            /**
             * TODO delete 逐渐删除 isSpecular
             * 是否具有反光、折射属性
             * @return
             */
            virtual bool isSpecular() {
                return false;
            }

            /**
             * 材质发射光线的能量
             * @param u 材质表面的纹理坐标
             * @param v 材质表面的纹理坐标
             * @return 发射光线能量
             */
            virtual Spectrum emitted(double u, double v) {
                return Spectrum(0.0f);
            }
        };

    }
}

#endif //KAGUYA_MATERIAL_H
