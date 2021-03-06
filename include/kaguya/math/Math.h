//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_MATH_H
#define KAGUYA_MATH_H

#include <ext/glm/glm.hpp>
#include <ext/glm/gtc/matrix_transform.hpp>
#include <ext/glm/gtc/matrix_inverse.hpp>
#include <ext/glm/gtx/compatibility.hpp>

#include <cstring>
#include <cmath>
#include <algorithm>

#include <functional>
#include <random>

#include <kaguya/Common.h>

using Vector2d = glm::dvec2;
using Vector2f = glm::vec2;
using Vector2i = glm::int2;

using Vector3i = glm::int3;
using Vector3d = glm::dvec3;
using Vector3f = glm::vec3;

using Vector4d = glm::dvec4;
using Vector4i = glm::int4;
using Vector4f = glm::vec4;

using Color = glm::int3;

using Matrix4d = glm::dmat4x4;
using Matrix3d = glm::dmat3x3;

using Matrix4f = glm::mat4x4;
using Matrix3f = glm::mat3x3;

//#define RENDER_DATA_DOUBLE

#if defined(RENDER_DATA_DOUBLE)
using Float = double;
using Vector4F = Vector4d;
using Vector3F = Vector3d;
using Vector2F = Vector2d;
using Matrix4F = Matrix4d;
using Matrix3F = Matrix3d;
#else
using Float = float;
using Vector4F = Vector4f;
using Vector3F = Vector3f;
using Vector2F = Vector2f;
using Matrix4F = Matrix4f;
using Matrix3F = Matrix3f;
#endif

using Point2F = Vector2F;
using Point3F = Vector3F;
using Point4F = Vector4F;
using Normal3F = Vector3F;
using Point2I = Vector2i;
using Point3I = Vector3i;
using Point4I= Vector4i;

#define RADIANS(radius) glm::radians(radius)
#define ROTATE(matrix, radius, axis) glm::rotate(matrix, glm::radians(radius), axis)
#define TRANSLATE(matrix, offset) glm::translate(matrix, offset)
#define SCALE(matrix, factor) glm::scale(matrix, factor)
#define INVERSE(matrix) glm::inverse(matrix)
#define INVERSE_TRANSPOSE(matrix) glm::inverseTranspose(matrix)
#define DETERMINANT(x) glm::determinant(x)
#define PERSPECTIVE(fovy, aspect, zNear, zFar) glm::perspective(glm::radians(fovy), aspect, zNear, zFar)
#define DOT(x, y) glm::dot(x, y)
#define ABS(x) glm::abs(x)
#define ABS_DOT(x, y) std::abs(glm::dot(x, y))
#define NORMALIZE(x) glm::normalize(x)
#define LENGTH(x) glm::length(x)
#define CROSS(x, y) glm::cross(x, y)
#define LOOK_AT(eye, center, up) glm::lookAt(eye, center, up)

namespace kaguya {
    namespace sampler {
        class Sampler;
    }

    namespace math {

#ifdef WINDOWS
        const double doubleOneMinusEpsilon = 0.99999999999999989;
        const float floatOneMinusEpsilon = 0.99999994;
#else
		const double doubleOneMinusEpsilon = 0x1.fffffffffffffp-1;
		const float floatOneMinusEpsilon = 0x1.fffffep-1;
#endif

#if defined(RENDER_DATA_DOUBLE)
        const Float ONE_MINUS_EPSILON = doubleOneMinusEpsilon;
#else
        const Float ONE_MINUS_EPSILON = floatOneMinusEpsilon;
#endif

#ifdef WINDOWS
#define MAX_FLOAT std::numeric_limits<Float>::max()
#else
		constexpr Float MAX_FLOAT = std::numeric_limits<Float>::max();
#endif
        
        constexpr Float infinity = std::numeric_limits<Float>::infinity();
        constexpr Float epsilon = std::numeric_limits<Float>::epsilon() * 0.5;
        constexpr Float shadowEpsilon = 0.0001;
        const double PI = 3.1415926535897932385;
        const Float INV_PI = 1.0 / PI;
        const Float INV_2PI = 1.0 / (2 * PI);
        const Float INV_4PI = 1.0 / (4 * PI);
        const Float EPSILON = 10e-6f;
        const Float REFRACTION_INDEX_WATER = 1.0f;

        using namespace kaguya;
        using sampler::Sampler;

