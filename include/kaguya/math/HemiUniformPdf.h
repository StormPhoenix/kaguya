//
// Created by Storm Phoenix on 2020/9/30.
//

#ifndef KAGUYA_HEMIUNIFORMPDF_H
#define KAGUYA_HEMIUNIFORMPDF_H

#include <kaguya/math/ScatterPdf.h>

namespace kaguya {
    namespace math {

        class HemiUniformPdf : public ScatterPdf {
        public:
            double pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) override;

            Vector3 random(const Vector3 &inDir, const Vector3 &normal, float &samplePdf) override;
        };
    }
}

#endif //KAGUYA_HEMIUNIFORMPDF_H
