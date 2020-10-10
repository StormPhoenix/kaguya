//
// Created by Storm Phoenix on 2020/10/9.
//

#ifndef KAGUYA_OBJECTHITPDF_H
#define KAGUYA_OBJECTHITPDF_H

#include <kaguya/scene/ObjectSampler.h>
#include <kaguya/tracer/Ray.h>

#include <memory>

namespace kaguya {
    namespace math {

        using kaguya::scene::ObjectSampler;
        using kaguya::tracer::Ray;

        class ObjectHitPdf {
        public:
            ObjectHitPdf(std::shared_ptr<ObjectSampler> objectSampler);

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
            virtual double pdf(const Ray &sampleRay);

        protected:
            std::shared_ptr<ObjectSampler> _objectSampler = nullptr;
        };

    }
}

#endif //KAGUYA_OBJECTHITPDF_H
