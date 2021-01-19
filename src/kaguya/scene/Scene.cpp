//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/core/light/Light.h>
#include <kaguya/core/light/AreaLight.h>
#include <kaguya/core/light/DiffuseAreaLight.h>
#include <kaguya/core/light/PointLight.h>
#include <kaguya/core/light/SpotLight.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/scene/meta/Triangle.h>
#include <kaguya/scene/Geometry.h>
#include <kaguya/Config.h>
#include <kaguya/scene/aggregation/Box.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/scene/accumulation/BVH.h>
#include <kaguya/scene/meta/Sphere.h>
#include <kaguya/scene/aggregation/TriangleMesh.h>
#include <kaguya/material/Dielectric.h>
#include <kaguya/material/Lambertian.h>
#include <kaguya/material/Metal.h>
#include <kaguya/material/ConstantTexture.h>
#include <kaguya/utils/ObjLoader.h>
#include <kaguya/core/medium/GridDensityMedium.h>

#ifdef TEST_SCENE

#include <cstring>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>

#endif

namespace kaguya {
    namespace scene {

        using namespace kaguya::material;
        using namespace kaguya::scene::acc;
        using namespace kaguya::core;


#ifdef TEST_SCENE

        Float *Scene::testSmokeData() {
            std::cout << "load smoke data ... " << std::endl;
            float density;
            float maxDensity = 0;
            long gridCount = 0;

            Float *smokeData = new Float[gridx * gridy * gridz];
            Float *p = smokeData;

            std::fstream fr(path.c_str(), std::ios::in);
            while (fr >> density) {
                (*p) = density * 15;
                maxDensity = maxDensity <= density ? density : maxDensity;
                gridCount++;
                p++;
            }
            fr.close();
            std::cout << "  max density: " << maxDensity << std::endl
                      << "  grid count: " << gridCount << std::endl
                      << "completed!" << std::endl;

            return smokeData;
        }

        std::shared_ptr<Medium> Scene::testAirMedium() {
            Spectrum sigmaA(0.01);
//                sigmaA.r(0.0011f);
//                sigmaA.g(0.0024f);
//                sigmaA.b(0.014f);
            std::shared_ptr<Medium> airMedium = std::make_shared<IsotropicMedium>(sigmaA, 0.1, 0);
            return airMedium;
        }

        std::vector<std::shared_ptr<Geometry>>
        Scene::testLeftWall(const std::shared_ptr<Material> material,
                            const std::shared_ptr<Medium> insideMedium,
                            const std::shared_ptr<Medium> outsideMedium) {

            const Vector3F a1(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE);
            const Vector3F a2(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE);
            const Vector3F a3(-0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE);
            const Vector3F a4(-0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE);

            const Normal3F n(1, 0, 0);
            const Vector2f default_uv(0);

            std::shared_ptr<meta::Shape> tri1 = std::make_shared<meta::Triangle>(a1, a3, a4,
                                                                                 n, n, n,
                                                                                 default_uv, default_uv, default_uv);
            std::shared_ptr<Geometry> gt1 = std::make_shared<Geometry>(tri1, material, insideMedium, outsideMedium,
                                                                       nullptr);

            std::shared_ptr<meta::Shape> tri2 = std::make_shared<meta::Triangle>(a1, a2, a3,
                                                                                 n, n, n,
                                                                                 default_uv, default_uv, default_uv);
            std::shared_ptr<Geometry> gt2 = std::make_shared<Geometry>(tri2, material, insideMedium, outsideMedium,
                                                                       nullptr);
            std::vector<std::shared_ptr<Geometry>> v;
            v.push_back(gt1);
            v.push_back(gt2);
            return v;
        }


        std::vector<std::shared_ptr<Geometry>>
        Scene::testRightWall(const std::shared_ptr<Material> material,
                             const std::shared_ptr<Medium> insideMedium,
                             const std::shared_ptr<Medium> outsideMedium) {
            const Vector3F a1(0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE);
            const Vector3F a2(0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE);
            const Vector3F a3(0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE);
            const Vector3F a4(0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE);

            const Normal3F n(-1, 0, 0);
            const Vector2f default_uv(0);

            std::shared_ptr<meta::Shape> tri1 = std::make_shared<meta::Triangle>(a1, a3, a4,
                                                                                 n, n, n,
                                                                                 default_uv, default_uv, default_uv);
            std::shared_ptr<Geometry> gt1 = std::make_shared<Geometry>(tri1, material, insideMedium, outsideMedium,
                                                                       nullptr);

            std::shared_ptr<meta::Shape> tri2 = std::make_shared<meta::Triangle>(a1, a2, a3,
                                                                                 n, n, n,
                                                                                 default_uv, default_uv, default_uv);
            std::shared_ptr<Geometry> gt2 = std::make_shared<Geometry>(tri2, material, insideMedium, outsideMedium,
                                                                       nullptr);
            std::vector<std::shared_ptr<Geometry>> v;
            v.push_back(gt1);
            v.push_back(gt2);
            return v;
        }

        std::vector<std::shared_ptr<Geometry>>
        Scene::testBottomWall(const std::shared_ptr<Material> material,
                              const std::shared_ptr<Medium> insideMedium,
                              const std::shared_ptr<Medium> outsideMedium) {
            const Vector3F a1(0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE);
            const Vector3F a2(-0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE);
            const Vector3F a3(-0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE);
            const Vector3F a4(0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE);

            const Normal3F n(0, 1, 0);
            const Vector2f default_uv(0);

            std::shared_ptr<meta::Shape> tri1 = std::make_shared<meta::Triangle>(a1, a3, a4,
                                                                                 n, n, n,
                                                                                 default_uv, default_uv, default_uv);
            std::shared_ptr<Geometry> gt1 = std::make_shared<Geometry>(tri1, material, insideMedium, outsideMedium,
                                                                       nullptr);

            std::shared_ptr<meta::Shape> tri2 = std::make_shared<meta::Triangle>(a1, a2, a3,
                                                                                 n, n, n,
                                                                                 default_uv, default_uv, default_uv);
            std::shared_ptr<Geometry> gt2 = std::make_shared<Geometry>(tri2, material, insideMedium, outsideMedium,
                                                                       nullptr);
            std::vector<std::shared_ptr<Geometry>> v;
            v.push_back(gt1);
            v.push_back(gt2);
            return v;
        }