        class Bound3 {
        public:
            Bound3() {
                Float minNum = std::numeric_limits<Float>::lowest();
                Float maxNum = (std::numeric_limits<Float>::max)();
                _min = Point3F(maxNum, maxNum, maxNum);
                _max = Point3F(minNum, minNum, minNum);
            }

            Bound3(const Point3F &p) : _min(p), _max(p) {}

            Bound3(const Point3F &min, const Point3F &max) : _min(min), _max(max) {}

            void expand(Float delta) {
                _min -= delta;
                _max += delta;
            }

            Vector3F diagonal() {
                return _max - _min;
            }

            void merge(const Bound3 &b) {
                _min = {(std::min)(_min.x, b._min.x),
						(std::min)(_min.y, b._min.y),
						(std::min)(_min.z, b._min.z)};

                _max = {(std::max)(_max.x, b._max.x),
						(std::max)(_max.y, b._max.y),
						(std::max)(_max.z, b._max.z)};
            }

            Vector3F offset(const Point3F &p) const {
                return p - _min;
            }

        private:
            Vector3F _min;
            Vector3F _max;
        };

        inline Float degreesToRadians(double degrees) {
            return degrees * PI / 180;
        }

        inline Float distance(const Point3F &p1, const Point3F &p2) {
            return (p2 - p1).length();
        }

        inline Float distanceSquare(const Point3F &p1, const Point3F &p2) {
            Float len = LENGTH(p2 - p1);
            return len * len;
        }

        /* [min, max] */
        template<typename T, typename U, typename V>
        T clamp(T x, U min, V max) {
            if (x < min) return min;
            if (x > max) return max;
            return x;
        }

        inline Float phaseFuncHG(Float cosTheta, Float g) {
            Float denominator = 1 + g * g + 2 * g * cosTheta;
            return INV_4PI * (1 - g * g) / (denominator * std::sqrt(denominator));
        }

        inline bool checkRange(Float num, Float min, Float max) {
            return num >= min && num <= max;
        }

        inline Float linearInterpolation(Float t, Float a, Float b) {
            return (1 - t) * a + t * b;
        }

        inline bool intersectBound(const Vector3F &origin, const Vector3F &direction,
                                   const Vector3F &boundMin, const Vector3F &boundMax,
                                   Float *stepMin, Float *stepMax) {
            ASSERT(stepMin != nullptr && stepMax != nullptr, "stepMin | maxStep is nullptr");

            Float t0 = *stepMin, t1 = *stepMax;
            for (int axis = 0; axis < 3; ++axis) {
                Float invStep = 1 / direction[axis];
                Float nearT = (boundMin[axis] - origin[axis]) * invStep;
                Float farT = (boundMax[axis] - origin[axis]) * invStep;

                if (nearT > farT) {
                    std::swap(nearT, farT);
                }

                t0 = nearT > t0 ? nearT : t0;
                t1 = farT < t1 ? farT : t1;
                if (t0 > t1) {
                    return false;
                }
            }

            *stepMin = t0;
            *stepMax = t1;
            return true;
        }

        Float randomDouble(Float min, Float max, Sampler *const sampler);

        int randomInt(int min, int max, Sampler *const sampler);

        inline Float maxAbsAxisValue(const Vector3F v) {
            Float maxValue = std::abs(v[0]);
            int axis = 0;
            for (int i = 1; i < 3; i++) {
                if (std::abs(v[i]) > maxValue) {
                    maxValue = std::abs(v[i]);
                    axis = i;
                }
            }
            return maxValue;
        }

        inline int maxAbsAxis(const Vector3F v) {
            Float maxValue = std::abs(v[0]);
            int axis = 0;
            for (int i = 1; i < 3; i++) {
                if (std::abs(v[i]) > maxValue) {
                    maxValue = std::abs(v[i]);
                    axis = i;
                }
            }
            return axis;
        }

        inline Vector3F swapAxis(const Vector3F v, int x, int y, int z) {
            return Vector3F(v[x], v[y], v[z]);
        }

        inline Vector3F reflect(const Vector3F &v, const Normal3F &normal) {
            return 2 * DOT(v, NORMALIZE(normal)) * normal - v;
        }

