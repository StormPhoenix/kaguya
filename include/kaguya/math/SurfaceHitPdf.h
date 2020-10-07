//
// Created by Storm Phoenix on 2020/10/1.
//

#ifndef KAGUYA_SURFACEHITPDF_H
#define KAGUYA_SURFACEHITPDF_H

#include <kaguya/math/Math.hpp>

namespace kaguya {
    namespace math {

        /**
         * 从某点发射线击中某一表面的概率
         */
        class SurfaceHitPdf {
        public:
            virtual double pdf(const Vector3 &point, Vector3 &direction) = 0;

            virtual Vector3 sample(const Vector3 &point) = 0;
        };

    }
}

#endif //KAGUYA_SURFACEHITPDF_H
