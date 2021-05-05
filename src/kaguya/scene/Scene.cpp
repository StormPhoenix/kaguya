//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/core/light/Light.h>
#include <kaguya/core/light/PointLight.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/scene/meta/Triangle.h>
#include <kaguya/scene/Geometry.h>
#include <kaguya/scene/accumulation/BVH.h>
#include <kaguya/scene/meta/Sphere.h>
#include <kaguya/scene/TriangleMesh.h>
#include <kaguya/material/Dielectric.h>
#include <kaguya/material/Lambertian.h>
#include <kaguya/material/Metal.h>
#include <kaguya/material/SubsurfaceMaterial.h>
#include <kaguya/material/texture/ConstantTexture.h>
#include <kaguya/material/texture/SphericalMapping2D.h>
#include <kaguya/utils/ObjLoader.h>
#include <kaguya/core/medium/GridDensityMedium.h>

#include <iostream>


namespace kaguya {
    namespace scene {
        using namespace kaguya::material;
        using namespace kaguya::material::texture;
        using namespace kaguya::scene::acc;
        using namespace kaguya::scene::meta;
        using namespace kaguya::core;

        Spectrum blue_spectrum(0.);
        Spectrum total_white_spectrum(1.0);

        /*
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
         */

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

/*
        std::vector<std::shared_ptr<Geometry>>
        Scene::testBottomPlane(const std::shared_ptr<Material> material,
                              const std::shared_ptr<Medium> insideMedium,
                              const std::shared_ptr<Medium> outsideMedium) {
            const Vector3F a1(10.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, -10.5 * MODEL_SCALE);
            const Vector3F a2(-10.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, -10.5 * MODEL_SCALE);
            const Vector3F a3(-10.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, 10.5 * MODEL_SCALE);
            const Vector3F a4(10.5 * MODEL_SCALE, -0.5 * MODEL_SCALE, 10.5 * MODEL_SCALE);

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


        */

        std::vector<std::shared_ptr<Geometry>>
        Scene::testTopAreaLight(const Spectrum spectrum, const std::shared_ptr<Medium> medium,
                                std::vector<std::shared_ptr<Light>> &lights, const std::shared_ptr<Material> material) {
            Float top = 0.46;
            const Vector3F a1(0.2 * MODEL_SCALE, top * MODEL_SCALE, -0.2 * MODEL_SCALE);
            const Vector3F a2(-0.2 * MODEL_SCALE, top * MODEL_SCALE, -0.2 * MODEL_SCALE);
            const Vector3F a3(-0.2 * MODEL_SCALE, top * MODEL_SCALE, 0.2 * MODEL_SCALE);
            const Vector3F a4(0.2 * MODEL_SCALE, top * MODEL_SCALE, 0.2 * MODEL_SCALE);

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
                                                                                 Vector2F(1, 1), Vector2F(0, 0),
                                                                                 Vector2F(1, 0));
            std::shared_ptr<Geometry> gt1 = std::make_shared<Geometry>(tri1, material, insideMedium, outsideMedium,
                                                                       nullptr);

            std::shared_ptr<meta::Shape> tri2 = std::make_shared<meta::Triangle>(a1, a2, a3,
                                                                                 n, n, n,
                                                                                 Vector2F(1, 1), Vector2F(0, 1),
                                                                                 Vector2F(0, 0));
            std::shared_ptr<Geometry> gt2 = std::make_shared<Geometry>(tri2, material, insideMedium, outsideMedium,
                                                                       nullptr);
            std::vector<std::shared_ptr<Geometry>> v;
            v.push_back(gt1);
            v.push_back(gt2);
            return v;
        }

/*
        std::shared_ptr<Aggregation> Scene::testSubsurfaceBunny(const std::shared_ptr<Material> material,
                                                      const std::shared_ptr<Medium> inside,
                                                      const std::shared_ptr<Medium> outside,
                                                      const std::shared_ptr<AreaLight> areaLight) {
            std::vector<Vertex> bunnyVertexes = kaguya::utils::ObjLoader::loadModel("./resource/objects/bunny.obj");

            Float scale = 0.4 * MODEL_SCALE;
            Matrix4F mat(1.0f);
            mat = TRANSLATE(mat, Vector3F(0, -scale / 0.8, 0));
            mat = SCALE(mat, Vector3F(scale, scale, scale));
            std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>(mat);
            std::shared_ptr<Aggregation> bunny = std::make_shared<TriangleMesh>(bunnyVertexes, material, inside,
                                                                                outside,
                                                                                areaLight, transformMatrix);
            return bunny;
        }
        */
        std::shared_ptr<AreaLight> Scene::testDiffuseAreaLight(const Spectrum &spectrum,
                                                               const std::shared_ptr<Geometry> geometry,
                                                               const std::shared_ptr<Medium> inside,
                                                               const std::shared_ptr<Medium> outside,
                                                               bool singleSide) {
            std::shared_ptr<AreaLight> light = std::make_shared<DiffuseAreaLight>(
                    spectrum, geometry->getShape(), MediumBoundary(inside.get(), outside.get()), singleSide);
            geometry->setAreaLight(light);
            return light;
        }

