//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_DIFFUSELIGHT_H
#define KAGUYA_DIFFUSELIGHT_H

#include <kaguya/material/Texture.h>
#include <kaguya/material/Material.h>
#include <memory>

namespace kaguya {
    namespace material {

        class DiffuseLight : public Material {
        public:
            DiffuseLight(std::shared_ptr<Texture> light);

            bool scatter(const Ray &ray, const HitRecord &hitRecord, Ray &scatteredRay, double pdf) override;

            double brdf(const HitRecord &hitRecord);

            Vector3 emitted(double u, double v);

        private:
            std::shared_ptr<Texture> _light;
        };

    }
}

#endif //KAGUYA_DIFFUSELIGHT_H
