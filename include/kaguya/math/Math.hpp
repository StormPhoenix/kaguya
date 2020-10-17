//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_MATH_HPP
#define KAGUYA_MATH_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <math.h>

#include <functional>
#include <random>

// TODO 相关函数分离定义到 core 里面

using Vector2 = glm::dvec2;
using Vector3 = glm::dvec3;
using Vector4 = glm::dvec4;
using Matrix4 = glm::dmat4x4;
using Matrix3 = glm::dmat3x3;

const double infinity = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;
const double INV_PI = 1.0 / PI;
const double EPSILON = 10e-6f;
const double REFRACTION_INDEX_WATER = 1.0f;

#define ROTATE(matrix, radius, axis) glm::rotate(matrix, glm::radians(radius), axis)
#define TRANSLATE(matrix, offset) glm::translate(matrix, offset)
#define SCALE(matrix, factor) glm::scale(matrix, factor)
#define INVERSE(matrix) glm::inverse(matrix)
#define INVERSE_TRANSPOSE(matrix) glm::inverseTranspose(matrix)
#define DETERMINANT(x) glm::determinant(x)
#define DOT(x, y) glm::dot(x, y)
#define NORMALIZE(x) glm::normalize(x)
#define LENGTH(x) glm::length(x)
#define CROSS(x, y) glm::cross(x, y)

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

inline double schlick(double cosine, double ref_idx) {
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

inline double uniformSample() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    static std::function<double()> rand_generator =
            std::bind(distribution, generator);
    return rand_generator();
}

inline double randomDouble(double min, double max) {
    return min + (max - min) * uniformSample();
}

inline int randomInt(int min, int max) {
    return static_cast<int>(randomDouble(min, max + 1));
}

inline Vector3 reflect(const Vector3 &v, const Vector3 &normal) {
    return v - 2 * DOT(v, NORMALIZE(normal)) * normal;
}

// TODO delete
inline Vector3 refract(const Vector3 &v, const Vector3 &normal, double refraction) {
    Vector3 perpendicularToNormal = refraction * (v + (DOT(-v, normal)) * normal);
    Vector3 parallelToNormal = sqrt(1 - pow(LENGTH(perpendicularToNormal), 2)) * -normal;
    return perpendicularToNormal + parallelToNormal;
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


#define DEGREES_TO_RADIANS(degrees) degreesToRadians(degrees)

#endif //KAGUYA_MATH_HPP
