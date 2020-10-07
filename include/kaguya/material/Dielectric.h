//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_DIELECTRIC_H
#define KAGUYA_DIELECTRIC_H

#include <kaguya/material/Material.h>

namespace kaguya {
    namespace material {

        class Dielectric : public Material {
        public:
            Dielectric(double refractionFactor);

            bool scatter(const Ray &ray, const HitRecord &hitRecord, Ray &scatteredRay, double pdf) override;

            double brdf(const HitRecord &hitRecord);

        private:
            double _refractionFactor;
        };

    }
}

#endif //KAGUYA_DIELECTRIC_H
