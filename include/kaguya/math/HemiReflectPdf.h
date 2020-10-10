//
// Created by Storm Phoenix on 2020/10/1.
//

#ifndef KAGUYA_HEMIREFLECTPDF_H
#define KAGUYA_HEMIREFLECTPDF_H

#include <kaguya/math/ScatterPdf.h>

namespace kaguya {
    namespace math {

        class HemiReflectPdf : public ScatterPdf {
        public:
            virtual double pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) override;

            virtual Vector3 random(const Vector3 &inDir, const Vector3 &normal, double &samplePdf) override;
        };

    }
}

#endif //KAGUYA_HEMIREFLECTPDF_H
