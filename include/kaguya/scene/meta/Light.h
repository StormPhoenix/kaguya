//
// Created by Storm Phoenix on 2020/10/8.
//

#ifndef KAGUYA_LIGHT_H
#define KAGUYA_LIGHT_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/scene/ShapeSampler.h>
#include <kaguya/material/Emitter.h>

namespace kaguya {
    namespace scene {

        using kaguya::material::Emitter;
        using kaguya::core::Spectrum;

        class Light : public ShapeSampler {
        public:
            Light(std::shared_ptr<Emitter> emitter,
                  std::shared_ptr<ShapeSampler> objectSampler);

            virtual double area() override;

            virtual Interaction sample() override;

            virtual double pdf(Interaction &point) override;

            virtual Interaction sample(const Interaction &eye) override;

            virtual double pdf(const Interaction &eye, const Vector3 &dir) override;

            virtual bool insect(const Ray &ray, Interaction &hitRecord, double stepMin, double stepMax) override;

            virtual const AABB &boundingBox() const override;

            virtual const long long getId() const override;

            virtual void setId(long long id) override;

            /**
             * 从 point 出发，向 Object 发射采样射线
             * @param point
             * @param sampleRay 计算得到的采样射线
             * @return
             */
            virtual bool sampleRay(const Vector3 &point, Ray &sampleRay);

            /**
             * 计算采样射线的 PDF
             * @param sampleRay 被计算的采样射线
             * @return 采样射线的 PDF
             */
            virtual double rayPdf(const Ray &sampleRay);

            virtual Spectrum luminance(double u, double v);

        protected:
            /**
             * 发光材质
             */
            std::shared_ptr<Emitter> _emitter = nullptr;
            std::shared_ptr<ShapeSampler> _objectSampler = nullptr;
        };

    }
}

#endif //KAGUYA_LIGHT_H
