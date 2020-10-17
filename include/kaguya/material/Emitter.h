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

            virtual bool scatter(const Ray &ray, const Interaction &hitRecord, Ray &scatteredRay, double &pdf) override;

            virtual double scatterPDF(const Ray &hitRay, const Interaction &hitRecord, const Ray &scatterRay) override;

            virtual std::shared_ptr<BSDF> bsdf(Interaction &insect) override;

            /**
             * TODO delete
             * @return
             */
            virtual bool isLight() override {
                return true;
            }

            virtual Spectrum emitted(double u, double v) override;

        protected:
            std::shared_ptr<Texture> _albedo = nullptr;
        };

    }
}

#endif //KAGUYA_EMITTER_H
