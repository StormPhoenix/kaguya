//
// Created by Storm Phoenix on 2020/10/9.
//

#include <kaguya/scene/meta/Light.h>

namespace kaguya {
    namespace scene {

        Light::Light(std::shared_ptr<Emitter> emitter, std::shared_ptr<ShapeSampler> objectSampler)
                : _emitter(emitter), _objectSampler(objectSampler) {
            assert(_emitter != nullptr && _objectSampler != nullptr);
        }

        bool Light::insect(const kaguya::tracer::Ray &ray, kaguya::scene::Interaction &hitRecord,
                           double stepMin, double stepMax) {
            bool isHit = _objectSampler->insect(ray, hitRecord, stepMin, stepMax);
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

        double Light::area() {
            return _objectSampler->area();
        }

        Interaction Light::sample() {
            return _objectSampler->sample();
        }

        double Light::pdf(Interaction &point) {
            return _objectSampler->pdf(point);
        }

        Interaction Light::sample(const Interaction &eye) {
            return _objectSampler->sample(eye);
        }

        double Light::pdf(const Interaction &eye, const Vector3 &dir) {
            return _objectSampler->pdf(eye, dir);
        }

        void Light::sampleRay(const Vector3 &point, Ray &sampleRay) {
            Interaction eye;
            eye.point = point;
            Interaction intersection = sample(eye);
            sampleRay.setOrigin(point);
            sampleRay.setDirection(NORMALIZE(intersection.point - point));
        }

        double Light::rayPdf(const Ray &sampleRay) {
            Interaction eye;
            eye.point = sampleRay.getOrigin();
            return pdf(eye, NORMALIZE(sampleRay.getDirection()));
        }
    }
}