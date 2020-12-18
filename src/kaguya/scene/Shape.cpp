//
// Created by Storm Phoenix on 2020/12/18.
//

#include <kaguya/scene/Shape.h>

namespace kaguya {
    namespace scene {

        double Shape::surfacePointPdf(const SurfaceInteraction &si) const {
            return 1.0 / area();
        }

        SurfaceInteraction Shape::sampleSurfaceInteraction(const Interaction &eye, const Sampler1D *sampler1D) const {
            return sampleSurfacePoint(sampler1D);
        }

        double Shape::surfaceInteractionPdf(const Interaction &eye, const Vector3 &dir) const {
            // build a ray to test interaction
            Ray ray = eye.sendRay(dir);
            SurfaceInteraction si;
            bool foundIntersection = insect(ray, si, ray.getMinStep(), ray.getStep());
            if (foundIntersection) {
                // convert density to pdf
                double distance = LENGTH(si.getPoint() - ray.getOrigin());
                double cosine = ABS_DOT(dir, si.getNormal());
                if (std::abs(cosine - 0) < EPSILON) {
                    return 0;
                }
                return pow(distance, 2) / (cosine * area());
            } else {
                return 0;
            }
        }

        const long long Shape::getId() const {
            return _id;
        }

        void Shape::setId(long long id) {
            _id = id;
        }
    }
}