        /**
         * 折射 wo，如果发射全反射，则返回 false，否则返回返回 true
         * @param wo
         * @param normal normal 与 wo 必须在同一侧
         * @param refraction
         * @param wi
         * @return
         */
        inline bool refract(const Vector3F &wo, const Normal3F &normal, Float refraction, Vector3F *wi) {
            Float cosineThetaI = DOT(wo, normal);
            Float sineThetaI = std::sqrt((std::max)(Float(0.), 1 - cosineThetaI * cosineThetaI));
            Float sineThetaT = refraction * sineThetaI;
            if (sineThetaT >= 1) {
                // Can't do refraction
                return false;
            }

            Float cosineThetaT = std::sqrt((std::max)(Float(0.), 1 - sineThetaT * sineThetaT));
            *wi = refraction * (-wo) + (refraction * cosineThetaI - cosineThetaT) * normal;
            return true;
        }

        inline Float gamma(int n) {
            return (n * epsilon) / (1 - n * epsilon);
        }

        inline Float misWeight(int nSampleF, Float pdfF, int nSampleG, Float pdfG) {
            Float f = nSampleF * pdfF;
            Float g = nSampleG * pdfG;
            return (f * f) / (g * g + f * f);
        }

        inline bool isValid(const Vector3F v) {
            return !std::isnan(v.x) && !std::isnan(v.y) && !std::isnan(v.z);
        }

        /**
         * 计算切线空间
         * @param tanY 切线空间 Y 轴
         * @param tanX 切线空间 X 轴
         * @param tanZ 切线空间 Z 轴
         */
        inline void tangentSpace(const Vector3F &tanY, Vector3F *tanX, Vector3F *tanZ) {
            // 计算与 tanY 垂直的 tanX
            if (std::abs(tanY.x) > std::abs(tanY.y)) {
                (*tanX) = Vector3F(-tanY.z, 0, tanY.x);
            } else {
                (*tanX) = Vector3F(0, tanY.z, -tanY.y);
            }
            (*tanX) = NORMALIZE(*tanX);
            (*tanZ) = NORMALIZE(CROSS(tanY, *tanX));
        }

        inline uint32_t float2bits(float v) {
            uint32_t bits;
            memcpy(&bits, &v, sizeof(float));
            return bits;
        }

        inline float bits2float(uint32_t bits) {
            float f;
            memcpy(&f, &bits, sizeof(uint32_t));
            return f;
        }

        inline uint64_t float2bits(double v) {
            uint64_t bits;
            memcpy(&bits, &v, sizeof(double));
            return bits;
        }

        inline double bits2float(uint64_t bits) {
            double f;
            memcpy(&f, &bits, sizeof(uint64_t));
            return f;
        }

        inline float floatUp(float a) {
            if (std::isinf(a) && a > 0.) {
                return a;
            }

            if (a == -0.) {
                a = 0.f;
            }

            uint32_t bits = float2bits(a);
            if (a >= 0) {
                bits++;
            } else {
                bits--;
            }
            return bits2float(bits);
        }


        inline float floatDown(float a) {
            if (std::isinf(a) && a < 0) {
                return a;
            }

            if (a == 0.f) {
                a = -0.f;
            }

            uint32_t bits = float2bits(a);
            if (a > 0) {
                bits--;
            } else {
                bits++;
            }
            return bits2float(bits);
        }

        inline double floatUp(double a, int delta = 1) {
            if (std::isinf(a) && a > 0.) {
                return a;
            }

            if (a == -0.f) {
                a = 0.f;
            }

            uint64_t bits = float2bits(a);
            if (a >= 0.) {
                bits += delta;
            } else {
                bits -= delta;
            }
            return bits2float(bits);
        }

