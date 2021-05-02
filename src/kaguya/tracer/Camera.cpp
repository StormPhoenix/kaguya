//
// Created by Storm Phoenix on 2020/9/30.
//

#include <kaguya/Config.h>
#include <kaguya/tracer/Camera.h>

namespace kaguya {
    namespace tracer {

        Camera::Camera(Transform::Ptr cameraToWorld, Float hFov, Float nearClip, Float farClip, Medium::Ptr medium)
                : _medium(medium), _nearClip(nearClip), _farClip(farClip) {
            Float aspect = Float(Config::Camera::width) / Float(Config::Camera::height);
            _cameraToWorld = cameraToWorld;
            _worldToCamera = _cameraToWorld->inverse().ptr();

            _lensRadius = 0.000025;
            _rasterToCamera = (Transform::scale(Config::Camera::width, Config::Camera::height, 1.) *
                               Transform::scale(-0.5, 0.5 * aspect, 1.) *
                               Transform::translate(-1, 1.0 / aspect, 0) *
                               Transform::perspective(hFov, nearClip, farClip)).inverse().ptr();

            _cameraToRaster = _rasterToCamera->inverse().ptr();
            _origin = _cameraToWorld->transformPoint(Point3F(0., 0., 0.));

            Point3F filmMin = _rasterToCamera->transformPoint(Point3F(0, 0, 0));
            Point3F filmMax = _rasterToCamera->transformPoint(
                    Point3F(Config::Camera::width, Config::Camera::height, 0));
            filmMin /= filmMin.z;
            filmMax /= filmMax.z;
            _filmPlaneArea = std::abs((filmMax.x - filmMin.x) * (filmMax.y - filmMin.y));
            _lensArea = math::PI * _lensRadius * _lensRadius;
            _front = NORMALIZE(_cameraToWorld->transformPoint(Vector3F(0, 0, 1))
                               - _cameraToWorld->transformPoint(Vector3F(0, 0, 0)));
        }

        Ray Camera::sendRay(Float u, Float v) const {
            Float x = Config::Camera::width * u;
            Float y = Config::Camera::height * v;
            Vector3F dir = NORMALIZE(_rasterToCamera->transformPoint(Point3F(x, y, 0)));

            dir = NORMALIZE(_cameraToWorld->transformVector(dir));
            return Ray(_origin, dir, _medium.get());
        }

        FilmPlane *Camera::buildFilmPlane(int channel) const {
            return new FilmPlane(Config::Camera::width, Config::Camera::height, channel);
        }

        Spectrum Camera::sampleCameraRay(const Interaction &eye,
                                         Vector3F *wi, Float *pdf,
                                         Point2F *filmPosition,
                                         Sampler *const sampler,
                                         VisibilityTester *visibilityTester) const {
            // Sample from lens disk
            Vector2F diskSample = math::sampling::diskUniformSampling(sampler, _lensRadius);
            // Calculate 3D coordinator
            Vector3F lensSample = _cameraToWorld->transformPoint(Point3F(diskSample.x, diskSample.y, 0));

            // W_i direction and distance
            (*wi) = lensSample - eye.point;
            Float dist = LENGTH(*wi);
            (*wi) = NORMALIZE(*wi);
            // Normal
            Vector3F normal = _front;

            Interaction lensInter = Interaction(lensSample, *wi, normal, _medium.get());
            (*visibilityTester) = VisibilityTester(eye, lensInter);

            // Compute importance value
            (*pdf) = (dist * dist) / (_lensArea * ABS_DOT(lensInter.normal, *wi));
            return We(Ray(lensSample, -(*wi)), filmPosition);
        }

        void Camera::pdfWe(const Ray &ray, Float &pdfPos, Float &pdfDir) const {
            Vector3F rayDir = NORMALIZE(ray.getDirection());
            Float cosine = DOT(rayDir, _front);
            if (cosine <= 0) {
                pdfPos = 0.;
                pdfDir = 0.;
                return;
            }

            // Compute raster position
            Point3F pFocus = (1 / cosine) * rayDir + ray.getOrigin();
            Point3F pRaster = _cameraToRaster->transformPoint(_worldToCamera->transformPoint(pFocus));
            // Check range
            if (pRaster.x < 0 || pRaster.x >= Config::Camera::width ||
                pRaster.y < 0 || pRaster.y >= Config::Camera::height) {
                pdfPos = 0.0;
                pdfDir = 0.0;
                return;
            }

            // 计算 pdfPos pdfDir
            pdfPos = 1. / (_lensArea);
            pdfDir = 1. / (_filmPlaneArea * cosine * cosine * cosine);
            return;
        }

        Spectrum Camera::We(const Ray &ray, Point2F *const filmPosition) const {
            Vector3F rayDir = NORMALIZE(ray.getDirection());
            Float cosine = DOT(rayDir, _front);
            if (cosine <= 0) {
                return Spectrum(0.);
            }

            // Compute raster position
            Point3F pFocus = (1 / cosine) * rayDir + ray.getOrigin();
            Point3F pRaster = _cameraToRaster->transformPoint(_worldToCamera->transformPoint(pFocus));

            // Check range
            if (pRaster.x < 0 || pRaster.x >= Config::Camera::width ||
                pRaster.y < 0 || pRaster.y >= Config::Camera::height) {
                return Spectrum(0.);
            }

            if (filmPosition != nullptr) {
                (*filmPosition) = Point2F(pRaster.x, pRaster.y);
            }
            /*
             * 1) p(w) = dist^2 / (A * cosine)
             * 2) dist * cosine = _focal
             * 3) W_e = p(w) / (PI * lensRadius^2 * cosine)
             * 4) A is area of image plane
             * 5) W_e = _focal^2 / (cosine^4 * A * PI * lensRadius^2)

             * Float dist = _focal / cosine;
             * Float pW = dist * dist / (_area * cosine);
             * Float weight = pW / (PI * _lensRadius * _lensRadius * cosine);
            */
            Float weight = 1. / (_filmPlaneArea * _lensArea * std::pow(cosine, 4));
            return Spectrum(weight);
        }

    }
}