//
// Created by Storm Phoenix on 2020/10/2.
//

#ifndef KAGUYA_PRECISEREFLECTPDF_H
#define KAGUYA_PRECISEREFLECTPDF_H

#include "ScatterSampler.h"

namespace kaguya {
    namespace math {

        class PreciseReflectPdf : public ScatterSampler {
        public:
            /**
             * pdf 默认为 1
             * @param inDir
             * @param normal
             * @param outDir
             * @return
             */
            double pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir);

            /**
             * 只采样光线反射方向
             * @param inDir
             * @param normal
             * @return
             */
            Vector3 random(const Vector3 &inDir, const Vector3 &normal);
        };

    }
}

#endif //KAGUYA_PRECISEREFLECTPDF_H
