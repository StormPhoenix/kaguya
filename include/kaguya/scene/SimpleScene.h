//
// Created by Graphics on 2021/5/30.
//

#ifndef TUNAN_SIMPLESCENE_H
#define TUNAN_SIMPLESCENE_H

#include <kaguya/core/Interaction.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/core/medium/Medium.h>
#include <kaguya/core/medium/IsotropicMedium.h>
#include <kaguya/material/Material.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/scene/meta/Shape.h>
#include <kaguya/core/Transform.h>

#include <vector>
#include <memory>

using namespace kaguya::core;
using namespace kaguya::core::medium;
using namespace kaguya::material;

using kaguya::core::Spectrum;

const int MODEL_SCALE = 5;

namespace RENDER_NAMESPACE {
    namespace scene {
        using kaguya::core::SurfaceInteraction;

        /**
         * 保存场景数据
         */
        class SimpleScene : public Scene {
        public:
            typedef std::shared_ptr<SimpleScene> Ptr;

            /*
            static Float *testSmokeData();

            static std::shared_ptr<Medium> testAirMedium();

            static std::shared_ptr<Medium> testSmokeMedium() {
                return std::make_shared<IsotropicMedium>(0.1, 1.2, 0);
            }
             */

            static std::vector<std::shared_ptr<Geometry>>
            testLeftWall(const Material material,
                         const std::shared_ptr<Medium> insideMedium,
                         const std::shared_ptr<Medium> outsideMedium,
                         MemoryAllocator &allocator);

            static std::vector<std::shared_ptr<Geometry>>
            testRightWall(const Material material,
                          const std::shared_ptr<Medium> insideMedium,
                          const std::shared_ptr<Medium> outsideMedium);

            static std::vector<std::shared_ptr<Geometry>> testBottomWall(const Material material,
                                                                         const std::shared_ptr<Medium> insideMedium,
                                                                         const std::shared_ptr<Medium> outsideMedium);

            static std::vector<std::shared_ptr<Geometry>> testTopWall(const Material material,
                                                                      const std::shared_ptr<Medium> insideMedium,
                                                                      const std::shared_ptr<Medium> outsideMedium);

            static std::vector<std::shared_ptr<Geometry>> testFrontWall(const Material material,
                                                                        const std::shared_ptr<Medium> insideMedium,
                                                                        const std::shared_ptr<Medium> outsideMedium);

            /*
            static std::vector<std::shared_ptr<Geometry>> testBottomPlane(const Material material,
                                                                          const std::shared_ptr<Medium> insideMedium,
                                                                          const std::shared_ptr<Medium> outsideMedium);



            static std::shared_ptr<Aggregation> testSubsurfaceBunny(const Material material,
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
                             std::vector<std::shared_ptr<Light>> &lights, const Material material);

//            static std::shared_ptr<SimpleScene> sceneBunnySubsurfaceWithAreaLight(MemoryAllocator &allocator);

            static std::shared_ptr<SimpleScene> innerSceneBunnyWithPointLight(MemoryAllocator &allocator);

            static std::shared_ptr<SimpleScene> innerSceneWithAreaLight(MemoryAllocator &allocator);

            static std::shared_ptr<SimpleScene> innerSceneWithPointLight(MemoryAllocator &allocator);

            /**
             * Intersect with scene，record the interaction
             * @param ray
             * @param si
             * @return
             */
            bool intersect(Ray &ray, SurfaceInteraction &si) const override;

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
                                     Sampler *sampler) const override;

            std::shared_ptr<Intersectable> getWorld() {
                return _world;
            }

            AABB getWorldBox() const override {
                return _world->boundingBox();
            }

            std::shared_ptr<Camera> getCamera() override;

            const std::vector<std::shared_ptr<Light>> &getLights() const;

            const std::vector<std::shared_ptr<EnvironmentLight>> &getEnvironmentLights() const;

            void addLight(std::shared_ptr<Light> light);

            void addEnvironmentLight(std::shared_ptr<EnvironmentLight> envLight);

            void setCamera(std::shared_ptr<Camera> camera);

            void setWorld(Intersectable::Ptr world) {
                _world = world;
            }

        protected:
            // scene objects
            std::shared_ptr<Intersectable> _world = nullptr;
            // camera
            std::shared_ptr<Camera> _camera = nullptr;
            std::vector<std::shared_ptr<Light>> _lights;
            std::vector<std::shared_ptr<EnvironmentLight>> _envLights;
        };

    }
}

#endif //TUNAN_SIMPLESCENE_H
