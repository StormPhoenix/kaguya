//
// Created by Storm Phoenix on 2020/10/8.
//

#ifndef KAGUYA_REFRACTPDF_H
#define KAGUYA_REFRACTPDF_H

#include <kaguya/math/ScatterPdf.h>

namespace kaguya {
    namespace math {

        class RefractPdf : public ScatterPdf {
        public:
            RefractPdf(double refractiveIndex);

            virtual double pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) override;

            virtual Vector3 random(const Vector3 &inDir, const Vector3 &normal, double &samplePdf) override;

        private:
            float _refractiveIndex;
        };

    }
}

#endif //KAGUYA_REFRACTPDF_H
