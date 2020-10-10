//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/tracer/Ray.h>
#include <kaguya/math/LightHitPdf.h>

namespace kaguya {
    namespace math {

        using kaguya::tracer::Ray;
        using kaguya::scene::HitRecord;

        double LightHitPdf::pdf(const Vector3 &point, const Vector3 &rayDir, Light *light) {
            Ray ray(point, NORMALIZE(rayDir));
            HitRecord hitRecord;
            if (light->hit(ray, hitRecord, 0.00001, infinity)) {
                Vector3 hitPoint = hitRecord.point;
                double cosine = DOT(NORMALIZE(hitRecord.normal), NORMALIZE(hitRecord.direction));
                double dist = LENGTH(point - hitPoint);
                return light->samplePointPdf(hitPoint) * pow(dist, 2) / abs(cosine);
            } else {
                // 未击中
                return 0;
            }
        }

        bool LightHitPdf::sample(const Vector3 &point, Vector3 &sampleRayDir, double &samplePdf, Light *light) {
            // 光源采样点密度
            double sampleLightPdf;
            Vector3 sampleLightNormal;
            Vector3 sampleLightPoint = light->samplePoint(sampleLightPdf, sampleLightNormal);
            // 光源采样点到 point 方向
            Vector3 lightToPointDir = NORMALIZE(point - sampleLightPoint);

            // TODO 如果要修改成对任意 Object 做 hit，则不需要判断是否背光的问题。背光问题应该由 class Light 来解决
            double cosine = DOT(sampleLightNormal, lightToPointDir);
//            if (cosine > 0) {
                double dist = LENGTH(point - sampleLightPoint);
                samplePdf = sampleLightPdf * pow(dist, 2) / abs(cosine);
                sampleRayDir = -lightToPointDir;
                return true;
//            } else {
//                 背光处
//                return false;
//            }
        }
    }
}