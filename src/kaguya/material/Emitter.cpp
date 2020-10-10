//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/material/Emitter.h>
#include <kaguya/material/ConstantTexture.h>

namespace kaguya {
    namespace material {

        Emitter::Emitter() : Emitter(std::make_shared<ConstantTexture>(Vector3(1.0, 1.0, 1.0))) {}

        Emitter::Emitter(std::shared_ptr<Texture> albedo) : _albedo(albedo) {}

        bool Emitter::scatter(const Ray &ray, const HitRecord &hitRecord, Ray &scatteredRay, double &pdf) {
            return false;
        }

        Vector3 Emitter::brdf(const HitRecord &hitRecord, const Vector3 &scatterDirection) {
            return {0.0f, 0.0f, 0.0f};
        }

        double Emitter::scatterPDF(const Ray &hitRay, const HitRecord &hitRecord, const Ray &scatterRay) {
            return 0;
        }

        Vector3 Emitter::emitted(double u, double v) {
            return _albedo->sample(u, v);
        }
    }
}
