//
// Created by Storm Phoenix on 2020/10/2.
//

#ifndef KAGUYA_FUZZYREFLECTPDF_H
#define KAGUYA_FUZZYREFLECTPDF_H

#include <kaguya/math/ScatterSampler.h>

namespace kaguya {
    namespace math {

        class FuzzyReflectPdf : public ScatterSampler {
        public:
            /**
             * 按照 fuzzy 角度计算 outDirt 的 pdf
             * @param inDir
             * @param normal
             * @param outDir
             * @return
             */
            double pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir);
            /**
             * 按照 fuzzy 角度做 2 / fuzzy 采样
             * @param inDir
             * @param normal
             * @return
             */
            Vector3 random(const Vector3 &inDir, const Vector3 &normal);
        };

    }
}

#endif //KAGUYA_FUZZYREFLECTPDF_H
