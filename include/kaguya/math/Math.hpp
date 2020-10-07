//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_MATH_HPP
#define KAGUYA_MATH_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

#include <functional>
#include <random>

using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Vector4 = glm::vec4;
using Matrix4 = glm::mat4;

const double infinity = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;
const double EPSILON = 10e-6f;

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
    return static_cast<int>(randomDouble(min, max+1));
}

#define ROTATE(matrix, radius, axis) glm::rotate(matrix, glm::radians(radius), axis)
#define TRANSLATE(matrix, offset) glm::translate(matrix, offset)

#define DETERMINANT(x) glm::determinant(x)
#define DOT(x, y) glm::dot(x, y)
#define NORMALIZE(x) glm::normalize(x)
#define LENGTH(x) glm::length(x)
#define CROSS(x, y) glm::cross(x, y)
#define INVERSE(x) glm::inverse(x)
#define DEGREES_TO_RADIANS(degrees) degreesToRadians(degrees)

#endif //KAGUYA_MATH_HPP
