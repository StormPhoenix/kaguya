//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/core/light/Light.h>
#include <kaguya/core/light/AreaLight.h>
#include <kaguya/core/light/DiffuseAreaLight.h>
#include <kaguya/core/light/PointLight.h>
#include <kaguya/core/light/SpotLight.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/Config.h>
#include <kaguya/scene/accumulation/BVH.h>
#include <kaguya/scene/meta/Sphere.h>
#include <kaguya/scene/meta/Wall.h>
#include <kaguya/scene/meta/TriangleMesh.h>
#include <kaguya/material/Dielectric.h>
#include <kaguya/material/Lambertian.h>
#include <kaguya/material/Metal.h>
#include <kaguya/material/ConstantTexture.h>
#include <kaguya/utils/ObjLoader.h>

namespace kaguya {
    namespace scene {

        using namespace kaguya::material;
        using namespace kaguya::scene::acc;
        using namespace kaguya::core;

        void Scene::testBuildCornelBoxWithBunny() {
            // For testing
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
            lightSpectrum.r(double(249.0) / 255.0 * 15);
            lightSpectrum.g(double(222.0) / 255.0 * 15);
            lightSpectrum.b(double(180.0) / 255.0 * 15);
            std::shared_ptr<Texture> lightIntensity = std::make_shared<ConstantTexture>(lightSpectrum);

            // 255 222 99

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            // walls
            std::shared_ptr<Shape> leftWall = std::make_shared<YZWall>(-250, 250, -250, 250, -250, true,
                                                                       lambertLeft);
            std::shared_ptr<Shape> rightWall = std::make_shared<YZWall>(-250, 250, -250, 250, 250, false,
                                                                        lambertRight);
            std::shared_ptr<Shape> bottomWall = std::make_shared<ZXWall>(-250, 250, -250, 250, -250, true,
                                                                         lambertBottom);
            std::shared_ptr<Shape> topWall = std::make_shared<ZXWall>(-250, 250, -250, 250, 250, false,
                                                                      lambertTop);
            std::shared_ptr<Shape> frontWall = std::make_shared<XYWall>(-250, 250, -250, 250, -250, false,
                                                                        lambertFront);

            std::shared_ptr<Shape> glassSphere = std::make_shared<Sphere>(Vector3(125, -169, 0), 80, glass);
//            std::shared_ptr<Shape> glassSphere = std::make_shared<Sphere>(Vector3(75, 0, 0), 80, metal);

            std::shared_ptr<Shape> metalSphere = std::make_shared<Sphere>(Vector3(-125, -149, 100), 100, metal);

            // light
            std::shared_ptr<ShapeSampler> lightWall = std::make_shared<ZXWall>(-100, 100, -100, 100, 245, false,
                                                                               nullptr);

            // build light
            std::shared_ptr<AreaLight> light = DiffuseAreaLight::buildDiffuseAreaLight(lightSpectrum, lightWall);
            _light = light;

            // load model
            std::vector<Vertex> bunnyVertexes = kaguya::utils::ObjLoader::loadModel("./resource/objects/bunny.obj");

            std::shared_ptr<Matrix4> transformMatrix = std::make_shared<Matrix4>(1.0f);
            double scale = 100.0;
            *transformMatrix = TRANSLATE(*transformMatrix, Vector3(scale / 4, -scale / 2 * 3, 0));
            *transformMatrix = SCALE(*transformMatrix, Vector3(scale, scale, scale));
            std::shared_ptr<Shape> bunny = std::static_pointer_cast<Shape>(
//                    std::make_shared<TriangleMesh>(bunnyVertexes, lambertTop, transformMatrix));
                    std::make_shared<TriangleMesh>(bunnyVertexes, glass, transformMatrix));
//                    std::make_shared<TriangleMesh>(bunnyVertexes, metal, transformMatrix));

            // objects
            std::vector<std::shared_ptr<Shape>> objects;
            objects.push_back(leftWall);
            objects.push_back(rightWall);
            objects.push_back(bottomWall);
            objects.push_back(topWall);
            objects.push_back(frontWall);
            objects.push_back(lightWall);
            // objects.push_back(bunny);

            // 给所有 object 赋予 id
            for (long long id = 0; id < objects.size(); id++) {
                objects[id]->setId(id);
            }

            // scene
            std::shared_ptr<Shape> bvh = std::make_shared<BVH>(objects);
            _world = bvh;

            // build camera
            auto eye = Vector3(0.0, 0.0, 700);
            auto dir = Vector3(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            _camera = camera;
        }

        void Scene::testBuildCornelBox() {
            // For testing
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
            int lightIntensity = 15;
            Spectrum lightSpectrum = Spectrum(0.0);
            lightSpectrum.r(double(249.0) / 255.0 * lightIntensity);
            lightSpectrum.g(double(222.0) / 255.0 * lightIntensity);
            lightSpectrum.b(double(180.0) / 255.0 * lightIntensity);
            std::shared_ptr<Texture> lightAlbedo = std::make_shared<ConstantTexture>(lightSpectrum);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            // walls
            std::shared_ptr<Shape> leftWall = std::make_shared<YZWall>(-250, 250, -250, 250, -250, true,
                                                                       lambertLeft);
            std::shared_ptr<Shape> rightWall = std::make_shared<YZWall>(-250, 250, -250, 250, 250, false,
                                                                        lambertRight);
            std::shared_ptr<Shape> bottomWall = std::make_shared<ZXWall>(-250, 250, -250, 250, -250, true,
                                                                         lambertBottom);
            std::shared_ptr<Shape> topWall = std::make_shared<ZXWall>(-250, 250, -250, 250, 250, false,
                                                                      lambertTop);
            std::shared_ptr<Shape> frontWall = std::make_shared<XYWall>(-250, 250, -250, 250, -250, false,
                                                                        lambertFront);

            std::shared_ptr<Shape> glassSphere = std::make_shared<Sphere>(Vector3(75, -169, 0), 80, glass);

            std::shared_ptr<Shape> metalSphere = std::make_shared<Sphere>(Vector3(-125, -149, 100), 100, metal);

            // light
            std::shared_ptr<ShapeSampler> lightWall = std::make_shared<ZXWall>(-100, 100, -100, 100, 235, false,
                                                                               lambertTop);

            // build light
            std::shared_ptr<AreaLight> light = DiffuseAreaLight::buildDiffuseAreaLight(lightSpectrum, lightWall);
            _light = light;

            // objects
            std::vector<std::shared_ptr<Shape>> objects;
            objects.push_back(leftWall);
            objects.push_back(rightWall);
            objects.push_back(bottomWall);
            objects.push_back(topWall);
            objects.push_back(frontWall);
            objects.push_back(glassSphere);
            objects.push_back(metalSphere);
            objects.push_back(lightWall);

            // 给所有 object 赋予 id
            for (long long id = 0; id < objects.size(); id++) {
                objects[id]->setId(id);
            }

            // scene
            std::shared_ptr<Shape> bvh = std::make_shared<BVH>(objects);
            _world = bvh;

            // build camera
            auto eye = Vector3(0.0, 0.0, 700);
            auto dir = Vector3(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            _camera = camera;
        }

        void Scene::testAreaLight() {
            const int MODEL_SCALE = 5;
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
            double areaLightIntensity = 15;
            areaLightSpectrum.r(double(249.0) / 255.0 * areaLightIntensity);
            areaLightSpectrum.g(double(222.0) / 255.0 * areaLightIntensity);
            areaLightSpectrum.b(double(180.0) / 255.0 * areaLightIntensity);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
//            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(white, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            // walls
            std::shared_ptr<Shape> leftWall = std::make_shared<YZWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                       -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                       -0.5 * MODEL_SCALE, true,
                                                                       lambertLeft);
            std::shared_ptr<Shape> rightWall = std::make_shared<YZWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        0.5 * MODEL_SCALE, false,
                                                                        lambertRight);
            std::shared_ptr<Shape> bottomWall = std::make_shared<ZXWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                         -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                         -0.5 * MODEL_SCALE, true,
                                                                         lambertBottom);
            std::shared_ptr<Shape> topWall = std::make_shared<ZXWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                      -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                      0.5 * MODEL_SCALE, false,
                                                                      lambertTop);
            std::shared_ptr<Shape> frontWall = std::make_shared<XYWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        -0.5 * MODEL_SCALE, false,
                                                                        lambertFront);

            // light
            std::shared_ptr<ShapeSampler> lightWall = std::make_shared<ZXWall>(-0.2 * MODEL_SCALE, 0.2 * MODEL_SCALE,
                                                                               -0.2 * MODEL_SCALE, 0.2 * MODEL_SCALE,
                                                                               0.46 * MODEL_SCALE, false,
                                                                               lambertTop);

            std::shared_ptr<Shape> glassSphere = std::make_shared<Sphere>(
                    Vector3(0.25 * MODEL_SCALE, -0.338 * MODEL_SCALE, 0 * MODEL_SCALE), 0.16 * MODEL_SCALE, glass);

            std::shared_ptr<Shape> metalSphere = std::make_shared<Sphere>(
                    Vector3(-0.25 * MODEL_SCALE, -0.298 * MODEL_SCALE, 0.2 * MODEL_SCALE), 0.2 * MODEL_SCALE, metal);

            // build area light
            std::shared_ptr<AreaLight> light = DiffuseAreaLight::buildDiffuseAreaLight(areaLightSpectrum, lightWall);
            _light = light;

            // objects
            std::vector<std::shared_ptr<Shape>> objects;
            objects.push_back(leftWall);
            objects.push_back(rightWall);
            objects.push_back(bottomWall);
            objects.push_back(topWall);
            objects.push_back(frontWall);
            objects.push_back(lightWall);
            objects.push_back(glassSphere);
            objects.push_back(metalSphere);

            // 给所有 object 赋予 id
            for (long long id = 0; id < objects.size(); id++) {
                objects[id]->setId(id);
            }

            // scene
            std::shared_ptr<Shape> bvh = std::make_shared<BVH>(objects);
            _world = bvh;

            // build camera
            auto eye = Vector3(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            _camera = camera;
        }

        void Scene::testSpotLight() {
            const int MODEL_SCALE = 2;
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
            double areaLightIntensity = 20;
            spotLightSpectrum.r(double(249.0) / 255.0 * areaLightIntensity);
            spotLightSpectrum.g(double(222.0) / 255.0 * areaLightIntensity);
            spotLightSpectrum.b(double(180.0) / 255.0 * areaLightIntensity);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(white, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            // walls
            std::shared_ptr<Shape> leftWall = std::make_shared<YZWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                       -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                       -0.5 * MODEL_SCALE, true,
                                                                       lambertLeft);
            std::shared_ptr<Shape> rightWall = std::make_shared<YZWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        0.5 * MODEL_SCALE, false,
                                                                        lambertRight);
            std::shared_ptr<Shape> bottomWall = std::make_shared<ZXWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                         -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                         -0.5 * MODEL_SCALE, true,
                                                                         lambertBottom);
            std::shared_ptr<Shape> topWall = std::make_shared<ZXWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                      -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                      0.5 * MODEL_SCALE, false,
                                                                      lambertTop);
            std::shared_ptr<Shape> frontWall = std::make_shared<XYWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        -0.5 * MODEL_SCALE, false,
                                                                        lambertFront);

            // build point light
            std::shared_ptr<SpotLight> light = SpotLight::buildSpotLight(
                    Vector3(0 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0 * MODEL_SCALE), Vector3(0.0, -1, 0.0),
                    spotLightSpectrum);

            _light = light;

            // objects
            std::vector<std::shared_ptr<Shape>> objects;
            objects.push_back(leftWall);
            objects.push_back(rightWall);
            objects.push_back(bottomWall);
            objects.push_back(topWall);
            objects.push_back(frontWall);

            // 给所有 object 赋予 id
            for (long long id = 0; id < objects.size(); id++) {
                objects[id]->setId(id);
            }

            // scene
            std::shared_ptr<Shape> bvh = std::make_shared<BVH>(objects);
            _world = bvh;

            // build camera
            auto eye = Vector3(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            _camera = camera;
        }

        void Scene::testPointLight() {
            const int MODEL_SCALE = 5;
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
            double areaLightIntensity = 15;
            areaLightSpectrum.r(double(249.0) / 255.0 * areaLightIntensity);
            areaLightSpectrum.g(double(222.0) / 255.0 * areaLightIntensity);
            areaLightSpectrum.b(double(180.0) / 255.0 * areaLightIntensity);

            Spectrum pointLightSpectrum = Spectrum(0.0);
            double pointLightIntensity = 62500;
            pointLightSpectrum.r(double(249.0) / 255.0 * pointLightIntensity);
            pointLightSpectrum.g(double(222.0) / 255.0 * pointLightIntensity);
            pointLightSpectrum.b(double(180.0) / 255.0 * pointLightIntensity);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
//            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(white, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            // walls
            std::shared_ptr<Shape> leftWall = std::make_shared<YZWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                       -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                       -0.5 * MODEL_SCALE, true,
                                                                       lambertLeft);
            std::shared_ptr<Shape> rightWall = std::make_shared<YZWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        0.5 * MODEL_SCALE, false,
                                                                        lambertRight);
            std::shared_ptr<Shape> bottomWall = std::make_shared<ZXWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                         -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                         -0.5 * MODEL_SCALE, true,
                                                                         lambertBottom);
            std::shared_ptr<Shape> topWall = std::make_shared<ZXWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                      -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                      0.5 * MODEL_SCALE, false,
                                                                      lambertTop);
            std::shared_ptr<Shape> frontWall = std::make_shared<XYWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        -0.5 * MODEL_SCALE, false,
                                                                        lambertFront);

            std::shared_ptr<Shape> glassSphere = std::make_shared<Sphere>(
                    Vector3(0.25 * MODEL_SCALE, -0.338 * MODEL_SCALE, 0 * MODEL_SCALE), 0.16 * MODEL_SCALE, glass);

            std::shared_ptr<Shape> metalSphere = std::make_shared<Sphere>(
                    Vector3(-0.25 * MODEL_SCALE, -0.298 * MODEL_SCALE, 0.2 * MODEL_SCALE), 0.2 * MODEL_SCALE, metal);

            // build point light
            std::shared_ptr<PointLight> light = PointLight::buildPointLight(
                    Vector3(0 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0 * MODEL_SCALE), areaLightSpectrum);
            _light = light;

            // objects
            std::vector<std::shared_ptr<Shape>> objects;
            objects.push_back(leftWall);
            objects.push_back(rightWall);
            objects.push_back(bottomWall);
            objects.push_back(topWall);
            objects.push_back(frontWall);
            objects.push_back(glassSphere);
            objects.push_back(metalSphere);

            // 给所有 object 赋予 id
            for (long long id = 0; id < objects.size(); id++) {
                objects[id]->setId(id);
            }

            // scene
            std::shared_ptr<Shape> bvh = std::make_shared<BVH>(objects);
            _world = bvh;

            // build camera
            auto eye = Vector3(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            _camera = camera;
        }

        void Scene::testBuildEmptyCornelBox() {
            const int MODEL_SCALE = 7;
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
            double areaLightIntensity = 15;
            areaLightSpectrum.r(double(249.0) / 255.0 * areaLightIntensity);
            areaLightSpectrum.g(double(222.0) / 255.0 * areaLightIntensity);
            areaLightSpectrum.b(double(180.0) / 255.0 * areaLightIntensity);

            Spectrum pointLightSpectrum = Spectrum(0.0);
            double pointLightIntensity = 62500;
            pointLightSpectrum.r(double(249.0) / 255.0 * pointLightIntensity);
            pointLightSpectrum.g(double(222.0) / 255.0 * pointLightIntensity);
            pointLightSpectrum.b(double(180.0) / 255.0 * pointLightIntensity);

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
//            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(white, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            // walls
            std::shared_ptr<Shape> leftWall = std::make_shared<YZWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                       -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                       -0.5 * MODEL_SCALE, true,
                                                                       lambertLeft);
            std::shared_ptr<Shape> rightWall = std::make_shared<YZWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        0.5 * MODEL_SCALE, false,
                                                                        lambertRight);
            std::shared_ptr<Shape> bottomWall = std::make_shared<ZXWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                         -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                         -0.5 * MODEL_SCALE, true,
                                                                         lambertBottom);
            std::shared_ptr<Shape> topWall = std::make_shared<ZXWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                      -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                      0.5 * MODEL_SCALE, false,
                                                                      lambertTop);
            std::shared_ptr<Shape> frontWall = std::make_shared<XYWall>(-0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        -0.5 * MODEL_SCALE, 0.5 * MODEL_SCALE,
                                                                        -0.5 * MODEL_SCALE, false,
                                                                        lambertFront);

            // build point light
            std::shared_ptr<PointLight> light = PointLight::buildPointLight(
                    Vector3(0 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0 * MODEL_SCALE), areaLightSpectrum);
            _light = light;

            // objects
            std::vector<std::shared_ptr<Shape>> objects;
            objects.push_back(leftWall);
            objects.push_back(rightWall);
            objects.push_back(bottomWall);
            objects.push_back(topWall);
            objects.push_back(frontWall);

            // 给所有 object 赋予 id
            for (long long id = 0; id < objects.size(); id++) {
                objects[id]->setId(id);
            }

            // scene
            std::shared_ptr<Shape> bvh = std::make_shared<BVH>(objects);
            _world = bvh;

            // build camera
            auto eye = Vector3(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            _camera = camera;
        }

        bool Scene::hit(const Ray &ray, SurfaceInteraction &hitRecord) {
            return _world->insect(ray, hitRecord, 0.001, infinity);
        }
    }
}