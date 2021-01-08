//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_MATH_H
#define KAGUYA_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/compatibility.hpp>

#include <cstring>
#include <cmath>
#include <algorithm>

#include <functional>
#include <random>

using Vector2d = glm::dvec2;
using Vector2i = glm::int2;
using Point2d = Vector2d;
using Point2i = Vector2i;

using Vector3d = glm::dvec3;
using Normal3d = Vector3d;
using Point3d = Vector3d;

using Vector4d = glm::dvec4;
using Matrix4d = glm::dmat4x4;
using Matrix3d = glm::dmat3x3;


#define ROTATE(matrix, radius, axis) glm::rotate(matrix, glm::radians(radius), axis)
#define TRANSLATE(matrix, offset) glm::translate(matrix, offset)
#define SCALE(matrix, factor) glm::scale(matrix, factor)
#define INVERSE(matrix) glm::inverse(matrix)
#define INVERSE_TRANSPOSE(matrix) glm::inverseTranspose(matrix)
#define DETERMINANT(x) glm::determinant(x)
#define DOT(x, y) glm::dot(x, y)
#define ABS(x) glm::abs(x)
#define ABS_DOT(x, y) std::abs(glm::dot(x, y))
#define NORMALIZE(x) glm::normalize(x)
#define LENGTH(x) glm::length(x)
#define CROSS(x, y) glm::cross(x, y)

namespace kaguya {
    namespace math {
        namespace random {
            class Sampler;
        }

        const double maxDouble = std::numeric_limits<double>::max();
        const double infinity = std::numeric_limits<double>::infinity();
        constexpr double epsilon = std::numeric_limits<float>::epsilon() * 0.5;
        constexpr float shadowEpsilon = 0.0001;
        const double PI = 3.1415926535897932385;
        const double INV_PI = 1.0 / PI;
        const double INV_4PI = 1.0 / (4 * PI);
        const double EPSILON = 10e-6f;
        const double REFRACTION_INDEX_WATER = 1.0f;

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

