//
// Created by Storm Phoenix on 2020/10/9.
//

#include <kaguya/math/ObjectHitPdf.h>
#include <kaguya/scene/Hittable.h>

namespace kaguya {
    namespace math {
        using kaguya::scene::HitRecord;

        ObjectHitPdf::ObjectHitPdf(std::shared_ptr<ObjectSampler> objectSampler) : _objectSampler(objectSampler) {}

        void ObjectHitPdf::sampleRay(const Vector3 &point, kaguya::tracer::Ray &sampleRay, double &samplePdf) {
            assert(_objectSampler != nullptr);

            // 采样点的 PDF
            double samplePointPdf;
            // 采样点的法线
            Vector3 samplePointNormal;
            // 采样点
            Vector3 samplePoint;
            // 在 objectSampler 上随机采样一个点
            samplePoint = _objectSampler->samplePoint(samplePointPdf, samplePointNormal);

            // 射线方向
            Vector3 sampleToPointDir = NORMALIZE(point - samplePoint);
            // 射线方向与法线余弦
            double cosine = DOT(sampleToPointDir, NORMALIZE(samplePointNormal));
            // 采样点到点的距离
            double dist = LENGTH(point - samplePoint);
            // 计算采样射线 PDF
            samplePdf = samplePointPdf * pow(dist, 2) / abs(cosine);
            // 构造采样射线
            sampleRay = {point, -sampleToPointDir};
        }

        double ObjectHitPdf::pdf(const kaguya::tracer::Ray &sampleRay) {
            assert(_objectSampler != nullptr);

            HitRecord hitRecord;
            if (_objectSampler->hit(sampleRay, hitRecord, 0.000001, infinity)) {
                // 获取击中点
                Vector3 hitPoint = hitRecord.point;
                // 计算击中点的 PDF
                double samplePointPdf = _objectSampler->samplePointPdf(hitPoint);
                // 计算射线 PDF，与 sampleRay 中的计算方法类似
                double cosine = abs(DOT(NORMALIZE(sampleRay.getDirection()), NORMALIZE(hitRecord.normal)));
                // 采样点到点的距离
                double dist = LENGTH(hitPoint - sampleRay.getOrigin());
                // 计算采样射线 PDF
                return samplePointPdf * pow(dist, 2) / cosine;
            } else {
                // 未击中被采样物，返回 0
                return 0;
            }
        }
    }
}