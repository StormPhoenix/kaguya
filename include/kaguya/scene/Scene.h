//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SCENE_H
#define KAGUYA_SCENE_H

#include <kaguya/common.h>
#include <kaguya/core/Interaction.h>
#include <kaguya/core/light/Light.h>
#include <kaguya/core/light/EnvironmentLight.h>
#include <kaguya/tracer/Ray.h>
#include <kaguya/sampler/Sampler.h>
#include <kaguya/scene/accumulation/AABB.h>

#include <string>

namespace RENDER_NAMESPACE {
    namespace scene {
        using acc::AABB;
        using tracer::Ray;
        using sampler::Sampler;
        using namespace core;

        class Scene {
        public:
            typedef std::shared_ptr<Scene> Ptr;

            virtual const std::vector<std::shared_ptr<EnvironmentLight>> &getEnvironmentLights() const = 0;

            virtual bool intersect(Ray &ray, SurfaceInteraction &si) const = 0;

            virtual bool intersectWithMedium(Ray &ray, SurfaceInteraction &si,
                                             core::Spectrum &transmittance, Sampler *sampler) const = 0;

            virtual const std::vector<std::shared_ptr<Light>> &getLights() const = 0;

            virtual std::shared_ptr<Camera> getCamera() = 0;

            virtual AABB getWorldBox() const = 0;

            virtual const std::string getName() const {
                return _sceneName;
            }

            virtual void setSceneName(std::string filename) {
                _sceneName = filename;
            }

        protected:

            std::string _sceneName;
        };
    }
}

#endif //KAGUYA_SCENE_H
