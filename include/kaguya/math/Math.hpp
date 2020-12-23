//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_MATH_HPP
#define KAGUYA_MATH_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <math.h>
#include <algorithm>

#include <functional>
#include <random>
#include <kaguya/math/Sampler.hpp>

// TODO 相关函数分离定义到 core 里面

using Vector2 = glm::dvec2;
using Vector3 = glm::dvec3;
using Vector4 = glm::dvec4;
using Matrix4 = glm::dmat4x4;
using Matrix3 = glm::dmat3x3;

using kaguya::math::random::Sampler1D;

typedef Vector2 Point2d;

const double maxDouble = std::numeric_limits<double>::max();
const double infinity = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;
const double INV_PI = 1.0 / PI;
const double INV_4PI = 1.0 / (4 * PI);
const double EPSILON = 10e-6f;
const double REFRACTION_INDEX_WATER = 1.0f;

#define ROTATE(matrix, radius, axis) glm::rotate(matrix, glm::radians(radius), axis)
#define TRANSLATE(matrix, offset) glm::translate(matrix, offset)
#define SCALE(matrix, factor) glm::scale(matrix, factor)
#define INVERSE(matrix) glm::inverse(matrix)
#define INVERSE_TRANSPOSE(matrix) glm::inverseTranspose(matrix)
#define DETERMINANT(x) glm::determinant(x)
#define DOT(x, y) glm::dot(x, y)
#define ABS_DOT(x, y) std::abs(glm::dot(x, y))
#define NORMALIZE(x) glm::normalize(x)
#define LENGTH(x) glm::length(x)
#define CROSS(x, y) glm::cross(x, y)

using namespace kaguya;