        inline double floatDown(double a, int delta = 1) {
            if (std::isinf(a) && a < 0.f) {
                return a;
            }

            if (a == 0.f) {
                a = -0.f;
            }

            uint64_t bits = float2bits(a);
            if (a > 0.) {
                bits -= delta;
            } else {
                bits += delta;
            }
            return bits2float(bits);
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

        namespace local_coord {
            inline Float dirToTheta(const Vector3F &dir) {
                return std::acos(clamp(dir.y, -1, 1));
            }

            inline Float dirToPhi(const Vector3F &dir) {
                Float phi = std::atan2(dir.z, dir.x);
                if (phi < 0) {
                    phi += 2 * PI;
                }
                return phi;
            }

            inline Float cosTheta(const Vector3F &v) {
                return v.y;
            }

            inline Float absCosTheta(const Vector3F &v) {
                return std::abs(v.y);
            }

            inline Float cosTheta2(const Vector3F &v) {
                return v.y * v.y;
            }

            inline Float sinTheta2(const Vector3F &v) {
                return std::max(Float(0.), 1 - cosTheta2(v));
            }

            inline Float sinTheta(const Vector3F &v) {
                return std::sqrt(sinTheta2(v));
            }

            inline Float tanTheta(const Vector3F &v) {
                return sinTheta(v) / cosTheta(v);
            }

            inline Float tanTheta2(const Vector3F &v) {
                return sinTheta2(v) / cosTheta2(v);
            }

            inline Float cosPhi(const Vector3F &v) {
                Float sin = sinTheta(v);
                if (sin == 0) {
                    return 0.;
                }
                return clamp<Float, Float, Float>(v.x / sin, -1, 1);
            }

            inline Float sinPhi(const Vector3F &v) {
                Float sin = sinTheta(v);
                if (sin == 0) {
                    return 0.;
                }
                return clamp<Float, Float, Float>(v.z / sin, -1, 1);
            }

            inline Float sinPhi2(const Vector3F &v) {
                Float sin = sinPhi(v);
                return sin * sin;
            }

            inline Float cosPhi2(const Vector3F &v) {
                Float cos = cosPhi(v);
                return cos * cos;
            }
        }

        namespace sampling {

            /**
             * （局部坐标系）
             * 从 y > 0 的半球面，按照 cos(theta) / Pi 的概率采样射线
             * @return
             */
            Vector3F hemiCosineSampling(Sampler *const sampler);

            /**
             * （局部坐标系）
             * 在球面做均匀采样，默认 (0, 1, 0) 为法线方向
             * @return
             */
            Vector3F sphereUniformSampling(Sampler *sampler);

            /**
             * （局部坐标系）
             * 计算 sample1d 在 hemi-sphere cosine 分布下的 pdf
             * @param sample
             * @return
             */
            inline Float hemiCosineSamplePdf(Vector3F sample) {
                Float cosine = NORMALIZE(sample).y;
                return cosine < 0 ? 0 : cosine * INV_PI;
            }

            inline Float hemiCosineSamplePdf(Float cosTheta) {
                return cosTheta / PI;
            }

            /**
             * 对圆盘做均匀采样
             * @return
             */
            Vector2F diskUniformSampling(Sampler *const sampler, Float radius = 1.);

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
            Vector2F triangleUniformSampling(Sampler *sampler);

            /**
             * 从 cone 空间中均匀采样射线
             * @param cosThetaMax
             * @return
             */
            Vector3F coneUniformSampling(Float cosThetaMax, Sampler *sampler);

            /**
             * 计算 cone 空间中均匀采样射线的 pdf
             * @param cosThetaMax
             * @return
             */
            inline Float coneUniformSamplePdf(Float cosThetaMax) {
                return 1 / (2 * PI * (1 - cosThetaMax));
            }

            namespace low_discrepancy {
                const uint16_t primeArraySize = 256;

                extern const uint16_t primes[primeArraySize];

                extern const int primeSums[primeArraySize];

                std::vector<uint16_t> computePermutationArray(bool initFaure = true);

                Float radicalReverse(const int dimension, uint64_t a);

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

                Float scrambledRadicalReverse(const int dimension, uint64_t n, uint16_t *perm);

                Float radicalReverse(uint64_t n);

                uint64_t reverseBit64(uint64_t n);

                uint32_t reverseBit32(uint32_t n);
            }
        }

        namespace spline_curve {
            /**
             * Integrate on catmull-rom spline curve
             */
            Float integrateCatmullRom(int n, const Float *nodes, const Float *values, Float *cdf);

            /**
             * Catmull-Rom interpolation, calculate weights
             */
            bool catmullRomWeights(int size, const Float *nodes, Float x, int *offset, Float *weights);

            /**
             * Sample catmull-rom spline
             */
            Float sampleCatmullRom(int size, const Float *nodes, const Float *f,
                                   const Float *cdf, Float sample, Float *fval = nullptr,
                                   Float *pdf = nullptr);

            /**
             * Sample 2-dimension catmull-rom spline function
             */
            Float sampleCatmullRom2D(int size1, int size2, const Float *nodes1,
                                     const Float *nodes2, const Float *values,
                                     const Float *cdf, Float alpha, Float sample,
                                     Float *fval = nullptr, Float *pdf = nullptr);

            /**
             * Invert catmull-rom spline function not its definite integral
             */
            Float invertCatmullRom(int n, const Float *x, const Float *values, Float u);
        }
    }
}

#define DEGREES_TO_RADIANS(degrees) degreesToRadians(degrees)

#endif //KAGUYA_MATH_H
