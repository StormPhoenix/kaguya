//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SCENE_H
#define KAGUYA_SCENE_H

#include <kaguya/core/Interaction.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/core/medium/Medium.h>
#include <kaguya/core/medium/IsotropicMedium.h>
#include <kaguya/material/Material.h>
#include <kaguya/scene/meta/Shape.h>
#include <kaguya/core/Transform.h>

#include <vector>
#include <memory>

using namespace kaguya::core;
using namespace kaguya::core::medium;
using namespace kaguya::material;

using kaguya::core::Spectrum;

const int MODEL_SCALE = 5;

namespace kaguya {
    namespace scene {
        using kaguya::core::SurfaceInteraction;

        /**
         * 保存场景数据
         */
        class Scene {
        public:
            typedef std::shared_ptr<Scene> Ptr;

            /*
            static Float *testSmokeData();

            static std::shared_ptr<Medium> testAirMedium();

            static std::shared_ptr<Medium> testSmokeMedium() {
                return std::make_shared<IsotropicMedium>(0.1, 1.2, 0);
            }
             */

            static std::vector<std::shared_ptr<Geometry>>
            testLeftWall(const std::shared_ptr<Material> material,
                         const std::shared_ptr<Medium> insideMedium,
                         const std::shared_ptr<Medium> outsideMedium);

            static std::vector<std::shared_ptr<Geometry>>
            testRightWall(const std::shared_ptr<Material> material,
                          const std::shared_ptr<Medium> insideMedium,
                          const std::shared_ptr<Medium> outsideMedium);

            static std::vector<std::shared_ptr<Geometry>> testBottomWall(const std::shared_ptr<Material> material,
                                                                         const std::shared_ptr<Medium> insideMedium,
                                                                         const std::shared_ptr<Medium> outsideMedium);

            static std::vector<std::shared_ptr<Geometry>> testTopWall(const std::shared_ptr<Material> material,
                                                                      const std::shared_ptr<Medium> insideMedium,
                                                                      const std::shared_ptr<Medium> outsideMedium);

            static std::vector<std::shared_ptr<Geometry>> testFrontWall(const std::shared_ptr<Material> material,
                                                                        const std::shared_ptr<Medium> insideMedium,
                                                                        const std::shared_ptr<Medium> outsideMedium);

            /*
            static std::vector<std::shared_ptr<Geometry>> testBottomPlane(const std::shared_ptr<Material> material,
                                                                          const std::shared_ptr<Medium> insideMedium,
                                                                          const std::shared_ptr<Medium> outsideMedium);



            static std::shared_ptr<Aggregation> testSubsurfaceBunny(const std::shared_ptr<Material> material,
                                                          const std::shared_ptr<Medium> inside = nullptr,
                                                          const std::shared_ptr<Medium> outside = nullptr,
                                                          const std::shared_ptr<AreaLight> areaLight = nullptr);

             */
            static std::shared_ptr<AreaLight> testDiffuseAreaLight(const Spectrum &spectrum,
                                                                   const std::shared_ptr<Geometry> geometry,
                                                                   const std::shared_ptr<Medium> inside,
                                                                   const std::shared_ptr<Medium> outside,
                                                                   bool singleSide);

            static std::vector<std::shared_ptr<Geometry>>
            testTopAreaLight(const Spectrum spectrum, const std::shared_ptr<Medium> medium,
                             std::vector<std::shared_ptr<Light>> &lights, const std::shared_ptr<Material> material);

//            static std::shared_ptr<Scene> sceneBunnySubsurfaceWithAreaLight();

            static std::shared_ptr<Scene> innerSceneBunnyWithPointLight();

            static std::shared_ptr<Scene> innerSceneWithAreaLight();

            static std::shared_ptr<Scene> innerSceneWithPointLight();

            /**
             * Intersect with scene，record the interaction
             * @param ray
             * @param si
             * @return
             */
            bool intersect(Ray &ray, SurfaceInteraction &si) const;

            /**
             * Intersect scene with participating medium
             *
             * intersectWithMedium() will skip the shape without material attribute
             * @param ray
             * @param si
             * @param transmittance
             * @return
             */
            bool intersectWithMedium(Ray &ray, SurfaceInteraction &si, core::Spectrum &transmittance,
                                     Sampler *sampler) const;

            std::shared_ptr<Intersectable> getWorld() {
                return _world;
            }

            std::shared_ptr<Camera> getCamera();

            const std::vector<std::shared_ptr<Light>> &getLights() const;

            const std::vector<std::shared_ptr<EnvironmentLight>> &getEnvironmentLights() const;

            void addLight(std::shared_ptr<Light> light);

            void addEnvironmentLight(std::shared_ptr<EnvironmentLight> envLight);

            const std::string getName() const {
                return _sceneName;
            }

            void setCamera(std::shared_ptr<Camera> camera);

            void setWorld(Intersectable::Ptr world) {
                _world = world;
            }

            void setSceneName(std::string filename) {
                _sceneName = filename;
            }
        protected:
            // scene name
            std::string _sceneName;
            // scene objects
            std::shared_ptr<Intersectable> _world = nullptr;
            // camera
            std::shared_ptr<Camera> _camera = nullptr;
            std::vector<std::shared_ptr<Light>> _lights;
            std::vector<std::shared_ptr<EnvironmentLight>> _envLights;
        };

    }
}

#endif //KAGUYA_SCENE_H
