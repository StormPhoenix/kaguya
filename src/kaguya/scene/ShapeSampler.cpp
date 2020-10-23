//
// Created by Storm Phoenix on 2020/10/13.
//

#include <kaguya/scene/ShapeSampler.h>

namespace kaguya {
    namespace scene {

        double ShapeSampler::surfacePointPdf(Interaction &point) {
            return 1 / area();
        }

        Interaction ShapeSampler::sampleRayIntersection(const Interaction &eye) {
            return sampleSurfacePoint();
        }

        double ShapeSampler::rayPdf(const Interaction &eye, const Vector3 &dir) {
            // 构建射线
            Ray ray = Ray(eye.point, dir);
            // 判断交点
            Interaction intersection;
            if (insect(ray, intersection, 0.001, infinity)) {
                // 将交点处 rayPdf 转化为 w_i surfacePointPdf
                double distance = LENGTH(intersection.point - ray.getOrigin());
                double cosine = abs(DOT(dir, intersection.normal));
                if (std::abs(cosine - 0) < EPSILON) {
                    return 0;
                }
                return pow(distance, 2) / (cosine * area());
            } else {
                return 0;
            }
        }

    }
}