//
// Created by Storm Phoenix on 2020/11/13.
//

#include <kaguya/core/medium/Medium.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/utils/VisibilityTester.h>

namespace kaguya {
    namespace utils {

        // TODO Move namespace
        using kaguya::core::medium::Medium;

        VisibilityTester::VisibilityTester(const kaguya::core::Interaction start, const kaguya::core::Interaction end) :
                _start(start), _end(end) {
        }

        bool VisibilityTester::isVisible(std::shared_ptr<Scene> scene) const {
            Ray ray = _start.sendRayTo(_end);
            while (true) {
                SurfaceInteraction si;
                bool foundIntersection = scene->intersect(ray, si);
                if (foundIntersection && si.getMaterial() != nullptr) {
                    return false;
                }

                if (!foundIntersection) {
                    return true;
                } else {
                    ray = si.sendRayTo(_end);
                }
            }
        }

        core::Spectrum VisibilityTester::transmittance(std::shared_ptr<Scene> scene, Sampler *sampler) const {
            Ray ray = _start.sendRayTo(_end);
            core::Spectrum tr(1.0);
            while (true) {
                SurfaceInteraction si;
                bool foundIntersection = scene->intersect(ray, si);
                // check whether interaction is between _start and _end
                if (foundIntersection && si.getMaterial() != nullptr) {
                    // occluded
                    return core::Spectrum(0);
                }

                if (ray.getMedium() != nullptr) {
                    tr *= ray.getMedium()->transmittance(ray, sampler);
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