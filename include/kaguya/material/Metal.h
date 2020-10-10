//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_METAL_H
#define KAGUYA_METAL_H

#include <kaguya/material/Material.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/math/ScatterPdf.h>

namespace kaguya {
    namespace material {

        using kaguya::math::ScatterPdf;

        class Metal : public Material {
        public:
            Metal();

            Metal(const Vector3 &albedo, double fuzzy = 0);

            virtual bool scatter(const Ray &ray, const HitRecord &hitRecord, Ray &scatteredRay, double &pdf) override;

            virtual bool isSpecular() override;

            virtual Vector3 brdf(const HitRecord &hitRecord, const Vector3 &scatterDirection) override;

            virtual double scatterPDF(const Ray &hitRay, const HitRecord &hitRecord, const Ray &scatterRay) override;

        private:
            // 反射率
            Vector3 _albedo;
            // 毛玻璃效果系数
            double _fuzzy;
            // 散射分布函数
            std::shared_ptr<ScatterPdf> _pdf = nullptr;
        };

    }
}

#endif //KAGUYA_METAL_H
