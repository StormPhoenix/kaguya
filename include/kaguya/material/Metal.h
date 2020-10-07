//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_METAL_H
#define KAGUYA_METAL_H

#include <kaguya/material/Material.h>
#include <kaguya/math/Math.hpp>

namespace kaguya {
    namespace material {

        class Metal : public Material {
        public:
            Metal(const Vector3 &albedo, double fuzzy);

            bool scatter(const Ray &ray, const HitRecord &hitRecord, Ray &scatteredRay, double pdf) override;

        private:
            // 反射率
            Vector3 _albedo;
            // 毛玻璃效果系数
            double _fuzzy;
        };

    }
}

#endif //KAGUYA_METAL_H