inline double degreesToRadians(double degrees) {
    return degrees * PI / 180;
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline bool checkRange(double num, double min, double max) {
    return num >= min && num <= max;
}

inline double linearInterpolation(double t, double a, double b) {
    return (1 - t) * a + t * b;
}

inline bool intersectBound(const Vector3 &origin, const Vector3 &direction,
                           const Vector3 &boundMin, const Vector3 &boundMax,
                           double *stepMin, double *maxStep) {
    assert(stepMin != nullptr && maxStep != nullptr);

    double t0 = *stepMin, t1 = *maxStep;
    for (int axis = 0; axis < 3; ++axis) {
        double invStep = 1 / direction[axis];
        double near = (boundMin[axis] - origin[axis]) * invStep;
        double far = (boundMax[axis] - origin[axis]) * invStep;

        if (near > far) {
            std::swap(near, far);
        }

        t0 = near > t0 ? near : t0;
        t1 = far < t1 ? far : t1;
        if (t0 > t1) {
            return false;
        }
    }

    *stepMin = t0;
    *maxStep = t1;
    return true;
}

inline double schlick(double cosine, double ref_idx) {
    if (cosine < 0) {
        ref_idx = 1 / ref_idx;
        cosine = -cosine;
    }

    double sine = std::sqrt(std::max(0.0, 1 - std::pow(cosine, 2)));
    if (sine * ref_idx >= 1) {
        return 1.0;
    }

    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

inline double randomDouble(double min, double max, const math::random::Sampler1D *const sampler1D) {
    return min + (max - min) * sampler1D->sample();
}

inline int randomInt(int min, int max, const math::random::Sampler1D *const sampler1D) {
    int ret = std::min(static_cast<int>(randomDouble(min, max + 1, sampler1D)), max);
    if (ret <= max) {
        return ret;
    } else {
        return randomInt(min, max, sampler1D);
    }
}

inline Vector3 reflect(const Vector3 &v, const Vector3 &normal) {
    return v - 2 * DOT(v, NORMALIZE(normal)) * normal;
}

/**
 * 折射 wo，如果发射全反射，则返回 false，否则返回返回 true
 * @param wo
 * @param normal normal 与 wo 必须在同一侧
 * @param refraction
 * @param wi
 * @return
 */
inline bool refract(const Vector3 &wo, const Vector3 &normal, double refraction, Vector3 *wi) {
    double cosineThetaI = DOT(wo, normal);
    double sineThetaI = std::sqrt(std::max(0.0, 1 - cosineThetaI * cosineThetaI));
    double sineThetaT = refraction * sineThetaI;
    if (sineThetaT > 1) {
        // 全反射，无法折射
        return false;
    }

    double cosineThetaT = std::sqrt(std::max(0.0, 1 - sineThetaT * sineThetaT));
    *wi = refraction * (-wo) + (refraction * cosineThetaI - cosineThetaT) * normal;
    return true;
}

inline double misWeight(int nSampleF, double pdfF, int nSampleG, double pdfG) {
    double f = nSampleF * pdfF;
    double g = nSampleG * pdfG;
    return (f * f) / (g * g + f * f);
}

/**
 * （局部坐标系）
 * 从 y > 0 的半球面，按照 cos(theta) / Pi 的概率采样射线
 * @return
 */
inline Vector3 hemiCosineSampling(const math::random::Sampler1D *const sampler1D) {
    // fi = 2 * Pi * sampleU
    double sampleU = sampler1D->sample();
    // sampleV = sin^2(theta)
    double sampleV = sampler1D->sample();
    // x = sin(theta) * cos(fi)
    double x = sqrt(sampleV) * cos(2 * PI * sampleU);
    // y = cos(theta)
    double y = sqrt(1 - sampleV);
    // z = sin(theta) * sin(fi)
    double z = sqrt(sampleV) * sin(2 * PI * sampleU);
    return NORMALIZE(Vector3(x, y, z));
}

/**
 * （局部坐标系）
 * 在球面做均匀采样，默认 (0, 1, 0) 为法线方向
 * @return
 */
inline Vector3 sphereUniformSampling(const math::random::Sampler1D *sampler1D) {
    // phi = 2 * Pi * sampleU
    double sampleU = sampler1D->sample();
    // 2 * sampleV = 1 - cos(theta)
    double sampleV = sampler1D->sample();

    // y = 1 - 2 * sampleV
    double y = 1 - 2 * sampleV;
    // x = sin(theta) * cos(phi)
    double x = std::sqrt(std::max(0.0, (1 - y * y))) * std::cos(2 * PI * sampleU);
    // z = sin(theta) * sin(phi)
    double z = std::sqrt(std::max(0.0, (1 - y * y))) * std::sin(2 * PI * sampleU);

    return NORMALIZE(Vector3(x, y, z));
}

/**
 * （局部坐标系）
 * 计算 sample 在 hemi-sphere cosine 分布下的 pdf
 * @param sample
 * @return
 */
inline double hemiCosineSamplePdf(Vector3 sample) {
    double cosine = NORMALIZE(sample).y;
    return cosine < 0 ? 0 : cosine * INV_PI;
}

inline double hemiCosineSamplePdf(double cosTheta) {
    return cosTheta / PI;
}

inline bool isValid(const Vector3 v) {
    return !std::isnan(v.x) && !std::isnan(v.y) && !std::isnan(v.z);
}

/**
 * 计算切线空间
 * @param tanY 切线空间 Y 轴
 * @param tanX 切线空间 X 轴
 * @param tanZ 切线空间 Z 轴
 */
inline void tangentSpace(Vector3 &tanY, Vector3 *tanX, Vector3 *tanZ) {
    // 计算与 tanY 垂直的 tanX
    if (std::abs(tanY.x) > std::abs(tanY.y)) {
        (*tanX) = Vector3(-tanY.z, 0, tanY.x);
    } else {
        (*tanX) = Vector3(0, tanY.z, -tanY.y);
    }
    (*tanX) = NORMALIZE(*tanX);
    (*tanZ) = NORMALIZE(CROSS(tanY, *tanX));
}

/**
 * 对圆盘做均匀采样
 * @return
 */
inline Vector2 diskUniformSampling(const math::random::Sampler1D *const sampler1D, double radius = 1.) {
    // sampleY = r / Radius
    // sampleX = theta / (2 * PI)
    double sampleY = sampler1D->sample();
    double sampleX = sampler1D->sample();

    double theta = 2 * PI * sampleX;
    double r = sampleY * radius;

    return Vector2(r * std::cos(theta), r * std::sin(theta));
}

/**
 * Sample barycentric coordinate from triangle
 *
 * 1 - u = sqrt(1 - sampleU)
 * v = sampleV * sqrt(1 - sampleU)
 *
 * ->
 *
 * 1 - u = sqrt(sampleU)
 * v = sampleV * sqrt(sampleU)
 *
 * @param sampler1D
 * @return
 */
inline Vector2 triangleUniformSampling(const Sampler1D *sampler1D) {
    double sampleU = sampler1D->sample();
    double sampleV = sampler1D->sample();

    double u = 1 - std::sqrt(sampleU);
    double v = sampleV * sqrt(sampleU);
    return Vector2(u, v);
}


/**
 * 从 cone 空间中均匀采样射线
 * @param cosThetaMax
 * @return
 */
inline Vector3 coneUniformSampling(double cosThetaMax, const Sampler1D *sampler1D) {
    // phi = 2 * PI * sampleU
    double sampleU = sampler1D->sample();
    // sampleV = (1 - cos(theta)) / (1 - cos(thetaMax))
    double sampleV = sampler1D->sample();

    // 计算 cos(theta) sin(theta)
    double cosTheta = 1.0 - sampleV + sampleV * cosThetaMax;
    double sinTheta = std::sqrt(std::max(0.0, 1 - cosTheta * cosTheta));
    // 计算 cos(phi) sin(phi)
    double cosPhi = std::cos(2 * PI * sampleU);
    double sinPhi = std::sin(2 * PI * sampleU);

    // y = cos(theta)
    double y = cosTheta;
    // x = sin(theta) * cos(phi)
    double x = sinTheta * cosPhi;
    // z = sin(theta) * sin(phi)
    double z = sinTheta * sinPhi;

    return NORMALIZE(Vector3(x, y, z));
}

/**
 * 计算 cone 空间中均匀采样射线的 pdf
 * @param cosThetaMax
 * @return
 */
inline double coneUniformSamplePdf(double cosThetaMax) {
    return 1 / (2 * PI * (1 - cosThetaMax));
}

#define DEGREES_TO_RADIANS(degrees) degreesToRadians(degrees)

#endif //KAGUYA_MATH_HPP
