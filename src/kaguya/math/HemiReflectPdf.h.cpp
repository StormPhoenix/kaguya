//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/math/HemiReflectSampler.h>

namespace kaguya {
    namespace math {

        double HemiReflectSampler::pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) {
            Vector3 reflectDir = reflect(inDir, normal);

            if (abs(outDir.x - reflectDir.x) < EPSILON &&
                abs(outDir.y - reflectDir.y) < EPSILON &&
                abs(outDir.z - reflectDir.z) < EPSILON) {
                return 1.0f;
            } else {
                return 0.0f;
            }
        }

        Vector3 HemiReflectSampler::sample(const Vector3 &inDir, const Vector3 &normal, double &samplePdf) {
            samplePdf = 1.0f;
            return reflect(inDir, normal);
        }

    }
}