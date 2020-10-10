//
// Created by Storm Phoenix on 2020/10/1.
//

#ifndef KAGUYA_LIGHTHITPDF_H
#define KAGUYA_LIGHTHITPDF_H

#include <kaguya/math/Math.hpp>
#include <kaguya/scene/meta/Light.h>

namespace kaguya {
    namespace math {

        using kaguya::scene::Light;

        /**
         * 从某点发射线击中光源的概率
         * TODO 目前只考虑方形光源的情况
         * TODO 应该把 LightHitPdf 泛化成 ObjectHitPdf
         */
        class LightHitPdf {
        public:

            /**
             * TODO 由于扩展成 ObjectHitPdf，则不需要返回 bool 类型了
             * 采样点到光源的射线
             * @param point
             * @param sampleRay
             * @param samplePdf
             * @return 是否采样成功
             */
            virtual bool sample(const Vector3 &point,
                                Vector3 &sampleRayDir,
                                double &samplePdf,
                                Light *light);

            /**
             * 计算射线的 pdf
             * @param point
             * @param rayDir
             * @param light
             * @return
             */
            virtual double pdf(const Vector3 &point, const Vector3 &rayDir, Light *light);
        };

    }
}

#endif //KAGUYA_LIGHTHITPDF_H