        std::vector<std::shared_ptr<Geometry>>
        Scene::testTopAreaLight(const Spectrum spectrum, const std::shared_ptr<Medium> medium,
                                std::vector<std::shared_ptr<Light>> &lights, const std::shared_ptr<Material> material) {
            const Vector3F a1(0.2 * MODEL_SCALE, 0.46 * MODEL_SCALE, -0.2 * MODEL_SCALE);
            const Vector3F a2(-0.2 * MODEL_SCALE, 0.46 * MODEL_SCALE, -0.2 * MODEL_SCALE);
            const Vector3F a3(-0.2 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0.2 * MODEL_SCALE);
            const Vector3F a4(0.2 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0.2 * MODEL_SCALE);

            const Normal3F n(0, -1, 0);
            const Vector2f default_uv(0);

            std::shared_ptr<meta::Shape> tri1 = std::make_shared<meta::Triangle>(a1, a3, a4,
                                                                                 n, n, n,
                                                                                 default_uv, default_uv, default_uv);
            std::shared_ptr<Geometry> gt1 = std::make_shared<Geometry>(tri1, material, medium, medium,
                                                                       nullptr);

            std::shared_ptr<meta::Shape> tri2 = std::make_shared<meta::Triangle>(a1, a2, a3,
                                                                                 n, n, n,
                                                                                 default_uv, default_uv, default_uv);
            std::shared_ptr<Geometry> gt2 = std::make_shared<Geometry>(tri2, material, medium, medium,
                                                                       nullptr);
            std::vector<std::shared_ptr<Geometry>> v;
            v.push_back(gt1);
            v.push_back(gt2);
            // build area light
            std::shared_ptr<AreaLight> light1 = testDiffuseAreaLight(spectrum, gt1, medium, medium, true);
            std::shared_ptr<AreaLight> light2 = testDiffuseAreaLight(spectrum, gt2, medium, medium, true);
            lights.push_back(light1);
            lights.push_back(light2);
            return v;
        }

        std::vector<std::shared_ptr<Geometry>> Scene::testTopWall(const std::shared_ptr<Material> material,
                                                                  const std::shared_ptr<Medium> insideMedium,
                                                                  const std::shared_ptr<Medium> outsideMedium) {
            const Vector3F a1(0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE);
            const Vector3F a2(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE);
            const Vector3F a3(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE);
            const Vector3F a4(0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE);

            const Normal3F n(0, -1, 0);
            const Vector2f default_uv(0);

            std::shared_ptr<meta::Shape> tri1 = std::make_shared<meta::Triangle>(a1, a3, a4,
                                                                                 n, n, n,
                                                                                 default_uv, default_uv, default_uv);
            std::shared_ptr<Geometry> gt1 = std::make_shared<Geometry>(tri1, material, insideMedium, outsideMedium,
                                                                       nullptr);

            std::shared_ptr<meta::Shape> tri2 = std::make_shared<meta::Triangle>(a1, a2, a3,
                                                                                 n, n, n,
                                                                                 default_uv, default_uv, default_uv);
            std::shared_ptr<Geometry> gt2 = std::make_shared<Geometry>(tri2, material, insideMedium, outsideMedium,
                                                                       nullptr);
            std::vector<std::shared_ptr<Geometry>> v;
            v.push_back(gt1);
            v.push_back(gt2);
            return v;
        }

        std::vector<std::shared_ptr<Geometry>>
        Scene::testFrontWall(const std::shared_ptr<Material> material,
                             const std::shared_ptr<Medium> insideMedium,
                             const std::shared_ptr<Medium> outsideMedium) {
            const Vector3F a1(0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE);
            const Vector3F a2(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE);
            const Vector3F a3(-0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE);
            const Vector3F a4(0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, -0.5 * MODEL_SCALE);

            const Normal3F n(0, 0, 1);
            const Vector2F default_uv(0);

            std::shared_ptr<meta::Shape> tri1 = std::make_shared<meta::Triangle>(a1, a3, a4,
                                                                                 n, n, n,
                                                                                 default_uv, default_uv, default_uv);
            std::shared_ptr<Geometry> gt1 = std::make_shared<Geometry>(tri1, material, insideMedium, outsideMedium,
                                                                       nullptr);

            std::shared_ptr<meta::Shape> tri2 = std::make_shared<meta::Triangle>(a1, a2, a3,
                                                                                 n, n, n,
                                                                                 default_uv, default_uv, default_uv);
            std::shared_ptr<Geometry> gt2 = std::make_shared<Geometry>(tri2, material, insideMedium, outsideMedium,
                                                                       nullptr);
            std::vector<std::shared_ptr<Geometry>> v;
            v.push_back(gt1);
            v.push_back(gt2);
            return v;
        }


        std::shared_ptr<Aggregation> Scene::testBunny(const std::shared_ptr<Material> material,
                                                      const std::shared_ptr<Medium> inside,
                                                      const std::shared_ptr<Medium> outside,
                                                      const std::shared_ptr<AreaLight> areaLight) {
            std::vector<Vertex> bunnyVertexes = kaguya::utils::ObjLoader::loadModel("./resource/objects/bunny.obj");

            Float scale = 0.4 * MODEL_SCALE;
            Matrix4F mat(1.0f);
            mat = TRANSLATE(mat, Vector3F(0, -scale / 1.2, 0));
            mat = SCALE(mat, Vector3F(scale, scale, scale));
            std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>(mat);
            std::shared_ptr<Aggregation> bunny = std::make_shared<TriangleMesh>(bunnyVertexes, material, inside,
                                                                                outside,
                                                                                areaLight, transformMatrix);
            return bunny;
        }

        std::shared_ptr<AreaLight> Scene::testDiffuseAreaLight(const Spectrum &spectrum,
                                                               const std::shared_ptr<Geometry> geometry,
                                                               const std::shared_ptr<Medium> inside,
                                                               const std::shared_ptr<Medium> outside,
                                                               bool singleSide) {
            std::shared_ptr<AreaLight> light = DiffuseAreaLight::buildDiffuseAreaLight(
                    spectrum, geometry, MediumBound(inside.get(), outside.get()), singleSide);
            return light;
        }

