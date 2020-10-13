//
// Created by Storm Phoenix on 2020/9/30.
//

#ifndef KAGUYA_SCATTERSAMPLER_H
#define KAGUYA_SCATTERSAMPLER_H

#include <kaguya/math/Math.hpp>

namespace kaguya {
    namespace math {
        /**
         * 针对入射光线，采样其反射光线以及计算反射光线的 pdf
         */
        class ScatterSampler {
        public:
            /**
             * 计算 outDir 方向的光线的采样概率
             * @param inDir
             * @param normal
             * @param outDir
             * @return
             */
            virtual double pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) = 0;

            /**
             * 随机采样一个方向
             *
             * @param inDir
             * @param normal
             * @param samplePdf 采样概率
             * @return
             */
            virtual Vector3 sample(const Vector3 &inDir, const Vector3 &normal, double &samplePdf) = 0;
        };
    }
}

#endif //KAGUYA_SCATTERSAMPLER_H
