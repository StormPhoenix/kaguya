//
// Created by Storm Phoenix on 2020/10/9.
//

#include <kaguya/scene/meta/Light.h>
#include <kaguya/math/ObjectHitPdf.h>

namespace kaguya {
    namespace scene {

        using kaguya::math::ObjectHitPdf;

        Light::Light(std::shared_ptr<Emitter> emitter, std::shared_ptr<ObjectSampler> objectSampler)
                : _emitter(emitter), _objectSampler(objectSampler) {
            assert(_emitter != nullptr && _objectSampler != nullptr);
            _objectHitPdf = std::make_shared<ObjectHitPdf>(_objectSampler);
        }

        Vector3 Light::samplePoint(double &pdf, Vector3 &normal) {
            return _objectSampler->samplePoint(pdf, normal);
        }

        double Light::samplePointPdf(Vector3 &point) {
            return _objectSampler->samplePointPdf(point);
        }

        bool Light::hit(const kaguya::tracer::Ray &ray, kaguya::scene::HitRecord &hitRecord,
                        double stepMin, double stepMax) {
            bool isHit = _objectSampler->hit(ray, hitRecord, stepMin, stepMax);
            if (isHit) {
                // 替换材质为发光体
                hitRecord.material = _emitter;
            }
            return isHit;
        }

        const AABB &Light::boundingBox() const {
            return _objectSampler->boundingBox();
        }

        const long long Light::getId() const {
            return _objectSampler->getId();
        }

        void Light::setId(long long id) {
            _objectSampler->setId(id);
        }

        void Light::sampleRay(const Vector3 &point, kaguya::tracer::Ray &sampleRay, double &samplePdf) {
            _objectHitPdf->sampleRay(point, sampleRay, samplePdf);
        }

        double Light::rayPdf(const kaguya::tracer::Ray &sampleRay) {
            return _objectHitPdf->pdf(sampleRay);
        }
    }
}