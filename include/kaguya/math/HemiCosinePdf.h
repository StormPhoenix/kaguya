//
// Created by Storm Phoenix on 2020/9/30.
//

#ifndef KAGUYA_HEMICOSINEPDF_H
#define KAGUYA_HEMICOSINEPDF_H

#include <kaguya/math/ScatterPdf.h>

namespace kaguya {
    namespace math {

        class HemiCosinePdf : public ScatterPdf {
        public:
            Vector3 random(const Vector3 &inDir, const Vector3 &normal, float &samplePdf) override;

            double pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) override;

        };

    }
}

#endif //KAGUYA_HEMICOSINEPDF_H
