//
// Created by Storm Phoenix on 2020/9/30.
//

#ifndef KAGUYA_SCATTERPDF_H
#define KAGUYA_SCATTERPDF_H

#include <kaguya/math/Math.hpp>

namespace kaguya {
    namespace math {
        /**
         * 三维空间概率密度函数，用于生成三维向量
         */
        class ScatterPdf {
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
            virtual Vector3 random(const Vector3 &inDir, const Vector3 &normal, double &samplePdf) = 0;
        };
    }
}

#endif //KAGUYA_SCATTERPDF_H
