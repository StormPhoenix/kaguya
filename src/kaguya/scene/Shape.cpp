//
// Created by Storm Phoenix on 2020/12/18.
//

#include <kaguya/scene/meta/Shape.h>

namespace kaguya {
    namespace scene {
        namespace meta {
            double Shape::surfacePointPdf(const SurfaceInteraction &si) const {
                return 1.0 / area();
            }

            SurfaceInteraction
            Shape::sampleSurfaceInteraction(const Interaction &eye, const Sampler *sampler1D) const {
                return sampleSurfacePoint(sampler1D);
            }

            double Shape::surfaceInteractionPdf(const Interaction &eye, const Vector3 &dir) const {
                // build a ray to test interaction
                Ray ray = eye.sendRay(dir);
                SurfaceInteraction si;
                bool foundIntersection = intersect(ray, si, ray.getMinStep(), ray.getStep());
                if (foundIntersection) {
                    // convert density to pdf
                    double distance = LENGTH(si.getPoint() - ray.getOrigin());
                    double cosine = ABS_DOT(dir, si.getNormal());
                    if (std::abs(cosine - 0) < math::EPSILON) {
                        return 0;
                    }
                    return pow(distance, 2) / (cosine * area());
                } else {
                    return 0;
                }
            }
        }
    }
}