        std::shared_ptr<Scene> Scene::sceneSmoke() {
            // For testing
            // albedos
            // total white
            Spectrum totalWhiteSpectrum = Spectrum(1.0);
            std::shared_ptr<Texture> totalWhite = std::make_shared<ConstantTexture>(totalWhiteSpectrum);

            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture> white = std::make_shared<ConstantTexture>(whiteSpectrum);

            // red
            Spectrum redSpectrum = Spectrum(0.0);
            redSpectrum.r(0.65);
            redSpectrum.g(0.05);
            redSpectrum.b(0.05);
            std::shared_ptr<Texture> red = std::make_shared<ConstantTexture>(redSpectrum);

            // lake blue
            Spectrum lakeBlueSpectrum = Spectrum(0.0);
            lakeBlueSpectrum.r(30.0 / 255);
            lakeBlueSpectrum.g(144.0 / 255);
            lakeBlueSpectrum.b(200.0 / 255);
            std::shared_ptr<Texture> lakeBlue = std::make_shared<ConstantTexture>(lakeBlueSpectrum);

            // green
            Spectrum greenSpectrum = Spectrum(0.0);
            greenSpectrum.r(0.12);
            greenSpectrum.g(0.45);
            greenSpectrum.b(0.15);
            std::shared_ptr<Texture> green = std::make_shared<ConstantTexture>(greenSpectrum);

            // blue
            Spectrum blueSpectrum = Spectrum(0.0);
            blueSpectrum.r(0);
            blueSpectrum.g(0);
            blueSpectrum.b(1.0);
            std::shared_ptr<Texture> blue = std::make_shared<ConstantTexture>(blueSpectrum);

            // pink
            Spectrum pinkSpectrum = Spectrum(0.0);
            pinkSpectrum.r(255.0 / 255);
            pinkSpectrum.g(192.0 / 255);
            pinkSpectrum.b(203.0 / 255);
            std::shared_ptr<Texture> pink = std::make_shared<ConstantTexture>(pinkSpectrum);

            // light spectrum
            Spectrum areaLightSpectrum = Spectrum(0.0);
            Float areaLightIntensity = 15;
            areaLightSpectrum.r(Float(249.0) / 255.0 * areaLightIntensity);
            areaLightSpectrum.g(Float(222.0) / 255.0 * areaLightIntensity);
            areaLightSpectrum.b(Float(180.0) / 255.0 * areaLightIntensity);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

//            std::shared_ptr<Medium> airMedium = testAirMedium();
            std::shared_ptr<Medium> airMedium = nullptr;

            // smoke data
            Float scale = 0.6 * MODEL_SCALE;
            Matrix4F mat(1.0f);
            mat = TRANSLATE(mat, Vector3F(-scale * 0.6, -MODEL_SCALE * 0.5 + 0.0001, -scale * 0.5));
            mat = SCALE(mat, Vector3F(scale * 1.2, MODEL_SCALE * 0.90, scale * 1.2));
            std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>(mat);

            Float *smoke = testSmokeData();
            std::shared_ptr<Medium> smokeMedium = std::make_shared<GridDensityMedium>(0.002, 2.3, 0, gridx, gridy,
                                                                                      gridz,
                                                                                      smoke, transformMatrix);

            // objects
            std::vector<std::shared_ptr<Intersectable>> objects;

            // tiny box wrap smoke
            Box smokeWrapper = Box(nullptr, smokeMedium, airMedium, transformMatrix);
            std::vector<std::shared_ptr<Intersectable>> boxes = smokeWrapper.aggregation();
            objects.insert(objects.end(), boxes.begin(), boxes.end());

            // walls
            std::vector<std::shared_ptr<Geometry>> leftWall =
                    testLeftWall(lambertLeft, airMedium, airMedium);
            objects.insert(objects.end(), leftWall.begin(), leftWall.end());

            std::vector<std::shared_ptr<Geometry>> rightWall =
                    testRightWall(lambertRight, airMedium, airMedium);
            objects.insert(objects.end(), rightWall.begin(), rightWall.end());

            std::vector<std::shared_ptr<Geometry>> bottomWall = testBottomWall(lambertBottom, airMedium, airMedium);
            objects.insert(objects.end(), bottomWall.begin(), bottomWall.end());


            std::vector<std::shared_ptr<Geometry>> topWall = testTopWall(lambertTop, airMedium, airMedium);
            objects.insert(objects.end(), topWall.begin(), topWall.end());

            std::vector<std::shared_ptr<Geometry>> frontWall = testFrontWall(lambertFront, airMedium, airMedium);
            objects.insert(objects.end(), frontWall.begin(), frontWall.end());

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build area light
            auto lightWall = testTopAreaLight(areaLightSpectrum, airMedium, scene->_lights, lambertTop);
            objects.insert(objects.end(), lightWall.begin(), lightWall.end());

            // scene
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(objects);
            scene->_world = bvh;

            // build camera
            auto eye = Vector3F(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3F(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir, airMedium);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            scene->_camera = camera;
            scene->_sceneName = "smoke-with-area-light";

            return scene;
        }

        std::shared_ptr<Scene> Scene::cornelBoxWater() {
            // For testing
            // albedos
            // total white
            Spectrum totalWhiteSpectrum = Spectrum(1.0);
            std::shared_ptr<Texture> totalWhite = std::make_shared<ConstantTexture>(totalWhiteSpectrum);

            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture> white = std::make_shared<ConstantTexture>(whiteSpectrum);

            // red
            Spectrum redSpectrum = Spectrum(0.0);
            redSpectrum.r(0.65);
            redSpectrum.g(0.05);
            redSpectrum.b(0.05);
            std::shared_ptr<Texture> red = std::make_shared<ConstantTexture>(redSpectrum);

            // lake blue
            Spectrum lakeBlueSpectrum = Spectrum(0.0);
            lakeBlueSpectrum.r(30.0 / 255);
            lakeBlueSpectrum.g(144.0 / 255);
            lakeBlueSpectrum.b(200.0 / 255);
            std::shared_ptr<Texture> lakeBlue = std::make_shared<ConstantTexture>(lakeBlueSpectrum);

            // green
            Spectrum greenSpectrum = Spectrum(0.0);
            greenSpectrum.r(0.12);
            greenSpectrum.g(0.45);
            greenSpectrum.b(0.15);
            std::shared_ptr<Texture> green = std::make_shared<ConstantTexture>(greenSpectrum);

            // blue
            Spectrum blueSpectrum = Spectrum(0.0);
            blueSpectrum.r(0);
            blueSpectrum.g(0);
            blueSpectrum.b(1.0);
            std::shared_ptr<Texture> blue = std::make_shared<ConstantTexture>(blueSpectrum);

            // pink
            Spectrum pinkSpectrum = Spectrum(0.0);
            pinkSpectrum.r(255.0 / 255);
            pinkSpectrum.g(192.0 / 255);
            pinkSpectrum.b(203.0 / 255);
            std::shared_ptr<Texture> pink = std::make_shared<ConstantTexture>(pinkSpectrum);

            // light spectrum
            Spectrum lightSpectrum = Spectrum(0.0);
            Float lightIntensity = 12;
            lightSpectrum.r(Float(249.0) / 255.0 * lightIntensity);
            lightSpectrum.g(Float(222.0) / 255.0 * lightIntensity);
            lightSpectrum.b(Float(180.0) / 255.0 * lightIntensity);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

//            std::shared_ptr<Medium> airMedium = testAirMedium();
            std::shared_ptr<Medium> airMedium = nullptr;
            // objects
            std::vector<std::shared_ptr<Intersectable>> objects;

            // walls
            std::vector<std::shared_ptr<Geometry>> leftWall =
                    testLeftWall(lambertLeft, airMedium, airMedium);
            objects.insert(objects.end(), leftWall.begin(), leftWall.end());

            std::vector<std::shared_ptr<Geometry>> rightWall =
                    testRightWall(lambertRight, airMedium, airMedium);
            objects.insert(objects.end(), rightWall.begin(), rightWall.end());

            std::vector<std::shared_ptr<Geometry>> bottomWall = testBottomWall(lambertBottom, airMedium, airMedium);
            objects.insert(objects.end(), bottomWall.begin(), bottomWall.end());


            std::vector<std::shared_ptr<Geometry>> topWall = testTopWall(lambertTop, airMedium, airMedium);
            objects.insert(objects.end(), topWall.begin(), topWall.end());

            std::vector<std::shared_ptr<Geometry>> frontWall = testFrontWall(lambertFront, airMedium, airMedium);
            objects.insert(objects.end(), frontWall.begin(), frontWall.end());
            std::vector<Vertex> waterVertexes = kaguya::utils::ObjLoader::loadModel("./resource/objects/water.obj");

            Float scale = 0.5 * MODEL_SCALE;
            Matrix4F mat(1.0f);
            mat = TRANSLATE(mat, Vector3F(0, -scale, 0));
            mat = SCALE(mat, Vector3F(scale, scale, scale));
            std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>(mat);
            std::shared_ptr<Aggregation> water = std::make_shared<TriangleMesh>(waterVertexes, glass, nullptr,
                                                                                nullptr, nullptr, transformMatrix);

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build area light
            auto lightWall = testTopAreaLight(lightSpectrum, airMedium, scene->_lights, lambertTop);
            objects.insert(objects.end(), lightWall.begin(), lightWall.end());

            objects.push_back(water);

            // scene
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(objects);
            scene->_world = bvh;

            // build camera
            auto eye = Vector3F(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3F(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir, airMedium);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            scene->_camera = camera;
            scene->_sceneName = "cornell-box-with-water";

            return scene;
        }

#endif

        std::shared_ptr<Scene> Scene::sceneBunnyWithPointLight() {
            // For testing
            // albedos
            // total white
            Spectrum totalWhiteSpectrum = Spectrum(1.0);
            std::shared_ptr<Texture> totalWhite = std::make_shared<ConstantTexture>(totalWhiteSpectrum);

            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture> white = std::make_shared<ConstantTexture>(whiteSpectrum);

            // red
            Spectrum redSpectrum = Spectrum(0.0);
            redSpectrum.r(0.65);
            redSpectrum.g(0.05);
            redSpectrum.b(0.05);
            std::shared_ptr<Texture> red = std::make_shared<ConstantTexture>(redSpectrum);

            // lake blue
            Spectrum lakeBlueSpectrum = Spectrum(0.0);
            lakeBlueSpectrum.r(30.0 / 255);
            lakeBlueSpectrum.g(144.0 / 255);
            lakeBlueSpectrum.b(200.0 / 255);
            std::shared_ptr<Texture> lakeBlue = std::make_shared<ConstantTexture>(lakeBlueSpectrum);

            // green
            Spectrum greenSpectrum = Spectrum(0.0);
            greenSpectrum.r(0.12);
            greenSpectrum.g(0.45);
            greenSpectrum.b(0.15);
            std::shared_ptr<Texture> green = std::make_shared<ConstantTexture>(greenSpectrum);

            // blue
            Spectrum blueSpectrum = Spectrum(0.0);
            blueSpectrum.r(0);
            blueSpectrum.g(0);
            blueSpectrum.b(1.0);
            std::shared_ptr<Texture> blue = std::make_shared<ConstantTexture>(blueSpectrum);

            // pink
            Spectrum pinkSpectrum = Spectrum(0.0);
            pinkSpectrum.r(255.0 / 255);
            pinkSpectrum.g(192.0 / 255);
            pinkSpectrum.b(203.0 / 255);
            std::shared_ptr<Texture> pink = std::make_shared<ConstantTexture>(pinkSpectrum);

            // light spectrum
            Spectrum lightSpectrum = Spectrum(0.0);
            Float lightIntensity = 12;
            lightSpectrum.r(Float(249.0) / 255.0 * lightIntensity);
            lightSpectrum.g(Float(222.0) / 255.0 * lightIntensity);
            lightSpectrum.b(Float(180.0) / 255.0 * lightIntensity);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            std::shared_ptr<Medium> airMedium = testAirMedium();

            // objects
            std::vector<std::shared_ptr<Intersectable>> objects;

            // walls
            std::vector<std::shared_ptr<Geometry>> leftWall =
                    testLeftWall(lambertLeft, airMedium, airMedium);
            objects.insert(objects.end(), leftWall.begin(), leftWall.end());

            std::vector<std::shared_ptr<Geometry>> rightWall =
                    testRightWall(lambertRight, airMedium, airMedium);
            objects.insert(objects.end(), rightWall.begin(), rightWall.end());

            std::vector<std::shared_ptr<Geometry>> bottomWall = testBottomWall(lambertBottom, airMedium, airMedium);
            objects.insert(objects.end(), bottomWall.begin(), bottomWall.end());


            std::vector<std::shared_ptr<Geometry>> topWall = testTopWall(lambertTop, airMedium, airMedium);
            objects.insert(objects.end(), topWall.begin(), topWall.end());

            std::vector<std::shared_ptr<Geometry>> frontWall = testFrontWall(lambertFront, airMedium, airMedium);
            objects.insert(objects.end(), frontWall.begin(), frontWall.end());

            // load model
            std::shared_ptr<Intersectable> bunny = testBunny(glass, nullptr, airMedium, nullptr);
            objects.push_back(bunny);

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build point light
            std::shared_ptr<PointLight> light = PointLight::buildPointLight(
                    Vector3F(0 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0 * MODEL_SCALE), lightSpectrum,
                    MediumBound(airMedium.get(), airMedium.get()));
            scene->_lights.push_back(light);


            // scene
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(objects);
            scene->_world = bvh;

            // build camera
            auto eye = Vector3F(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3F(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir, airMedium);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            scene->_camera = camera;

            scene->_sceneName = "bunny-with-point-light";

            return scene;
        }


        std::shared_ptr<Scene> Scene::sceneBunnyWithAreaLight() {
            // For testing
            // albedos
            // total white
            Spectrum totalWhiteSpectrum = Spectrum(1.0);
            std::shared_ptr<Texture> totalWhite = std::make_shared<ConstantTexture>(totalWhiteSpectrum);

            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture> white = std::make_shared<ConstantTexture>(whiteSpectrum);

            // red
            Spectrum redSpectrum = Spectrum(0.0);
            redSpectrum.r(0.65);
            redSpectrum.g(0.05);
            redSpectrum.b(0.05);
            std::shared_ptr<Texture> red = std::make_shared<ConstantTexture>(redSpectrum);

            // lake blue
            Spectrum lakeBlueSpectrum = Spectrum(0.0);
            lakeBlueSpectrum.r(30.0 / 255);
            lakeBlueSpectrum.g(144.0 / 255);
            lakeBlueSpectrum.b(200.0 / 255);
            std::shared_ptr<Texture> lakeBlue = std::make_shared<ConstantTexture>(lakeBlueSpectrum);

            // green
            Spectrum greenSpectrum = Spectrum(0.0);
            greenSpectrum.r(0.12);
            greenSpectrum.g(0.45);
            greenSpectrum.b(0.15);
            std::shared_ptr<Texture> green = std::make_shared<ConstantTexture>(greenSpectrum);

            // blue
            Spectrum blueSpectrum = Spectrum(0.0);
            blueSpectrum.r(0);
            blueSpectrum.g(0);
            blueSpectrum.b(1.0);
            std::shared_ptr<Texture> blue = std::make_shared<ConstantTexture>(blueSpectrum);

            // pink
            Spectrum pinkSpectrum = Spectrum(0.0);
            pinkSpectrum.r(255.0 / 255);
            pinkSpectrum.g(192.0 / 255);
            pinkSpectrum.b(203.0 / 255);
            std::shared_ptr<Texture> pink = std::make_shared<ConstantTexture>(pinkSpectrum);

            // light spectrum
            Spectrum lightSpectrum = Spectrum(0.0);
            Float lightIntensity = 12;
            lightSpectrum.r(Float(249.0) / 255.0 * lightIntensity);
            lightSpectrum.g(Float(222.0) / 255.0 * lightIntensity);
            lightSpectrum.b(Float(180.0) / 255.0 * lightIntensity);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            // medium
            Spectrum sigmaA(0);
            sigmaA.r(0.0011f);
            sigmaA.g(0.0024f);
            sigmaA.b(0.014f);

//            std::shared_ptr<Medium> airMedium = testAirMedium();
            std::shared_ptr<Medium> airMedium = nullptr;
            std::shared_ptr<Medium> bunnyMedium = testSmokeMedium();
//            std::shared_ptr<Medium> bunnyMedium = nullptr;

            // objects
            std::vector<std::shared_ptr<Intersectable>> objects;

            // walls
            std::vector<std::shared_ptr<Geometry>> leftWall =
                    testLeftWall(lambertLeft, airMedium, airMedium);
            objects.insert(objects.end(), leftWall.begin(), leftWall.end());

            std::vector<std::shared_ptr<Geometry>> rightWall =
                    testRightWall(lambertRight, airMedium, airMedium);
            objects.insert(objects.end(), rightWall.begin(), rightWall.end());

            std::vector<std::shared_ptr<Geometry>> bottomWall = testBottomWall(lambertBottom, airMedium, airMedium);
            objects.insert(objects.end(), bottomWall.begin(), bottomWall.end());


            std::vector<std::shared_ptr<Geometry>> topWall = testTopWall(lambertTop, airMedium, airMedium);
            objects.insert(objects.end(), topWall.begin(), topWall.end());

            std::vector<std::shared_ptr<Geometry>> frontWall = testFrontWall(lambertFront, airMedium, airMedium);
            objects.insert(objects.end(), frontWall.begin(), frontWall.end());

            // load model
            std::shared_ptr<Intersectable> bunny = testBunny(glass, bunnyMedium, airMedium, nullptr);
//            std::shared_ptr<Intersectable> bunny = testBunny(nullptr, bunnyMedium, airMedium, nullptr);

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build area light
            auto lightWall = testTopAreaLight(lightSpectrum, airMedium, scene->_lights, lambertTop);
            objects.insert(objects.end(), lightWall.begin(), lightWall.end());

            objects.push_back(bunny);

            // scene
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(objects);
            scene->_world = bvh;

            // build camera
            auto eye = Vector3F(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3F(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir, airMedium);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            scene->_camera = camera;
            scene->_sceneName = "bunny-with-area-light";

            return scene;
        }

        std::shared_ptr<Scene> Scene::sceneTwoSpheresWithAreaLight() {
            // For testing
            // albedos
            // total white
            Spectrum totalWhiteSpectrum = Spectrum(1.0);
            std::shared_ptr<Texture> totalWhite = std::make_shared<ConstantTexture>(totalWhiteSpectrum);

            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture> white = std::make_shared<ConstantTexture>(whiteSpectrum);

            // red
            Spectrum redSpectrum = Spectrum(0.0);
            redSpectrum.r(0.65);
            redSpectrum.g(0.05);
            redSpectrum.b(0.05);
            std::shared_ptr<Texture> red = std::make_shared<ConstantTexture>(redSpectrum);

            // lake blue
            Spectrum lakeBlueSpectrum = Spectrum(0.0);
            lakeBlueSpectrum.r(30.0 / 255);
            lakeBlueSpectrum.g(144.0 / 255);
            lakeBlueSpectrum.b(200.0 / 255);
            std::shared_ptr<Texture> lakeBlue = std::make_shared<ConstantTexture>(lakeBlueSpectrum);

            // green
            Spectrum greenSpectrum = Spectrum(0.0);
            greenSpectrum.r(0.12);
            greenSpectrum.g(0.45);
            greenSpectrum.b(0.15);
            std::shared_ptr<Texture> green = std::make_shared<ConstantTexture>(greenSpectrum);

            // blue
            Spectrum blueSpectrum = Spectrum(0.0);
            blueSpectrum.r(0);
            blueSpectrum.g(0);
            blueSpectrum.b(1.0);
            std::shared_ptr<Texture> blue = std::make_shared<ConstantTexture>(blueSpectrum);

            // pink
            Spectrum pinkSpectrum = Spectrum(0.0);
            pinkSpectrum.r(255.0 / 255);
            pinkSpectrum.g(192.0 / 255);
            pinkSpectrum.b(203.0 / 255);
            std::shared_ptr<Texture> pink = std::make_shared<ConstantTexture>(pinkSpectrum);

            // light spectrum
            Spectrum areaLightSpectrum = Spectrum(0.0);
            Float areaLightIntensity = 15;
            areaLightSpectrum.r(Float(249.0) / 255.0 * areaLightIntensity);
            areaLightSpectrum.g(Float(222.0) / 255.0 * areaLightIntensity);
            areaLightSpectrum.b(Float(180.0) / 255.0 * areaLightIntensity);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

//            std::shared_ptr<Medium> airMedium = testAirMedium();
            std::shared_ptr<Medium> airMedium = nullptr;

            // objects
            std::vector<std::shared_ptr<Intersectable>> objects;

            // walls
            std::vector<std::shared_ptr<Geometry>> leftWall =
                    testLeftWall(lambertLeft, airMedium, airMedium);
            objects.insert(objects.end(), leftWall.begin(), leftWall.end());

            std::vector<std::shared_ptr<Geometry>> rightWall =
                    testRightWall(lambertRight, airMedium, airMedium);
            objects.insert(objects.end(), rightWall.begin(), rightWall.end());

            std::vector<std::shared_ptr<Geometry>> bottomWall = testBottomWall(lambertBottom, airMedium, airMedium);
            objects.insert(objects.end(), bottomWall.begin(), bottomWall.end());


            std::vector<std::shared_ptr<Geometry>> topWall = testTopWall(lambertTop, airMedium, airMedium);
            objects.insert(objects.end(), topWall.begin(), topWall.end());

            std::vector<std::shared_ptr<Geometry>> frontWall = testFrontWall(lambertFront, airMedium, airMedium);
            objects.insert(objects.end(), frontWall.begin(), frontWall.end());

            std::shared_ptr<Shape> glassSphereShape = std::make_shared<meta::Sphere>(
                    Vector3F(0.25 * MODEL_SCALE, -0.338 * MODEL_SCALE, 0 * MODEL_SCALE), 0.16 * MODEL_SCALE);
            std::shared_ptr<Geometry> glassSphere = std::make_shared<Geometry>(glassSphereShape, glass,
                                                                               nullptr, airMedium);

            std::shared_ptr<Shape> metalSphereShape = std::make_shared<meta::Sphere>(
                    Vector3F(-0.25 * MODEL_SCALE, -0.298 * MODEL_SCALE, 0.2 * MODEL_SCALE), 0.2 * MODEL_SCALE);
            std::shared_ptr<Geometry> metalSphere = std::make_shared<Geometry>(metalSphereShape, metal,
                                                                               nullptr, airMedium);

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build area light
            auto lightWall = testTopAreaLight(areaLightSpectrum, airMedium, scene->_lights, lambertTop);
            objects.insert(objects.end(), lightWall.begin(), lightWall.end());

            objects.push_back(glassSphere);
            objects.push_back(metalSphere);
//            objects.push_back(mediumSphere);

            // scene
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(objects);
            scene->_world = bvh;

            // build camera
            auto eye = Vector3F(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3F(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir, airMedium);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            scene->_camera = camera;
            scene->_sceneName = "two-spheres-with-area-light";

            return scene;
        }

        std::shared_ptr<Scene> Scene::sceneTwoSpheresWithSpotLight() {
            // For testing
            // albedos
            // total white
            Spectrum totalWhiteSpectrum = Spectrum(1.0);
            std::shared_ptr<Texture> totalWhite = std::make_shared<ConstantTexture>(totalWhiteSpectrum);

            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture> white = std::make_shared<ConstantTexture>(whiteSpectrum);

            // red
            Spectrum redSpectrum = Spectrum(0.0);
            redSpectrum.r(0.65);
            redSpectrum.g(0.05);
            redSpectrum.b(0.05);
            std::shared_ptr<Texture> red = std::make_shared<ConstantTexture>(redSpectrum);

            // lake blue
            Spectrum lakeBlueSpectrum = Spectrum(0.0);
            lakeBlueSpectrum.r(30.0 / 255);
            lakeBlueSpectrum.g(144.0 / 255);
            lakeBlueSpectrum.b(200.0 / 255);
            std::shared_ptr<Texture> lakeBlue = std::make_shared<ConstantTexture>(lakeBlueSpectrum);

            // green
            Spectrum greenSpectrum = Spectrum(0.0);
            greenSpectrum.r(0.12);
            greenSpectrum.g(0.45);
            greenSpectrum.b(0.15);
            std::shared_ptr<Texture> green = std::make_shared<ConstantTexture>(greenSpectrum);

            // blue
            Spectrum blueSpectrum = Spectrum(0.0);
            blueSpectrum.r(0);
            blueSpectrum.g(0);
            blueSpectrum.b(1.0);
            std::shared_ptr<Texture> blue = std::make_shared<ConstantTexture>(blueSpectrum);

            // pink
            Spectrum pinkSpectrum = Spectrum(0.0);
            pinkSpectrum.r(255.0 / 255);
            pinkSpectrum.g(192.0 / 255);
            pinkSpectrum.b(203.0 / 255);
            std::shared_ptr<Texture> pink = std::make_shared<ConstantTexture>(pinkSpectrum);

            // light spectrum
            Spectrum spotLightSpectrum = Spectrum(0.0);
            Float areaLightIntensity = 200;
            spotLightSpectrum.r(Float(249.0) / 255.0 * areaLightIntensity);
            spotLightSpectrum.g(Float(222.0) / 255.0 * areaLightIntensity);
            spotLightSpectrum.b(Float(180.0) / 255.0 * areaLightIntensity);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            // medium
            Spectrum sigmaA(0);
            sigmaA.r(0.0011f);
            sigmaA.g(0.0024f);
            sigmaA.b(0.014f);

            std::shared_ptr<Medium> airMedium = testAirMedium();

            // objects
            std::vector<std::shared_ptr<Intersectable>> objects;

            // walls
            std::vector<std::shared_ptr<Geometry>> leftWall =
                    testLeftWall(lambertLeft, airMedium, airMedium);
            objects.insert(objects.end(), leftWall.begin(), leftWall.end());

            std::vector<std::shared_ptr<Geometry>> rightWall =
                    testRightWall(lambertRight, airMedium, airMedium);
            objects.insert(objects.end(), rightWall.begin(), rightWall.end());

            std::vector<std::shared_ptr<Geometry>> bottomWall = testBottomWall(lambertBottom, airMedium, airMedium);
            objects.insert(objects.end(), bottomWall.begin(), bottomWall.end());


            std::vector<std::shared_ptr<Geometry>> topWall = testTopWall(lambertTop, airMedium, airMedium);
            objects.insert(objects.end(), topWall.begin(), topWall.end());

            std::vector<std::shared_ptr<Geometry>> frontWall = testFrontWall(lambertFront, airMedium, airMedium);
            objects.insert(objects.end(), frontWall.begin(), frontWall.end());

            std::shared_ptr<Shape> glassSphereShape = std::make_shared<meta::Sphere>(
                    Vector3F(0.25 * MODEL_SCALE, -0.338 * MODEL_SCALE, 0 * MODEL_SCALE), 0.16 * MODEL_SCALE);
            std::shared_ptr<Geometry> glassSphere = std::make_shared<Geometry>(glassSphereShape, glass,
                                                                               nullptr, airMedium);


            std::shared_ptr<Shape> metalSphereShape = std::make_shared<meta::Sphere>(
                    Vector3F(-0.25 * MODEL_SCALE, -0.298 * MODEL_SCALE, 0.2 * MODEL_SCALE), 0.2 * MODEL_SCALE);
            std::shared_ptr<Geometry> metalSphere = std::make_shared<Geometry>(metalSphereShape, metal,
                                                                               nullptr, airMedium);

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build point light
            std::shared_ptr<SpotLight> light = SpotLight::buildSpotLight(
                    Vector3F(0 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0 * MODEL_SCALE), Vector3F(0.0, -1, 0.0),
                    spotLightSpectrum, MediumBound(airMedium.get(), airMedium.get()));
            scene->_lights.push_back(light);

            objects.push_back(glassSphere);
            objects.push_back(metalSphere);

            // scene
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(objects);
            scene->_world = bvh;

            // build camera
            auto eye = Vector3F(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3F(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir, airMedium);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            scene->_camera = camera;
            scene->_sceneName = "two-spheres-with-spot-light";

            return scene;
        }

        std::shared_ptr<Scene> Scene::sceneTwoSpheresWithPointLight() {
            // For testing
            // albedos
            // total white
            Spectrum totalWhiteSpectrum = Spectrum(1.0);
            std::shared_ptr<Texture> totalWhite = std::make_shared<ConstantTexture>(totalWhiteSpectrum);

            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture> white = std::make_shared<ConstantTexture>(whiteSpectrum);

            // red
            Spectrum redSpectrum = Spectrum(0.0);
            redSpectrum.r(0.65);
            redSpectrum.g(0.05);
            redSpectrum.b(0.05);
            std::shared_ptr<Texture> red = std::make_shared<ConstantTexture>(redSpectrum);

            // lake blue
            Spectrum lakeBlueSpectrum = Spectrum(0.0);
            lakeBlueSpectrum.r(30.0 / 255);
            lakeBlueSpectrum.g(144.0 / 255);
            lakeBlueSpectrum.b(200.0 / 255);
            std::shared_ptr<Texture> lakeBlue = std::make_shared<ConstantTexture>(lakeBlueSpectrum);

            // green
            Spectrum greenSpectrum = Spectrum(0.0);
            greenSpectrum.r(0.12);
            greenSpectrum.g(0.45);
            greenSpectrum.b(0.15);
            std::shared_ptr<Texture> green = std::make_shared<ConstantTexture>(greenSpectrum);

            // blue
            Spectrum blueSpectrum = Spectrum(0.0);
            blueSpectrum.r(0);
            blueSpectrum.g(0);
            blueSpectrum.b(1.0);
            std::shared_ptr<Texture> blue = std::make_shared<ConstantTexture>(blueSpectrum);

            // pink
            Spectrum pinkSpectrum = Spectrum(0.0);
            pinkSpectrum.r(255.0 / 255);
            pinkSpectrum.g(192.0 / 255);
            pinkSpectrum.b(203.0 / 255);
            std::shared_ptr<Texture> pink = std::make_shared<ConstantTexture>(pinkSpectrum);

            // light spectrum
            Spectrum lightSpectrum = Spectrum(0.0);
            Float lightIntensity = 15;
            lightSpectrum.r(Float(249.0) / 255.0 * lightIntensity);
            lightSpectrum.g(Float(222.0) / 255.0 * lightIntensity);
            lightSpectrum.b(Float(180.0) / 255.0 * lightIntensity);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            std::shared_ptr<Medium> airMedium = testAirMedium();

            // objects
            std::vector<std::shared_ptr<Intersectable>> objects;

            // walls
            std::vector<std::shared_ptr<Geometry>> leftWall =
                    testLeftWall(lambertLeft, airMedium, airMedium);
            objects.insert(objects.end(), leftWall.begin(), leftWall.end());

            std::vector<std::shared_ptr<Geometry>> rightWall =
                    testRightWall(lambertRight, airMedium, airMedium);
            objects.insert(objects.end(), rightWall.begin(), rightWall.end());

            std::vector<std::shared_ptr<Geometry>> bottomWall = testBottomWall(lambertBottom, airMedium, airMedium);
            objects.insert(objects.end(), bottomWall.begin(), bottomWall.end());


            std::vector<std::shared_ptr<Geometry>> topWall = testTopWall(lambertTop, airMedium, airMedium);
            objects.insert(objects.end(), topWall.begin(), topWall.end());

            std::vector<std::shared_ptr<Geometry>> frontWall = testFrontWall(lambertFront, airMedium, airMedium);
            objects.insert(objects.end(), frontWall.begin(), frontWall.end());

            std::shared_ptr<Shape> glassSphereShape = std::make_shared<meta::Sphere>(
                    Vector3F(0.25 * MODEL_SCALE, -0.338 * MODEL_SCALE, 0 * MODEL_SCALE), 0.16 * MODEL_SCALE);
            std::shared_ptr<Geometry> glassSphere = std::make_shared<Geometry>(glassSphereShape, glass,
                                                                               nullptr, airMedium);

            std::shared_ptr<Shape> metalSphereShape = std::make_shared<meta::Sphere>(
                    Vector3F(-0.25 * MODEL_SCALE, -0.298 * MODEL_SCALE, 0.2 * MODEL_SCALE), 0.2 * MODEL_SCALE);
            std::shared_ptr<Geometry> metalSphere = std::make_shared<Geometry>(metalSphereShape, metal,
                                                                               nullptr, airMedium);

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build point light
            std::shared_ptr<PointLight> light = PointLight::buildPointLight(
                    Vector3F(0.0 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0 * MODEL_SCALE), lightSpectrum,
                    MediumBound(airMedium.get(), airMedium.get()));
            scene->_lights.push_back(light);

            objects.push_back(glassSphere);
            objects.push_back(metalSphere);

            // scene
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(objects);
            scene->_world = bvh;

            // build camera
            auto eye = Vector3F(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3F(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir, airMedium);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            scene->_camera = camera;
            scene->_sceneName = "two-spheres-with-point-light";

            return scene;
        }

        std::shared_ptr<Scene> Scene::sceneLightThroughAir() {
            // For testing
            // albedos
            // total white
            Spectrum totalWhiteSpectrum = Spectrum(1.0);
            std::shared_ptr<Texture> totalWhite = std::make_shared<ConstantTexture>(totalWhiteSpectrum);

            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture> white = std::make_shared<ConstantTexture>(whiteSpectrum);

            // red
            Spectrum redSpectrum = Spectrum(0.0);
            redSpectrum.r(0.65);
            redSpectrum.g(0.05);
            redSpectrum.b(0.05);
            std::shared_ptr<Texture> red = std::make_shared<ConstantTexture>(redSpectrum);

            // lake blue
            Spectrum lakeBlueSpectrum = Spectrum(0.0);
            lakeBlueSpectrum.r(30.0 / 255);
            lakeBlueSpectrum.g(144.0 / 255);
            lakeBlueSpectrum.b(200.0 / 255);
            std::shared_ptr<Texture> lakeBlue = std::make_shared<ConstantTexture>(lakeBlueSpectrum);

            // green
            Spectrum greenSpectrum = Spectrum(0.0);
            greenSpectrum.r(0.12);
            greenSpectrum.g(0.45);
            greenSpectrum.b(0.15);
            std::shared_ptr<Texture> green = std::make_shared<ConstantTexture>(greenSpectrum);

            // blue
            Spectrum blueSpectrum = Spectrum(0.0);
            blueSpectrum.r(0);
            blueSpectrum.g(0);
            blueSpectrum.b(1.0);
            std::shared_ptr<Texture> blue = std::make_shared<ConstantTexture>(blueSpectrum);

            // pink
            Spectrum pinkSpectrum = Spectrum(0.0);
            pinkSpectrum.r(255.0 / 255);
            pinkSpectrum.g(192.0 / 255);
            pinkSpectrum.b(203.0 / 255);
            std::shared_ptr<Texture> pink = std::make_shared<ConstantTexture>(pinkSpectrum);

            // light spectrum
            Spectrum pointLightSpectrum = Spectrum(0.0);
            Float lightIntensity = 15;
            pointLightSpectrum.r(Float(249.0) / 255.0 * lightIntensity);
            pointLightSpectrum.g(Float(222.0) / 255.0 * lightIntensity);
            pointLightSpectrum.b(Float(180.0) / 255.0 * lightIntensity);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            // medium
            std::shared_ptr<Medium> airMedium = testAirMedium();
//            std::shared_ptr<Medium> airMedium = nullptr;

            // objects
            std::vector<std::shared_ptr<Intersectable>> objects;

            // walls
            std::vector<std::shared_ptr<Geometry>> leftWall =
                    testLeftWall(lambertLeft, airMedium, airMedium);
            objects.insert(objects.end(), leftWall.begin(), leftWall.end());

            std::vector<std::shared_ptr<Geometry>> rightWall =
                    testRightWall(lambertRight, airMedium, airMedium);
            objects.insert(objects.end(), rightWall.begin(), rightWall.end());

            std::vector<std::shared_ptr<Geometry>> bottomWall = testBottomWall(lambertBottom, airMedium, airMedium);
            objects.insert(objects.end(), bottomWall.begin(), bottomWall.end());


            std::vector<std::shared_ptr<Geometry>> topWall = testTopWall(lambertTop, airMedium, airMedium);
            objects.insert(objects.end(), topWall.begin(), topWall.end());

            std::vector<std::shared_ptr<Geometry>> frontWall = testFrontWall(lambertFront, airMedium, airMedium);
            objects.insert(objects.end(), frontWall.begin(), frontWall.end());

            std::shared_ptr<Shape> glassSphereShape = std::make_shared<meta::Sphere>(
                    Vector3F(0.10 * MODEL_SCALE, 0.15 * MODEL_SCALE, 0 * MODEL_SCALE), 0.16 * MODEL_SCALE);
            std::shared_ptr<Geometry> glassSphere = std::make_shared<Geometry>(glassSphereShape, glass,
                                                                               nullptr, airMedium);

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build point light
            std::shared_ptr<PointLight> light = PointLight::buildPointLight(
                    Vector3F(0.0 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0 * MODEL_SCALE), pointLightSpectrum,
                    MediumBound(airMedium.get(), airMedium.get()));
            scene->_lights.push_back(light);

            objects.push_back(glassSphere);

            // scene
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(objects);
            scene->_world = bvh;

            // build camera
            auto eye = Vector3F(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3F(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir, airMedium);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            scene->_camera = camera;
            scene->_sceneName = "light_through-air";

            return scene;
        }

        bool Scene::intersect(Ray &ray, SurfaceInteraction &si) const {
            return _world->intersect(ray, si, ray.getMinStep(), ray.getStep());
        }

        bool Scene::intersectWithMedium(Ray &ray, SurfaceInteraction &si, core::Spectrum &transmittance,
                                        Sampler *sampler) const {
            transmittance = Spectrum(1.0);
            while (true) {
                bool foundIntersection = intersect(ray, si);
                if (ray.getMedium() != nullptr) {
                    transmittance *= ray.getMedium()->transmittance(ray, sampler);
                }

                if (!foundIntersection) {
                    return false;
                }

                // skip the shape without material and generate new ray
                if (si.getMaterial() == nullptr) {
                    ray = si.sendRay(ray.getDirection());
                } else {
                    return true;
                }
            }
        }
    }
}