        inline bool intersectBound(const Vector3d &origin, const Vector3d &direction,
                                   const Vector3d &boundMin, const Vector3d &boundMax,
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

        double randomDouble(double min, double max, random::Sampler *const sampler1D);

        int randomInt(int min, int max, math::random::Sampler *const sampler1D);

        inline int maxAbsAxis(const Vector3d v) {
            double maxValue = std::abs(v[0]);
            int axis = 0;
            for (int i = 1; i < 3; i++) {
                if (std::abs(v[i]) > maxValue) {
                    maxValue = std::abs(v[i]);
                    axis = i;
                }
            }
            return axis;
        }

        inline Vector3d swapAxis(const Vector3d v, int x, int y, int z) {
            return Vector3d(v[x], v[y], v[z]);
        }

        inline Vector3d reflect(const Vector3d &v, const Vector3d &normal) {
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
        inline bool refract(const Vector3d &wo, const Vector3d &normal, double refraction, Vector3d *wi) {
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

        inline double gamma(int n) {
            return (n * epsilon) / (1 - n * epsilon);
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
        Vector3d hemiCosineSampling(random::Sampler *const sampler);

        /**
         * （局部坐标系）
         * 在球面做均匀采样，默认 (0, 1, 0) 为法线方向
         * @return
         */
        Vector3d sphereUniformSampling(math::random::Sampler *sampler);

        /**
         * （局部坐标系）
         * 计算 sample1d 在 hemi-sphere cosine 分布下的 pdf
         * @param sample
         * @return
         */
        inline double hemiCosineSamplePdf(Vector3d sample) {
            double cosine = NORMALIZE(sample).y;
            return cosine < 0 ? 0 : cosine * INV_PI;
        }

        inline double hemiCosineSamplePdf(double cosTheta) {
            return cosTheta / PI;
        }

        inline bool isValid(const Vector3d v) {
            return !std::isnan(v.x) && !std::isnan(v.y) && !std::isnan(v.z);
        }

        /**
         * 计算切线空间
         * @param tanY 切线空间 Y 轴
         * @param tanX 切线空间 X 轴
         * @param tanZ 切线空间 Z 轴
         */
        inline void tangentSpace(Vector3d &tanY, Vector3d *tanX, Vector3d *tanZ) {
            // 计算与 tanY 垂直的 tanX
            if (std::abs(tanY.x) > std::abs(tanY.y)) {
                (*tanX) = Vector3d(-tanY.z, 0, tanY.x);
            } else {
                (*tanX) = Vector3d(0, tanY.z, -tanY.y);
            }
            (*tanX) = NORMALIZE(*tanX);
            (*tanZ) = NORMALIZE(CROSS(tanY, *tanX));
        }

        /**
         * 对圆盘做均匀采样
         * @return
         */
        Vector2d diskUniformSampling(math::random::Sampler *const sampler1D, double radius = 1.);

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
         * @param sampler
         * @return
         */
        Vector2d triangleUniformSampling(random::Sampler *sampler);

        /**
         * 从 cone 空间中均匀采样射线
         * @param cosThetaMax
         * @return
         */
        Vector3d coneUniformSampling(double cosThetaMax, random::Sampler *sampler);

        /**
         * 计算 cone 空间中均匀采样射线的 pdf
         * @param cosThetaMax
         * @return
         */
        inline double coneUniformSamplePdf(double cosThetaMax) {
            return 1 / (2 * PI * (1 - cosThetaMax));
        }

        inline uint64_t double2bits(double v) {
            uint64_t bits;
            memcpy(&bits, &v, sizeof(double));
            return bits;
        }

        inline double bits2double(uint64_t bits) {
            double d;
            memcpy(&d, &bits, sizeof(uint64_t));
            return d;
        }

        inline double doubleUp(double a) {
            if (std::isinf(a) && a > 0) {
                return a;
            }

            if (a == -0.) {
                a = 0.;
            }

            uint64_t bits = double2bits(a);
            if (bits >= 0) {
                bits++;
            } else {
                bits--;
            }
            return bits2double(bits);
        }

        inline double doubleDown(double a) {
            if (std::isinf(a) && a < 0) {
                return a;
            }

            if (a == 0.) {
                a = -0.;
            }

            uint64_t bits = double2bits(a);
            if (bits > 0) {
                bits--;
            } else {
                bits++;
            }
            return bits2double(bits);
        }

        inline int mod(int a, int b) {
            int rest = a - (a / b) * b;
            return (rest < 0) ? rest + b : rest;
        }

        /**
         * solve x, y, r, make a * x + b * y = r = gcd(a, b)
         */
        static int extendGCD(uint64_t a, uint64_t b, int64_t *x, int64_t *y) {
            if (b == 0) {
                *x = 1;
                *y = 0;
                return a;
            }

            int r = extendGCD(b, a % b, x, y);
            int t = *y;
            *y = *x - a / b * *y;
            *x = t;
            return r;
        }

        namespace low_discrepancy {
            const int primeArraySize = 1000;

            extern const int primes[primeArraySize];

            extern const int primeSums[primeArraySize];

            std::vector<uint16_t> computePermutationArray();

            double radicalReverse(const int base, uint64_t n);

            template<int base>
            uint16_t inverseRadicalReverse(uint64_t reversed, int digitsCount) {
                uint64_t ret = 0;
                for (int i = 0; i < digitsCount; i++) {
                    uint64_t digit = reversed % base;
                    reversed /= base;
                    ret = ret * base + digit;
                }
                return ret;
            }

            double scrambledRadicalReverse(const int base, uint64_t n, uint16_t *perm);

            double radicalReverse(uint64_t n);

            uint64_t reverseBit64(uint64_t n);

            uint32_t reverseBit32(uint32_t n);
        }
    }
}

#define DEGREES_TO_RADIANS(degrees) degreesToRadians(degrees)

#endif //KAGUYA_MATH_H
