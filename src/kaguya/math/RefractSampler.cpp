//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/math/RefractSampler.h>

namespace kaguya {
    namespace math {

        RefractSampler::RefractSampler(double refractiveIndex) : _refractiveIndex(refractiveIndex) {}

        Vector3 RefractSampler::sample(const Vector3 &inDir, const Vector3 &normal, double &samplePdf) {
            Vector3 normalizedInDir = NORMALIZE(inDir);

            double cosine = DOT(normalizedInDir, NORMALIZE(normal));
            double refraction;
            Vector3 refractNormal;

            // 计算折射比例，并保证后续计算用到的法线是与 inDir 在同一侧的
            if (cosine < 0) {
                //  外部入射
                refraction = REFRACTION_INDEX_WATER / _refractiveIndex;
                refractNormal = NORMALIZE(normal);
            } else {
                // 内部入射
                refraction = _refractiveIndex / REFRACTION_INDEX_WATER;
                refractNormal = -NORMALIZE(normal);
            }

            // 判断是否发生全反射
            double sine = sqrt(1 - pow(abs(cosine), 2));
            if (refraction * sine > 1) {
                // 全反射
                samplePdf = 1.0f;
                return NORMALIZE(reflect(normalizedInDir, refractNormal));
            } else {
                // 计算反射概率
                double reflectProb = std::max(schlick(abs(cosine), refraction), 0.0);
                if (uniformSample() < reflectProb) {
                    // 反射
                    samplePdf = reflectProb;
                    return NORMALIZE(reflect(normalizedInDir, refractNormal));
                } else {
                    // 折射
                    samplePdf = 1.0 - reflectProb;
                    return NORMALIZE(refract(normalizedInDir, refractNormal, refraction));
                }
            }
        }

        double RefractSampler::pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) {
            Vector3 normalizedInDir = NORMALIZE(inDir);
            Vector3 normalizedOutDir = NORMALIZE(outDir);

            // 计算折射比例
            double cosine = DOT(normalizedInDir, NORMALIZE(normal));
            double refraction;
            Vector3 refractNormal;
            if (cosine < 0) {
                //  外部入射
                refraction = REFRACTION_INDEX_WATER / _refractiveIndex;
                refractNormal = normal;
            } else {
                // 内部入射
                refraction = _refractiveIndex / REFRACTION_INDEX_WATER;
                refractNormal = -normal;
            }

            double sine = sqrt(1 - pow(abs(cosine), 2));
            if (refraction * sine > 1) {
                // 全反射
                Vector3 reflectDir = reflect(normalizedInDir, refractNormal);

                if (abs(normalizedOutDir.x - reflectDir.x) < EPSILON &&
                    abs(normalizedOutDir.y - reflectDir.y) < EPSILON &&
                    abs(normalizedOutDir.z - reflectDir.z) < EPSILON) {
                    return 1.0f;
                } else {
                    return 0.0f;
                }
            } else {
                // 计算反射概率
                double reflectProb = std::max(schlick(abs(cosine), refraction), 0.0);

                // 计算反射光线
                Vector3 reflectDir = reflect(normalizedInDir, refractNormal);
                if (abs(normalizedOutDir.x - reflectDir.x) < EPSILON &&
                    abs(normalizedOutDir.y - reflectDir.y) < EPSILON &&
                    abs(normalizedOutDir.z - reflectDir.z) < EPSILON) {
                    return reflectProb;
                } else {
                    // 计算折射光线
                    Vector3 refractDir = NORMALIZE(refract(normalizedInDir, refractNormal, refraction));
                    if (abs(normalizedOutDir.x - refractDir.x) < EPSILON &&
                        abs(normalizedOutDir.y - refractDir.y) < EPSILON &&
                        abs(normalizedOutDir.z - refractDir.z) < EPSILON) {
                        return 1 - reflectProb;
                    } else {
                        return 0.0f;
                    }
                }
            }
        }
    }
}