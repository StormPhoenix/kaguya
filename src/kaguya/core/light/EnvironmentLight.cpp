//
// Created by Storm Phoenix on 2021/4/19.
//

#include <kaguya/math/Math.h>
#include <kaguya/utils/ImageReader.h>
#include <kaguya/core/light/EnvironmentLight.h>

namespace kaguya {
    namespace core {
        using namespace utils;

        EnvironmentLight::EnvironmentLight(Float intensity, std::string texturePath,
                                           const MediumBoundary &mediumBoundary,
                                           Transform::Ptr lightToWorld)
                : Light(ENVIRONMENT, mediumBoundary), _intensity(intensity), _lightToWorld(lightToWorld) {
            // Check file exists
            {
                std::ifstream in(texturePath);
                ASSERT(in.good(), texturePath + " NOT EXISTS.");
            }

            // Transform matrix
            if (_lightToWorld == nullptr) {
                _lightToWorld = std::make_shared<Transform>();
            }
            _worldToLight = _lightToWorld->inverse().ptr();
            _texture = io::readImage(texturePath.c_str(), &_width, &_height);

            std::unique_ptr<Float[]> sampleFunction(new Float[_width * _height]);
            int sampleChannel = 1;
            for (int row = 0; row < _height; row++) {
                for (int col = 0; col < _width; col++) {
                    int offset = row * _width + col;
                    // SinTheta for sampling correction
                    Float sinTheta = std::sin(Float(row) / _height * math::PI);
                    sampleFunction[offset] = _texture[offset][sampleChannel] * sinTheta;
                }
            }

            _textureDistribution.reset(new Distribution2D(sampleFunction.get(), _width, _height));
        }

        Spectrum EnvironmentLight::Le(const Ray &ray) const {
            Vector3F wi = NORMALIZE(_worldToLight->transformVector(ray.getDirection()));
            // wi.y = cosTheta
            Float theta = math::local_coord::dirToTheta(wi);
            // wi.x = sinTheta * cosPhi
            // wi.z = sinTheta * sinPhi
            // wi.z / wi.x = tanPhi
            Float phi = math::local_coord::dirToPhi(wi);
            Point2F uv = {phi * math::INV_2PI, theta * math::INV_PI};
            return sampleTexture(uv);
        }

        Spectrum EnvironmentLight::sampleLi(const Interaction &eye, Vector3F *wi,
                                            Float *pdf, Sampler *sampler,
                                            VisibilityTester *visibilityTester) {
            // Sample from environment map
            Float samplePdf = 0.;
            Point2F uv = _textureDistribution->sampleContinuous(&samplePdf, sampler);

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
                // Jacobi correction
                *pdf = samplePdf / (2 * math::PI * math::PI * sinTheta);
            }

            *visibilityTester = VisibilityTester(eye, Interaction(
                    eye.point + (*wi) * Float(2. * _worldRadius),
                    (*wi), -(*wi), _mediumBoundary));
            // TODO 暂时不考虑对纹理做重要性采样
            return sampleTexture(uv);
        }

        Float EnvironmentLight::pdfLi(const Interaction &eye, const Vector3F &dir) {
            Vector3F wi = NORMALIZE(_worldToLight->transformVector(dir));
            Float theta = math::local_coord::dirToTheta(wi);
            Float phi = math::local_coord::dirToPhi(wi);

            Float u = phi * math::INV_2PI;
            Float v = theta * math::INV_PI;
            // TODO 暂时不考虑对纹理做重要性采样

            Float sinTheta = std::sin(theta);
            if (sinTheta == 0) {
                return 0;
            } else {
                // Jacobi correction
                return (_textureDistribution->pdfContinuous(Point2F(u, v))) / (2 * math::PI * math::PI * sinTheta);
            }
        }

        Spectrum EnvironmentLight::sampleLe(Ray *ray, Vector3F *normal,
                                            Float *pdfPos, Float *pdfDir,
                                            Sampler *sampler) {
            Float samplePdf = 0;
            Point2F uv = _textureDistribution->sampleContinuous(&samplePdf, sampler);
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
            (*pdfDir) = sinTheta == 0 ? 0 : samplePdf / (2.0 * math::PI * math::PI * sinTheta);
            (*ray) = Ray(posSample, dir, _mediumBoundary.inside());
            return sampleTexture(uv);
        }

        void EnvironmentLight::pdfLe(const Ray &ray, const Vector3F &normal, Float *pdfPos, Float *pdfDir) const {
            Vector3F dir = -_worldToLight->transformVector(ray.getDirection());
            Float theta = math::local_coord::dirToTheta(dir);
            Float phi = math::local_coord::dirToPhi(dir);
            Float sinTheta = std::sin(theta);
            // TODO 暂时不考虑对纹理做重要性采样
            Point2F uv(phi * math::INV_2PI, theta * math::INV_PI);
            Float mapPdf = _textureDistribution->pdfContinuous(uv);

            (*pdfDir) = mapPdf / (2 * math::PI * math::PI * sinTheta);
            (*pdfPos) = 1.0 / (math::PI * _worldRadius * _worldRadius);
        }

        void EnvironmentLight::worldBound(const Scene::Ptr scene) {
            const AABB &bound = scene->getWorld()->boundingBox();
            _worldRadius = 0.5 * LENGTH(bound.maxPos() - bound.minPos());
            _worldCenter = (bound.maxPos() + bound.minPos()) / Float(2.0);
        }

        EnvironmentLight::~EnvironmentLight() {
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
            int offset = (hOffset * _width + wOffset);

            Spectrum ret(0);
            // TODO Adjust rgb channels
            for (int ch = 0; ch < 3 && ch < SPECTRUM_CHANNEL; ch++) {
//                ret[ch] = Float(_texture[offset][ch]) / 255.0 * _intensity;
                ret[ch] = Float(_texture[offset][ch]) * _intensity;
            }
            return ret;
        }
    }
}