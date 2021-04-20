//
// Created by Storm Phoenix on 2021/4/19.
//

#include <kaguya/math/Math.h>
#include <kaguya/core/light/EnvironmentLight.h>
#include <kaguya/utils/IOReader.h>

namespace kaguya {
    namespace core {
        using namespace utils;

        EnvironmentLight::EnvironmentLight(Float intensity, std::string texturePath,
                                           const MediumBoundary &mediumBoundary,
                                           Transform::Ptr lightToWorld)
                : Light(INFINITE, mediumBoundary), _intensity(intensity), _lightToWorld(lightToWorld) {
            // Check file exists
            {
                std::ifstream in(texturePath);
                ASSERT(in.good(), texturePath + " NOT EXISTS.");
            }

            // Transform matrix
            if (_lightToWorld == nullptr) {
                _lightToWorld = std::make_shared<Transform>();
            }
            _worldToLight = _lightToWorld->inverse();
            _texture = io::readImage(texturePath.c_str(), &_width, &_height, &_channel, 0);
        }

        Spectrum EnvironmentLight::Le(const Ray &ray) const {
            Vector3F wi = NORMALIZE(_worldToLight->transformVector(ray.getDirection()));
            // wi.y = cosTheta
            Float theta = math::dirToTheta(wi);
            // wi.x = sinTheta * cosPhi
            // wi.z = sinTheta * sinPhi
            // wi.z / wi.x = tanPhi
            Float phi = math::dirToPhi(wi);
            Point2F uv = {phi * math::INV_2PI, theta * math::INV_PI};
            return sampleTexture(uv);
        }

        Spectrum EnvironmentLight::sampleLi(const Interaction &eye, Vector3F *wi,
                                            Float *pdf, Sampler *sampler,
                                            VisibilityTester *visibilityTester) {
            Point2F uv = sampler->sample2D();
            Float theta = math::PI * uv.y;
            Float phi = 2 * math::PI * uv.x;

            Float sinTheta = std::sin(theta);
            Float cosTheta = std::cos(theta);
            Float cosPhi = std::cos(phi);
            Float sinPhi = std::sin(phi);

            *wi = _lightToWorld->transformVector({sinTheta * cosPhi, sinTheta * sinPhi, cosTheta});
            *wi = NORMALIZE(*wi);
            if (sinTheta == 0) {
                *pdf = 0;
            } else {
                *pdf = 1.0 / (2 * math::PI * math::PI * sinTheta);
            }

            *visibilityTester = VisibilityTester(eye, Interaction(
                    eye.point + (*wi) * Float(2. * _worldRadius),
                    (*wi), -(*wi), _mediumBoundary));
            // TODO 暂时不考虑对纹理做重要性采样
            return sampleTexture(uv);
        }

        Float EnvironmentLight::pdfLi(const Interaction &eye, const Vector3F &dir) {
            Vector3F wi = NORMALIZE(_worldToLight->transformVector(dir));
            Float theta = math::dirToTheta(wi);
            Float phi = math::dirToPhi(wi);

            Float u = phi * math::INV_2PI;
            Float v = theta * math::INV_PI;
            // TODO 暂时不考虑对纹理做重要性采样

            Float sinTheta = std::sin(theta);
            if (sinTheta == 0) {
                return 0;
            } else {
                return 1.0 / (2 * math::PI * math::PI * sinTheta);
            }
        }

        Spectrum EnvironmentLight::sampleLe(Ray *ray, Vector3F *normal,
                                            Float *pdfPos, Float *pdfDir,
                                            Sampler *sampler) {
            Point2F uv = sampler->sample2D();
            Float theta = uv.y * math::PI;
            Float phi = uv.x * math::PI * 2.0;

            Float sinTheta = std::sin(theta);
            Float cosTheta = std::cos(theta);
            Float cosPhi = std::cos(phi);
            Float sinPhi = std::sin(phi);

            Vector3F dir = -_lightToWorld->transformVector(Vector3F(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta));
            *normal = NORMALIZE(dir);

            // Sample light pos
            Vector3F tanY = -(*normal);
            Vector3F tanX, tanZ;
            math::tangentSpace(tanY, &tanX, &tanZ);

            Vector2F diskSample = math::sampling::diskUniformSampling(sampler);
            Point3F posSample = _worldCenter + (diskSample.x * tanX + diskSample.y * tanZ) * _worldRadius;
            posSample = posSample + tanY * _worldRadius;

            (*pdfPos) = 1.0 / (math::PI * _worldRadius * _worldRadius);
            // TODO 暂时不考虑对纹理做重要性采样
            (*pdfDir) = sinTheta == 0 ? 0 : 1 / (2.0 * math::PI * math::PI * sinTheta);
            (*ray) = Ray(posSample, dir, _mediumBoundary.inside());
            return sampleTexture(uv);
        }

        void EnvironmentLight::pdfLe(const Ray &ray, const Vector3F &normal, Float *pdfPos, Float *pdfDir) const {
            Vector3F dir = -_worldToLight->transformVector(ray.getDirection());
            Float theta = math::dirToTheta(dir);
            Float phi = math::dirToPhi(dir);
            Float sinTheta = std::sin(theta);
            // TODO 暂时不考虑对纹理做重要性采样
            (*pdfDir) = 1.0 / (2 * math::PI * math::PI * sinTheta);
            (*pdfPos) = 1.0 / (math::PI * _worldRadius * _worldRadius);
        }

        void EnvironmentLight::worldBound(const Scene::Ptr scene) {
            const AABB &bound = scene->getWorld()->boundingBox();
            _worldRadius = 0.5 * LENGTH(bound.max() - bound.min());
            _worldCenter = (bound.max() + bound.min()) / Float(2.0);
        }

        EnvironmentLight::~EnvironmentLight() {
            if (_texture != nullptr) {
                free(_texture);
            }
        }

        Spectrum EnvironmentLight::sampleTexture(Point2F uv) const {
            int wOffset, hOffset;
            wOffset = uv[0] * _width;
            hOffset = uv[1] * _height;

            if (wOffset < 0 || wOffset >= _width
                || hOffset < 0 || hOffset >= _height) {
                return 0;
            }

            // flip
            hOffset = _height - (hOffset + 1);
            int offset = (hOffset * _width + wOffset) * _channel;

            Spectrum ret(0);
            for (int ch = 0; ch < _channel && ch < SPECTRUM_CHANNEL; ch++) {
                ret[ch] = Float(_texture[offset + ch]) / 255.0 * _intensity;
            }
            return ret;
        }
    }
}