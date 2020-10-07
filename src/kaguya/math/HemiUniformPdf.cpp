//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/math/HemiUniformPdf.h>

namespace kaguya {
    namespace math {
        double HemiUniformPdf::pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) {
            double cosine = DOT(NORMALIZE(normal), NORMALIZE(outDir));
            // p(direction) = 1 / (2 * Pi)
            return cosine < 0 ? 0 : 1 / (2 * PI);
        }

        Vector3 HemiUniformPdf::random(const Vector3 &inDir, const Vector3 &normal, float &samplePdf) {
            // fi = 2 * Pi * sampleX
            double sampleX = uniformSample();
            // sampleY = 1 - cos(theta)
            double sampleY = uniformSample();
            // x = sin(theta) * cos(fi)
            float x = sqrt(1 - pow(1 - sampleY, 2)) * cos(2 * PI * sampleX);
            // y = cos(theta)
            float y = 1 - sampleY;
            // z = - sin(theta) * sin(fi)
            float z = -sqrt(1 - pow(1 - sampleY, 2)) * sin(2 * PI * sampleX);

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