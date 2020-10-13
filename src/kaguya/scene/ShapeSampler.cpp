//
// Created by Storm Phoenix on 2020/10/13.
//

#include <kaguya/scene/ShapeSampler.h>

namespace kaguya {
    namespace scene {

        double ShapeSampler::pdf(Interaction &point) {
            return 1 / area();
        }

        Interaction ShapeSampler::sample(const Interaction &eye) {
            return sample();
        }

        double ShapeSampler::pdf(const Interaction &eye, const Vector3 &dir) {
            // 构建射线
            Ray ray = Ray(eye.point, dir);
            // 判断交点
            Interaction intersection;
            if (hit(ray, intersection, 0.001, infinity)) {
                // 将交点处 pdf 转化为 w_i pdf
                double distance = LENGTH(intersection.point - ray.getOrigin());
                double cosine = abs(DOT(dir, intersection.normal));
                return pow(distance, 2) / (cosine * area());
            } else {
                return 0;
            }
        }

    }
}