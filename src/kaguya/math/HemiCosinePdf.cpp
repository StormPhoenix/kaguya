//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/math/HemiCosinePdf.h>

namespace kaguya {
    namespace math {

        double HemiCosinePdf::pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) {
            double cosine = DOT(NORMALIZE(normal), NORMALIZE(outDir));
            // p(direction)  = cos(theta) / Pi
            return cosine < 0 ? 0 : cosine / PI;
        }

        Vector3 HemiCosinePdf::random(const Vector3 &inDir, const Vector3 &normal, double &samplePdf) {
            // fi = 2 * Pi * sampleX
            double sampleX = uniformSample();
            // sampleY = sin^2(theta)
            double sampleY = uniformSample();
            // x = sin(theta) * cos(fi)
            float x = sqrt(sampleY) * cos(2 * PI * sampleX);
            // y = cos(theta)
            float y = sqrt(1 - sampleY);
            // z = - sin(theta) * sin(fi)
            float z = -sqrt(sampleY) * sin(2 * PI * sampleX);

            // 构造关于法线的切线空间
            Vector3 tanY = NORMALIZE(normal);
            Vector3 tanZ = CROSS(NORMALIZE(inDir), tanY);
            Vector3 tanX = CROSS(tanY, tanZ);
            Vector3 sample = NORMALIZE(x * tanX + y * tanY + z * tanZ);
            samplePdf = pdf(inDir, normal, sample);
            return sample;
        }
    }
}
