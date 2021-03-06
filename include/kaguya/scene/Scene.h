//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SCENE_H
#define KAGUYA_SCENE_H

#include <kaguya/core/Interaction.h>
#include <kaguya/scene/meta/Shape.h>
#include <kaguya/scene/Aggregation.h>
#include <kaguya/tracer/Camera.h>
#include <memory>

#define TEST_SCENE
#ifdef TEST_SCENE

#include <kaguya/core/medium/Medium.h>
#include <kaguya/core/medium/IsotropicMedium.h>

using namespace kaguya::core::medium;

#include <kaguya/material/Material.h>

using namespace kaguya::material;

#include <kaguya/core/light/Light.h>
#include <kaguya/core/light/AreaLight.h>
#include <kaguya/core/light/DiffuseAreaLight.h>
#include <kaguya/core/light/PointLight.h>
#include <kaguya/core/light/SpotLight.h>

using namespace kaguya::core;

#include <kaguya/core/spectrum/Spectrum.hpp>

using kaguya::core::Spectrum;
const int MODEL_SCALE = 5;

// smoke data
const int gridz = 192;
const int gridy = 256;
const int gridx = 192;

const std::string path = "./resource/volume/density_big_0084.pbrt";

#endif

#include <vector>

namespace kaguya {
    namespace core {
        class Light;
    }
}

namespace kaguya {
    namespace scene {

        using kaguya::tracer::Camera;
        using kaguya::core::Light;
        using kaguya::core::SurfaceInteraction;

        /**
         * 保存场景数据
         */
        class Scene {
        public:

#ifdef TEST_SCENE

            static Float *testSmokeData();

            static std::shared_ptr<Medium> testAirMedium();

            static std::shared_ptr<Medium> testSmokeMedium() {
                return std::make_shared<IsotropicMedium>(0.1, 1.2, 0);
            }

            static std::vector<std::shared_ptr<Geometry>>
            testLeftWall(const std::shared_ptr<Material> material,
                         const std::shared_ptr<Medium> insideMedium,
                         const std::shared_ptr<Medium> outsideMedium);

            static std::vector<std::shared_ptr<Geometry>>
            testRightWall(const std::shared_ptr<Material> material,
                          const std::shared_ptr<Medium> insideMedium,
                          const std::shared_ptr<Medium> outsideMedium);

            static std::vector<std::shared_ptr<Geometry>>
            testTopAreaLight(const Spectrum spectrum, const std::shared_ptr<Medium> medium,
                             std::vector<std::shared_ptr<Light>> &lights, const std::shared_ptr<Material> material);

            static std::vector<std::shared_ptr<Geometry>> testBottomWall(const std::shared_ptr<Material> material,
                                                                         const std::shared_ptr<Medium> insideMedium,
                                                                         const std::shared_ptr<Medium> outsideMedium);

            static std::vector<std::shared_ptr<Geometry>> testTopWall(const std::shared_ptr<Material> material,
                                                                      const std::shared_ptr<Medium> insideMedium,
                                                                      const std::shared_ptr<Medium> outsideMedium);

            static std::vector<std::shared_ptr<Geometry>> testFrontWall(const std::shared_ptr<Material> material,
                                                                        const std::shared_ptr<Medium> insideMedium,
                                                                        const std::shared_ptr<Medium> outsideMedium);

            static std::shared_ptr<Aggregation> testBunny(const std::shared_ptr<Material> material,
                                                          const std::shared_ptr<Medium> inside = nullptr,
                                                          const std::shared_ptr<Medium> outside = nullptr,
                                                          const std::shared_ptr<AreaLight> areaLight = nullptr);

            static std::shared_ptr<AreaLight> testDiffuseAreaLight(const Spectrum &spectrum,
                                                                   const std::shared_ptr<Geometry> geometry,
                                                                   const std::shared_ptr<Medium> inside,
                                                                   const std::shared_ptr<Medium> outside,
                                                                   bool singleSide);

            static std::shared_ptr<Scene> sceneSmoke();

            static std::shared_ptr<Scene> cornelBoxWater();

            static std::shared_ptr<Scene> sceneLightThroughAir();

#endif

            /**
             * 构建 Cornel box，加载 bunny 模型
             */
            static std::shared_ptr<Scene> sceneBunnyWithAreaLight();

            /**
             * 构建 Cornel box，加载 bunny 模型，次表面散射材质
             */
            static std::shared_ptr<Scene> sceneBunnySubsurfaceWithAreaLight();


            /**
             * 构建 bunny，采用点光源
             */
            static std::shared_ptr<Scene> sceneBunnyWithPointLight();

            /**
             * 构建 two spheres，用区域光源
             */
            static std::shared_ptr<Scene> sceneTwoSpheresWithAreaLight();

            /**
             * 测试 PointLight
             */
            static std::shared_ptr<Scene> sceneTwoSpheresWithPointLight();

            /**
             * 测试 SpotLight
             */
            static std::shared_ptr<Scene> sceneTwoSpheresWithSpotLight();

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

            std::shared_ptr<Camera> getCamera() {
                return _camera;
            }

            const std::vector<std::shared_ptr<Light>> getLights() const {
                return _lights;
            }

            const std::string getName() const {
                return _sceneName;
            }

            // TODO 增加多个光源选项
//            const std::vector<std::shared_ptr<DiffuseLight>> &getLights() {
//                return _lights;
//            }

        protected:
            // scene name
            std::string _sceneName;
            // scene objects
            std::shared_ptr<Intersectable> _world = nullptr;
            // camera
            std::shared_ptr<Camera> _camera = nullptr;
            // TODO 单个光源 用于测试
//            std::shared_ptr<Light> _light = nullptr;
            // TODO 增加多个光源选项
            std::vector<std::shared_ptr<Light>> _lights;
        };

    }
}

#endif //KAGUYA_SCENE_H