        std::shared_ptr<Scene> Scene::innerSceneWithAreaLight() {
            // For testing
            // albedos
            // total white
            Spectrum totalWhiteSpectrum = Spectrum(1.0);
            std::shared_ptr<Texture<Spectrum>> totalWhite = std::make_shared<ConstantTexture<Spectrum>>(
                    totalWhiteSpectrum);

            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture<Spectrum>> white = std::make_shared<ConstantTexture<Spectrum>>(whiteSpectrum);

            // red
            Spectrum redSpectrum = Spectrum(0.0);
            redSpectrum.r(0.65);
            redSpectrum.g(0.05);
            redSpectrum.b(0.05);
            std::shared_ptr<Texture<Spectrum>> red = std::make_shared<ConstantTexture<Spectrum>>(redSpectrum);


            // green
            Spectrum greenSpectrum = Spectrum(0.0);
            greenSpectrum.r(0.12);
            greenSpectrum.g(0.45);
            greenSpectrum.b(0.15);
            std::shared_ptr<Texture<Spectrum>> green = std::make_shared<ConstantTexture<Spectrum>>(greenSpectrum);

            // blue
            std::shared_ptr<Texture<Spectrum>> blue = std::make_shared<ConstantTexture<Spectrum>>(blue_spectrum);

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

            // Build metal sphere
            Vector3F metalSphereCenter = Vector3F(-0.25 * MODEL_SCALE, -0.298 * MODEL_SCALE, 0.2 * MODEL_SCALE);
            Float metalSphereRadius =   0.2 * MODEL_SCALE;
            Matrix4F metalSphereMat(1.0f);
            metalSphereMat = TRANSLATE(metalSphereMat, metalSphereCenter);
            metalSphereMat = SCALE(metalSphereMat, Vector3F(metalSphereRadius, metalSphereRadius, metalSphereRadius));
            std::shared_ptr<Transform> metalSphereTransform = std::make_shared<Transform>(metalSphereMat);
            std::shared_ptr<Shape> metalSphereShape = std::make_shared<meta::Sphere>(metalSphereTransform);
            std::shared_ptr<Geometry> metalSphere = std::make_shared<Geometry>(metalSphereShape, metal,
                                                                               nullptr, nullptr);

            // Build glass sphere
            Vector3F glassSphereCenter = Vector3F(0.25 * MODEL_SCALE, -0.338 * MODEL_SCALE, 0 * MODEL_SCALE);
            Float glassSphereRadius =   0.16 * MODEL_SCALE;
            Matrix4F glassSphereMat(1.0f);
            glassSphereMat = TRANSLATE(glassSphereMat, glassSphereCenter);
            glassSphereMat = SCALE(glassSphereMat, Vector3F(glassSphereRadius, glassSphereRadius, glassSphereRadius));
            std::shared_ptr<Transform> glassSphereTransform = std::make_shared<Transform>(glassSphereMat);
            std::shared_ptr<Shape> glassSphereShape = std::make_shared<meta::Sphere>(glassSphereTransform);
            std::shared_ptr<Geometry> glassSphere = std::make_shared<Geometry>(glassSphereShape, glass,
                                                                               nullptr, nullptr);

            objects.push_back(glassSphere);
            objects.push_back(metalSphere);

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build area light
            Matrix4F lightToWorldMat(1.0);
            lightToWorldMat = TRANSLATE(lightToWorldMat,
                                        Vector3F(0 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0 * MODEL_SCALE));
            Transform::Ptr lightToWorld = std::make_shared<Transform>(lightToWorldMat);


            auto lightWall = testTopAreaLight(lightSpectrum, airMedium, scene->_lights, lambertTop);
            objects.insert(objects.end(), lightWall.begin(), lightWall.end());

            // scene
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(objects);
            scene->_world = bvh;

            // build camera
            auto eye = Vector3F(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3F(0.0f, 0.0f, -1.0f);
            auto up = Vector3F(0.0f, 1.0f, 0.0f);

            Transform cameraToWorld = Transform::lookAt(eye, dir, up);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(cameraToWorld.ptr(), 60);
            scene->_camera = camera;

            scene->_sceneName = "inner-scene-with-area-light.png";

            return scene;
        }


        std::shared_ptr<Scene> Scene::innerSceneWithPointLight() {
            // For testing
            // albedos
            // total white
            Spectrum totalWhiteSpectrum = Spectrum(1.0);
            std::shared_ptr<Texture<Spectrum>> totalWhite = std::make_shared<ConstantTexture<Spectrum>>(
                    totalWhiteSpectrum);

            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture<Spectrum>> white = std::make_shared<ConstantTexture<Spectrum>>(whiteSpectrum);

            // red
            Spectrum redSpectrum = Spectrum(0.0);
            redSpectrum.r(0.65);
            redSpectrum.g(0.05);
            redSpectrum.b(0.05);
            std::shared_ptr<Texture<Spectrum>> red = std::make_shared<ConstantTexture<Spectrum>>(redSpectrum);


            // green
            Spectrum greenSpectrum = Spectrum(0.0);
            greenSpectrum.r(0.12);
            greenSpectrum.g(0.45);
            greenSpectrum.b(0.15);
            std::shared_ptr<Texture<Spectrum>> green = std::make_shared<ConstantTexture<Spectrum>>(greenSpectrum);

            // blue
            std::shared_ptr<Texture<Spectrum>> blue = std::make_shared<ConstantTexture<Spectrum>>(blue_spectrum);

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


            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build point light
            Matrix4F lightToWorldMat(1.0);
            lightToWorldMat = TRANSLATE(lightToWorldMat,
                                        Vector3F(0 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0 * MODEL_SCALE));
            Transform::Ptr lightToWorld = std::make_shared<Transform>(lightToWorldMat);
            std::shared_ptr<PointLight> light = std::make_shared<PointLight>(lightSpectrum,
                                                                             lightToWorld,
                                                                             MediumBoundary(nullptr, nullptr));
            scene->_lights.push_back(light);

            // scene
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(objects);
            scene->_world = bvh;

            // build camera
            auto eye = Vector3F(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3F(0.0f, 0.0f, -1.0f);
            auto up = Vector3F(0.0f, 1.0f, 0.0f);

            Transform cameraToWorld = Transform::lookAt(eye, dir, up);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(cameraToWorld.ptr(), 60);
            scene->_camera = camera;

            scene->_sceneName = "inner-scene-with-point-light.png";

            return scene;
        }

        std::shared_ptr<Scene> Scene::innerSceneBunnyWithPointLight() {
            // For testing
            // albedos
            // total white
            Spectrum totalWhiteSpectrum = Spectrum(1.0);
            std::shared_ptr<Texture<Spectrum>> totalWhite = std::make_shared<ConstantTexture<Spectrum>>(
                    totalWhiteSpectrum);

            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture<Spectrum>> white = std::make_shared<ConstantTexture<Spectrum>>(whiteSpectrum);

            // red
            Spectrum redSpectrum = Spectrum(0.0);
            redSpectrum.r(0.65);
            redSpectrum.g(0.05);
            redSpectrum.b(0.05);
            std::shared_ptr<Texture<Spectrum>> red = std::make_shared<ConstantTexture<Spectrum>>(redSpectrum);


            // green
            Spectrum greenSpectrum = Spectrum(0.0);
            greenSpectrum.r(0.12);
            greenSpectrum.g(0.45);
            greenSpectrum.b(0.15);
            std::shared_ptr<Texture<Spectrum>> green = std::make_shared<ConstantTexture<Spectrum>>(greenSpectrum);

            // blue
            std::shared_ptr<Texture<Spectrum>> blue = std::make_shared<ConstantTexture<Spectrum>>(blue_spectrum);

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

            // load model
            std::vector<Vector3F> vertices;
            std::vector<Normal3F> normals;
            std::vector<Point2F> texcoords;
            std::vector<TriMesh::TriIndex> indics;
            bool good = utils::io::ObjLoader::loadObj("./resource/objects/bunny2.obj", vertices, normals, texcoords,
                                                          indics);
            ASSERT(good, "Load *.obj model failed: /resource/objects/bunny2.obj ");

            Transform::Ptr toWorld = nullptr;
            Float scale = 0.2 * MODEL_SCALE;
            Matrix4F bunnyMat(1.0f);
            bunnyMat = TRANSLATE(bunnyMat, Vector3F(scale / 4.5, -scale / 1.0, scale / 6.0));
            bunnyMat = SCALE(bunnyMat, Vector3F(scale, scale, scale));
            toWorld = std::make_shared<Transform>(bunnyMat);
            TriMesh::Ptr tris = std::make_shared<TriangleMesh>(vertices, normals, texcoords, indics, toWorld);

            for (auto it = tris->triangles()->begin(); it != tris->triangles()->end(); it++) {
                Geometry::Ptr geometry = std::make_shared<Geometry>(*it, glass);
                objects.push_back(geometry);
            }

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build point light
            Matrix4F lightToWorldMat(1.0);
            lightToWorldMat = TRANSLATE(lightToWorldMat,
                                        Vector3F(0 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0 * MODEL_SCALE));
            Transform::Ptr lightToWorld = std::make_shared<Transform>(lightToWorldMat);
            std::shared_ptr<PointLight> light = std::make_shared<PointLight>(lightSpectrum,
                                                                             lightToWorld,
                                                                             MediumBoundary(nullptr, nullptr));
            scene->_lights.push_back(light);

            // scene
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(objects);
            scene->_world = bvh;

            // build camera
            auto eye = Vector3F(0.0 * MODEL_SCALE, 0.0 * MODEL_SCALE, 1.4 * MODEL_SCALE);
            auto dir = Vector3F(0.0f, 0.0f, -1.0f);
            auto up = Vector3F(0.0f, 1.0f, 0.0f);

            Transform cameraToWorld = Transform::lookAt(eye, dir, up);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(cameraToWorld.ptr(), 60);
            scene->_camera = camera;

            scene->_sceneName = "inner-scene-bunny-with-point-light.png";

            return scene;
        }

        /*
        std::shared_ptr<Scene> Scene::sceneBunnySubsurfaceWithAreaLight() {
            // For testing
            // albedos
            // total white
            Spectrum totalWhiteSpectrum = Spectrum(1.0);
            std::shared_ptr<Texture<Spectrum>> totalWhite = std::make_shared<ConstantTexture<Spectrum>>(
                    totalWhiteSpectrum);

            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture<Spectrum>> white = std::make_shared<ConstantTexture<Spectrum>>(whiteSpectrum);

            // red
            Spectrum redSpectrum = Spectrum(0.0);
            redSpectrum.r(0.65);
            redSpectrum.g(0.05);
            redSpectrum.b(0.05);
            std::shared_ptr<Texture<Spectrum>> red = std::make_shared<ConstantTexture<Spectrum>>(redSpectrum);


            // green
            Spectrum greenSpectrum = Spectrum(0.0);
            greenSpectrum.r(0.12);
            greenSpectrum.g(0.45);
            greenSpectrum.b(0.15);
            std::shared_ptr<Texture<Spectrum>> green = std::make_shared<ConstantTexture<Spectrum>>(greenSpectrum);

            // blue
            std::shared_ptr<Texture<Spectrum>> blue = std::make_shared<ConstantTexture<Spectrum>>(blue_spectrum);



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
            std::vector<std::shared_ptr<Geometry>> bottomWall = testBottomPlane(lambertBottom, airMedium, airMedium);
            objects.insert(objects.end(), bottomWall.begin(), bottomWall.end());

            // Subsurface Material
            Spectrum albedoEff(0.99);
            Spectrum mft(0.01);
            Float g = 0.0f;
            Float theta = 1.33f;
            std::shared_ptr<Material> subsurfaceMaterial = std::make_shared<SubsurfaceMaterial>(albedoEff, mft, g, theta);

            // load model
//            std::shared_ptr<Intersectable> bunny = testBunny(lambertFront, bunnyMedium, airMedium, nullptr);
            std::shared_ptr<Intersectable> bunny = testSubsurfaceBunny(subsurfaceMaterial, bunnyMedium, airMedium, nullptr);
//            std::shared_ptr<Intersectable> bunny = testBunny(glass, bunnyMedium, airMedium, nullptr);
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
            camera->setResolutionWidth(Config::Camera::width);
            camera->setResolutionHeight(Config::Camera::height);
            scene->_camera = camera;
            scene->_sceneName = "bunny-subsurface-material-with-area-light";

            return scene;
        }
         */

        void Scene::setCamera(std::shared_ptr<Camera> camera) {
            _camera = camera;
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