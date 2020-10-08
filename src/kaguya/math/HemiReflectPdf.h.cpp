//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/math/HemiReflectPdf.h>

namespace kaguya {
    namespace math {

        double HemiReflectPdf::pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) {
            Vector3 reflectDir = reflect(inDir, normal);

            if (abs(outDir.x - reflectDir.x) < EPSILON &&
                abs(outDir.y - reflectDir.y) < EPSILON &&
                abs(outDir.z - reflectDir.z) < EPSILON) {
                return 1.0f;
            } else {
                return 0.0f;
            }
        }

        Vector3 HemiReflectPdf::random(const Vector3 &inDir, const Vector3 &normal, float &samplePdf) {
            samplePdf = 1.0f;
            return reflect(inDir, normal);
        }

    }
}