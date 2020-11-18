//
// Created by Storm Phoenix on 2020/10/13.
//

#include <kaguya/scene/ShapeSampler.h>

namespace kaguya {
    namespace scene {

        double ShapeSampler::surfacePointPdf(SurfaceInteraction &point) {
            return 1 / area();
        }

        SurfaceInteraction ShapeSampler::sampleRayIntersection(const Interaction &eye, random::Sampler1D *sampler1D) {
            return sampleSurfacePoint(sampler1D);
        }

        double ShapeSampler::rayPdf(const Interaction &eye, const Vector3 &dir) {
            // 构建射线
            Ray ray = Ray(eye.getPoint(), dir);
            // 判断交点
            SurfaceInteraction intersection;
            if (insect(ray, intersection, 0.001, infinity)) {
                // 将交点处 rayPdf 转化为 w_i surfacePointPdf
                double distance = LENGTH(intersection.getPoint() - ray.getOrigin());
                double cosine = abs(DOT(dir, intersection.getNormal()));
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