//
// Created by Storm Phoenix on 2020/10/8.
//

#ifndef KAGUYA_LIGHT_H
#define KAGUYA_LIGHT_H

#include <kaguya/scene/ObjectSampler.h>
#include <kaguya/math/ObjectHitPdf.h>
#include <kaguya/material/Emitter.h>

namespace kaguya {
    namespace scene {

        using kaguya::material::Emitter;
        using kaguya::math::ObjectHitPdf;

        class Light : public ObjectSampler {
        public:
            Light(std::shared_ptr<Emitter> emitter,
                  std::shared_ptr<ObjectSampler> objectSampler);

            virtual Vector3 samplePoint(double &pdf, Vector3 &normal) override;

            virtual double samplePointPdf(Vector3 &point) override;

            virtual bool hit(const Ray &ray, HitRecord &hitRecord, double stepMin, double stepMax) override;

            virtual const AABB &boundingBox() const override;

            virtual const long long getId() const override;

            virtual void setId(long long id) override;

            /**
             * 从 point 出发，向 Object 发射采样射线
             * @param point
             * @param sampleRay 计算得到的采样射线
             * @param samplePdf 计算得得到的采样射线的 PDF
             * @return
             */
            virtual void sampleRay(const Vector3 &point, Ray &sampleRay, double &samplePdf);

            /**
             * 计算采样射线的 PDF
             * @param sampleRay 被计算的采样射线
             * @return 采样射线的 PDF
             */
            virtual double rayPdf(const Ray &sampleRay);

        protected:
            std::shared_ptr<Emitter> _emitter = nullptr;
            std::shared_ptr<ObjectSampler> _objectSampler = nullptr;
            std::shared_ptr<ObjectHitPdf> _objectHitPdf = nullptr;
        };

    }
}

#endif //KAGUYA_LIGHT_H
