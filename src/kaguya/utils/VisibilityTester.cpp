//
// Created by Storm Phoenix on 2020/11/13.
//

#include <kaguya/sampler/Sampler.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/core/medium/Medium.h>
#include <kaguya/utils/VisibilityTester.h>

namespace RENDER_NAMESPACE {
    namespace utils {
        using sampler::Sampler;
        using core::medium::Medium;

        VisibilityTester::VisibilityTester(const core::Interaction start, const core::Interaction end) :
                _start(start), _end(end) {
        }

        bool VisibilityTester::isVisible(std::shared_ptr<Scene> scene) const {
            Ray ray = _start.sendRayTo(_end);
            while (true) {
                SurfaceInteraction si;
                bool foundIntersection = scene->intersect(ray, si);
                if (foundIntersection && !si.getMaterial().nullable()) {
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
                if (foundIntersection && !si.getMaterial().nullable()) {
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