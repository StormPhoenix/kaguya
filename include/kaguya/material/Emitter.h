//
// Created by Storm Phoenix on 2020/10/8.
//

#ifndef KAGUYA_EMITTER_H
#define KAGUYA_EMITTER_H

#include <kaguya/material/Material.h>
#include <kaguya/material/Texture.h>

namespace kaguya {
    namespace material {

        class Emitter : public Material {
        public:
            Emitter();

            Emitter(std::shared_ptr<Texture> albedo);

            virtual bool scatter(const Ray &ray, const HitRecord &hitRecord, Ray &scatteredRay, double &pdf) override;

            virtual Vector3 brdf(const HitRecord &hitRecord, const Vector3 &scatterDirection) override;

            virtual double scatterPDF(const Ray &hitRay, const HitRecord &hitRecord, const Ray &scatterRay) override;

            /**
             * TODO delete
             * @return
             */
            virtual bool isLight() override {
                return true;
            }

            virtual Vector3 emitted(double u, double v) override;

        protected:
            std::shared_ptr<Texture> _albedo = nullptr;
        };

    }
}

#endif //KAGUYA_EMITTER_H
