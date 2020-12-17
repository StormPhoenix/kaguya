//
// Created by Storm Phoenix on 2020/11/13.
//

#include <kaguya/core/medium/Medium.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/utils/VisibilityTester.h>

namespace kaguya {
    namespace utils {

        using kaguya::core::medium::Medium;

        VisibilityTester::VisibilityTester(const kaguya::core::Interaction start, const kaguya::core::Interaction end) :
                _start(start), _end(end) {
            _step = LENGTH(_end.getPoint() - _start.getPoint());
        }

        bool VisibilityTester::isVisible(Scene &scene) const {
            Ray ray(_start.getPoint(), NORMALIZE(_end.getPoint() - _start.getPoint()));
            // TODO 如何融合 Surface 和 Volume
            SurfaceInteraction interaction;
            bool isIntersect = scene.intersect(ray, interaction);

            return (!isIntersect) || (isIntersect && ray.getStep() >= (_step - EPSILON));
        }

        bool VisibilityTester::pointMatch(const Vector3 &a, const Vector3 &b) {
            return std::abs(a.x - b.x) < EPSILON &&
                   std::abs(a.y - b.y) < EPSILON &&
                   std::abs(a.z - b.z) < EPSILON;
        }

        core::Spectrum VisibilityTester::transmittance(Scene &scene) const {
            Ray ray = _start.sendRayTo(_end);
            core::Spectrum tr(1.0);
            while (true) {
                SurfaceInteraction si;
                bool foundIntersection = scene.intersect(ray, si);
                // check whether interaction is between _start and _end
                if (foundIntersection && si.getMaterial() != nullptr) {
                    // occluded
                    return core::Spectrum(0);
                }

                if (ray.getMedium() != nullptr) {
                    tr *= ray.getMedium()->transmittance(ray);
                }

                if (!foundIntersection) {
                    return tr;
                } else {
                    ray = si.sendRayTo(_end);
                }
            }
        }
    }
}