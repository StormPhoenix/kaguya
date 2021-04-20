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
#include <kaguya/scene/meta/Triangle.h>
#include <kaguya/scene/aggregation/TriangleMesh.h>
#include <kaguya/material/Dielectric.h>
#include <kaguya/material/Lambertian.h>
#include <kaguya/material/Metal.h>
#include <kaguya/material/SubsurfaceMaterial.h>
#include <kaguya/material/texture/ConstantTexture.h>
#include <kaguya/material/texture/ImageTexture.h>
#include <kaguya/material/texture/UVMapping2D.h>
#include <kaguya/material/texture/SphericalMapping2D.h>
#include <kaguya/utils/ObjLoader.h>
#include <kaguya/core/medium/GridDensityMedium.h>
#include <kaguya/core/light/EnvironmentLight.h>

#ifdef TEST_SCENE

#include <cstring>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>

#endif

namespace kaguya {
    namespace scene {
        using namespace kaguya::material;
        using namespace kaguya::material::texture;
        using namespace kaguya::scene::acc;
        using namespace kaguya::scene::meta;
        using namespace kaguya::core;

#ifdef TEST_SCENE

        // Image texture UVMapping
        std::shared_ptr<Texture<Spectrum>> imageTexture1 = nullptr;
        Spectrum blue_spectrum(0.);
        Spectrum area_light_spectrum(0.);
        Spectrum total_white_spectrum(1.0);

        /*
        std::shared_ptr<Geometry>
        buildSphere(const Vector3F &center, const Float &radius, std::shared_ptr<Material> material) {
            // Transform
            Matrix4F sphereMat(1.0f);
            sphereMat = TRANSLATE(sphereMat, center);
            sphereMat = SCALE(sphereMat, Vector3F(radius, radius, radius));
            std::shared_ptr<Transform> metalSphereTransform = std::make_shared<Transform>(sphereMat);

            std::shared_ptr<Shape> metalSphereShape = std::make_shared<meta::Sphere>(metalSphereTransform);
            std::shared_ptr<Geometry> metalSphere = std::make_shared<Geometry>(metalSphereShape, material,
                                                                               nullptr, nullptr);
            return metalSphere;
        }

        std::shared_ptr<Geometry> buildMetalSphere(const Vector3F &center, const Float &radius) {
            std::shared_ptr<Material> metal = std::make_shared<Metal>();
            return buildSphere(center, radius, metal);
        }

        std::shared_ptr<Geometry> buildGlassSphere(const Vector3F &center, const Float &radius) {
            std::shared_ptr<Texture<Spectrum>> totalWhite = std::make_shared<ConstantTexture<Spectrum>>(
                    total_white_spectrum);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            return buildSphere(center, radius, glass);
        }

        std::shared_ptr<Geometry> buildLambSphere(const Vector3F &center, const Float &radius) {
            std::shared_ptr<Texture<Spectrum>> totalWhite = std::make_shared<ConstantTexture<Spectrum>>(
                    total_white_spectrum);
            std::shared_ptr<Material> lamb = std::make_shared<Lambertian>(totalWhite);
            return buildSphere(center, radius, lamb);
        }

        std::shared_ptr<Geometry> buildLambSphereImageTexture(const Vector3F &center, const Float &radius) {
            // Transform
            Matrix4F sphereMat(1.0f);
            sphereMat = TRANSLATE(sphereMat, center);
            sphereMat = SCALE(sphereMat, Vector3F(radius, radius, radius));
            std::shared_ptr<Transform> transform = std::make_shared<Transform>(sphereMat);
            std::shared_ptr<TextureMapping2D> textureMapping2D = std::make_shared<SphericalMapping2D>(transform);

            std::shared_ptr<Texture<Spectrum>> imageTextureLocal = std::make_shared<ImageTexture<Spectrum>>("./resource/texture/texture1.jpg", textureMapping2D);
            std::shared_ptr<Material> lamb = std::make_shared<Lambertian>(imageTextureLocal);
            return buildSphere(center, radius, lamb);
        }

        void Scene::initSceneComponents() {

            imageTexture1 = std::make_shared<ImageTexture<Spectrum>>(
                    "./resource/texture/texture1.jpg");

            // blue
            blue_spectrum = Spectrum(0.0);
            blue_spectrum.r(0);
            blue_spectrum.g(0);
            blue_spectrum.b(1.0);

            // light spectrum
            Float areaLightIntensity = 125;
            area_light_spectrum.r(Float(249.0) / 255.0 * areaLightIntensity);
            area_light_spectrum.g(Float(222.0) / 255.0 * areaLightIntensity);
            area_light_spectrum.b(Float(180.0) / 255.0 * areaLightIntensity);

        }

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

        std::shared_ptr<Scene> Scene::sceneCornelBoxXml() {
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();
            //
//            Matrix4F floorMat =
            // No matrix
            Texture<Spectrum>::Ptr texture = std::make_shared<ConstantTexture<Spectrum>>(Spectrum(0.73));
            Material::Ptr material = std::make_shared<Lambertian>(texture);
            Spectrum r(15);

            // Floor shape
            Normal3F normal = {0, 1, 0};
            Point2F uv = {0, 0};
            Vector3F a1 = {-0.99999994, 0.0000000874227978, -1};
            Vector3F a2 = {-1, 0.0000000874227978, 0.99999994};
            Vector3F a3 = {0.99999994, -0.0000000874227978, 1};
            Shape::Ptr tri1 = std::make_shared<Triangle>(a1, a2, a3,
                                                         normal, normal, normal,
                                                         uv, uv, uv);
            Geometry::Ptr gtri1 = std::make_shared<Geometry>(tri1, material, nullptr, nullptr, nullptr);

            Vector3F b1 = {0.99999994, -0.0000000874227978, 1};
            Vector3F b2 = {1, -0.0000000874227978, -0.99999994};
            Vector3F b3 = {-0.99999994, 0.0000000874227978, -1};
            Triangle::Ptr tri2 = std::make_shared<Triangle>(b1, b2, b3,
                                                            normal, normal, normal,
                                                            uv, uv, uv);
            Geometry::Ptr gtri2 = std::make_shared<Geometry>(tri2, material, nullptr, nullptr, nullptr);


            // Light shape
            Vector3F la1 = {-0.23999998, 1.98000002, -0.220000014};
            Vector3F la2 = {0.230000019, 1.98000002, -0.219999984};
            Vector3F la3 = {0.229999989, 1.98000002, 0.160000011};


            Triangle::Ptr l1 = std::make_shared<Triangle>(la1, la2, la3,
                                                          -normal, -normal, -normal,
                                                          uv, uv, uv);
            AreaLight::Ptr areaL1 = std::make_shared<DiffuseAreaLight>(r, l1, nullptr, true);
            scene->addLight(areaL1);
            Geometry::Ptr gl1 = std::make_shared<Geometry>(l1, material, nullptr, nullptr, areaL1);


            Vector3F lb1 = {0.229999989, 1.98000002, 0.160000011};
            Vector3F lb2 = {-0.24000001, 1.98000002, 0.159999982};
            Vector3F lb3 = {-0.23999998, 1.98000002, -0.220000014};
            Triangle::Ptr l2 = std::make_shared<Triangle>(lb1, lb2, lb3,
                                                          -normal, -normal, -normal,
                                                          uv, uv, uv);
            AreaLight::Ptr areaL2 = std::make_shared<DiffuseAreaLight>(r, l2, nullptr, true);
            scene->addLight(areaL2);
            Geometry::Ptr gl2 = std::make_shared<Geometry>(l2, material, nullptr, nullptr, areaL2);

            // Shapes
            std::vector<Intersectable::Ptr> shapes;
            shapes.push_back(gtri1);
            shapes.push_back(gtri2);
            shapes.push_back(gl1);
            shapes.push_back(gl2);

            // Camera
            Matrix4F cMat;
            cMat[0][0] = -1;
            cMat[1][0] = 0;
            cMat[2][0] = 0;
            cMat[3][0] = 0;

            cMat[0][1] = 0;
            cMat[1][1] = 1;
            cMat[2][1] = 0;
            cMat[3][1] = 1;

            cMat[0][2] = 0;
            cMat[1][2] = 0;
            cMat[2][2] = -1;
            cMat[3][2] = 6.8;

            cMat[0][3] = 0;
            cMat[1][3] = 0;
            cMat[2][3] = 0;
            cMat[3][3] = 1;
            Transform::Ptr transform = std::make_shared<Transform>(cMat);

            Camera::Ptr camera = std::make_shared<Camera>(transform, 19.5);
            scene->_camera = camera;

            // Build
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(shapes);
            scene->_world = bvh;
            scene->_sceneName = "compare";

            return scene;
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

        std::vector<std::shared_ptr<Geometry>>
        Scene::testTopAreaLightByTime(const Spectrum spectrum, const std::shared_ptr<Medium> medium,
                                      std::vector<std::shared_ptr<Light>> &lights,
                                      const std::shared_ptr<Material> material,
                                      Float t) {

            const Float baseLeft = -2.0;

            const Vector3F a1((baseLeft + 0.4 + 0.02 * t) * MODEL_SCALE, 0.55 * MODEL_SCALE, -0.2 * MODEL_SCALE);
            const Vector3F a2((baseLeft + 0.02 * t) * MODEL_SCALE, 0.55 * MODEL_SCALE, -0.2 * MODEL_SCALE);
            const Vector3F a3((baseLeft + 0.02 * t) * MODEL_SCALE, 0.55 * MODEL_SCALE, 0.2 * MODEL_SCALE);
            const Vector3F a4((baseLeft + 0.4 + 0.02 * t) * MODEL_SCALE, 0.55 * MODEL_SCALE, 0.2 * MODEL_SCALE);

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

 */
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

        std::shared_ptr<AreaLight> Scene::testDiffuseAreaLight(const Spectrum &spectrum,
                                                               const std::shared_ptr<Geometry> geometry,
                                                               const std::shared_ptr<Medium> inside,
                                                               const std::shared_ptr<Medium> outside,
                                                               bool singleSide) {
            std::shared_ptr<AreaLight> light = DiffuseAreaLight::buildDiffuseAreaLight(
                    spectrum, geometry->getShape(), MediumBoundary(inside.get(), outside.get()), singleSide);
            geometry->setAreaLight(light);
            return light;
        }

        std::shared_ptr<Scene> Scene::sceneSmoke() {
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

            std::shared_ptr<Texture<Spectrum>> blue = std::make_shared<ConstantTexture<Spectrum>>(blue_spectrum);




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
            auto lightWall = testTopAreaLight(area_light_spectrum, airMedium, scene->_lights, lambertTop);
            objects.insert(objects.end(), lightWall.begin(), lightWall.end());

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
            scene->_sceneName = "smoke-with-area-light";

            return scene;
        }

        std::shared_ptr<Scene> Scene::sceneTwoBox() {
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
            std::shared_ptr<Material> lambertFront2 = std::make_shared<Lambertian>(imageTexture1);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            // objects
            std::vector<std::shared_ptr<Intersectable>> objects;

            std::shared_ptr<Medium> airMedium = nullptr;

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

            std::vector<std::shared_ptr<Geometry>> frontWall = testFrontWall(lambertFront2, airMedium, airMedium);
            objects.insert(objects.end(), frontWall.begin(), frontWall.end());

            Float scale = 0.25 * MODEL_SCALE;
            Matrix4F mat(1.0f);
            mat = TRANSLATE(mat, Vector3F(-1.3 * scale, -2 * scale, -0.5 * scale));
            mat = ROTATE(mat, Float(30.0), Vector3F(0, 1, 0));
            mat = SCALE(mat, Vector3F(1.2 * scale, 2.5 * scale, 1.2 * scale));
            std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>(mat);

            Box box1 = Box(metal, airMedium, airMedium, transformMatrix);
            std::vector<std::shared_ptr<Intersectable>> boxes1 = box1.aggregation();
            objects.insert(objects.end(), boxes1.begin(), boxes1.end());

            Matrix4F mat1(1.0f);
            mat1 = TRANSLATE(mat1, Vector3F(0.1 * scale, -2 * scale, 0.5 * scale));
            mat1 = ROTATE(mat1, Float(5.0), Vector3F(0, 1, 0));
            mat1 = SCALE(mat1, Vector3F(scale, scale, scale));
            std::shared_ptr<Transform> transformMatrix1 = std::make_shared<Transform>(mat1);
            Box box2 = Box(lambertFront, airMedium, airMedium, transformMatrix1);
            std::vector<std::shared_ptr<Intersectable>> boxes2 = box2.aggregation();
            objects.insert(objects.end(), boxes2.begin(), boxes2.end());

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build area light
            auto lightWall = testTopAreaLight(lightSpectrum, airMedium, scene->_lights, lambertTop);
            objects.insert(objects.end(), lightWall.begin(), lightWall.end());

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
            scene->_sceneName = "scene-two-boxes";

            return scene;
        }

        std::shared_ptr<Scene> Scene::cornelBoxWater() {
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
            camera->setResolutionWidth(Config::Camera::width);
            camera->setResolutionHeight(Config::Camera::height);
            scene->_camera = camera;
            scene->_sceneName = "cornell-box-with-water";

            return scene;
        }
         */

        /*

        std::shared_ptr<Scene> Scene::sceneDeskAndBunny() {
            // white
            Spectrum whiteSpectrum = Spectrum(0.0);
            whiteSpectrum.r(0.73);
            whiteSpectrum.g(0.73);
            whiteSpectrum.b(0.73);
            std::shared_ptr<Texture<Spectrum>> white = std::make_shared<ConstantTexture<Spectrum>>(whiteSpectrum);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);

            std::shared_ptr<TextureMapping2D> textureMapping = std::make_shared<UVMapping2D>();
            std::shared_ptr<Texture<Spectrum>> imageTextureLocal = std::make_shared<ImageTexture<Spectrum>>(
                    "./resource/texture/texture4.jpg", textureMapping);

            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(imageTextureLocal);

            const float planeWidthHeightRatio = 780.0 / 1040.0;

            const Vector3F a1(2 * MODEL_SCALE, -0.5 * MODEL_SCALE, -2 * planeWidthHeightRatio * MODEL_SCALE);
            const Vector3F a2(-2 * MODEL_SCALE, -0.5 * MODEL_SCALE, -2 * planeWidthHeightRatio * MODEL_SCALE);
            const Vector3F a3(-2 * MODEL_SCALE, -0.5 * MODEL_SCALE, 2 * planeWidthHeightRatio * MODEL_SCALE);
            const Vector3F a4(2 * MODEL_SCALE, -0.5 * MODEL_SCALE, 2 * planeWidthHeightRatio * MODEL_SCALE);

            const Normal3F n(0, 1, 0);

            std::shared_ptr<meta::Shape> tri1 = std::make_shared<meta::Triangle>(a1, a3, a4,
                                                                                 n, n, n,
                                                                                 Point2F(1, 0), Point2F(0, 1),
                                                                                 Point2F(0, 0));
            std::shared_ptr<Geometry> gt1 = std::make_shared<Geometry>(tri1, lambertBottom, nullptr, nullptr,
                                                                       nullptr);

            std::shared_ptr<meta::Shape> tri2 = std::make_shared<meta::Triangle>(a1, a2, a3,
                                                                                 n, n, n,
                                                                                 Point2F(1, 0), Point2F(1, 1),
                                                                                 Point2F(0, 1));
            std::shared_ptr<Geometry> gt2 = std::make_shared<Geometry>(tri2, lambertBottom, nullptr, nullptr,
                                                                       nullptr);
            std::vector<std::shared_ptr<Geometry>> desk;
            desk.push_back(gt1);
            desk.push_back(gt2);

            // objects
            std::vector<std::shared_ptr<Intersectable>> objects;
            objects.insert(objects.end(), desk.begin(), desk.end());

            // load model
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            Float scale = 0.4 * MODEL_SCALE;
            Matrix4F mat(1.0f);
            mat = TRANSLATE(mat, Vector3F(scale * 1.2, -scale / 0.8, 0));
            mat = SCALE(mat, Vector3F(scale, scale, scale));
            mat = ROTATE(mat, Float(45.0), Vector3F(0, 1, 0));
            std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>(mat);
            std::shared_ptr<Intersectable> bunny = testBunny(metal, nullptr, nullptr, nullptr, transformMatrix);
            objects.push_back(bunny);

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build area light
            auto lightWall = testTopAreaLight(area_light_spectrum, nullptr, scene->_lights, lambertTop);
            objects.insert(objects.end(), lightWall.begin(), lightWall.end());

            // scene
            std::shared_ptr<Intersectable> bvh = std::make_shared<BVH>(objects);
            scene->_world = bvh;

            // build camera
            auto eye = Vector3F(2.2 * MODEL_SCALE, 0.5 * MODEL_SCALE, 1.2);
            auto dir = Vector3F(-0.95f, -0.6f, -0.3f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir, nullptr);
            camera->setResolutionWidth(Config::Camera::width);
            camera->setResolutionHeight(Config::Camera::height);
            scene->_camera = camera;
            scene->_sceneName = "girl-friend";

            return scene;
        }
         */

#endif

        std::shared_ptr<Scene> Scene::sceneBunnyWithPointLight() {
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

            // build environment lights
            /*
            Light::Ptr enLight = std::make_shared<EnvironmentLight>(1, "./resource/texture/en2.png",
                                                                    MediumBoundary(nullptr, nullptr));
            scene->addEnvironmentLight(enLight);
             */

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

            scene->_sceneName = "bunny-with-point-light.png";

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

        std::shared_ptr<Scene> Scene::sceneBunnyWithAreaLight() {
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
//            std::shared_ptr<Medium> bunnyMedium = testSmokeMedium();
            std::shared_ptr<Medium> bunnyMedium = nullptr;
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
            camera->setResolutionWidth(Config::Camera::width);
            camera->setResolutionHeight(Config::Camera::height);
            scene->_camera = camera;
            scene->_sceneName = "bunny-with-area-light";

            return scene;
        }

        std::shared_ptr<Scene> Scene::sceneTwoSpheresWithAreaLight() {
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

            // objects
            std::vector<std::shared_ptr<Intersectable>> objects;

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(totalWhite, 1.5);

//            std::shared_ptr<Medium> airMedium = testAirMedium();
            std::shared_ptr<Medium> airMedium = nullptr;

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

            std::shared_ptr<Geometry> glassSphere = buildGlassSphere(
                    Vector3F(0.25 * MODEL_SCALE, -0.338 * MODEL_SCALE, 0 * MODEL_SCALE), 0.16 * MODEL_SCALE);

            std::shared_ptr<Geometry> metalSphere = buildLambSphereImageTexture(
                    Vector3F(-0.25 * MODEL_SCALE, -0.298 * MODEL_SCALE, 0.2 * MODEL_SCALE),
                    0.2 * MODEL_SCALE);

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build area light
            auto lightWall = testTopAreaLight(area_light_spectrum, airMedium, scene->_lights, lambertTop);
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
            camera->setResolutionWidth(Config::Camera::width);
            camera->setResolutionHeight(Config::Camera::height);
            scene->_camera = camera;
            scene->_sceneName = "two-spheres-with-area-light";

            return scene;
        }

        std::shared_ptr<Scene> Scene::sceneTwoSpheresWithSpotLight() {
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

            std::shared_ptr<Geometry> glassSphere = buildGlassSphere(Vector3F(0.25 * MODEL_SCALE, -0.338 * MODEL_SCALE, 0 * MODEL_SCALE), 0.16 * MODEL_SCALE);
            std::shared_ptr<Geometry> metalSphere = buildMetalSphere(Vector3F(-0.25 * MODEL_SCALE, -0.298 * MODEL_SCALE, 0.2 * MODEL_SCALE), 0.2 * MODEL_SCALE);

            // build scene object
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();

            // build point light
            std::shared_ptr<SpotLight> light = SpotLight::buildSpotLight(
                    Vector3F(0 * MODEL_SCALE, 0.46 * MODEL_SCALE, 0 * MODEL_SCALE), Vector3F(0.0, -1, 0.0),
                    spotLightSpectrum, MediumBoundary(airMedium.get(), airMedium.get()));
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
            camera->setResolutionWidth(Config::Camera::width);
            camera->setResolutionHeight(Config::Camera::height);
            scene->_camera = camera;
            scene->_sceneName = "two-spheres-with-spot-light";

            return scene;
        }

        std::shared_ptr<Scene> Scene::sceneTwoSpheresWithPointLight() {
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
                    MediumBoundary(airMedium.get(), airMedium.get()));
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
            camera->setResolutionWidth(Config::Camera::width);
            camera->setResolutionHeight(Config::Camera::height);
            scene->_camera = camera;
            scene->_sceneName = "two-spheres-with-point-light";

            return scene;
        }

        std::shared_ptr<Scene> Scene::sceneLightThroughAir() {
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
                    MediumBoundary(airMedium.get(), airMedium.get()));
            scene->_lights.push_back(light);

            objects.push_back(glassSphere);

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
            scene->_sceneName = "light_through-air";

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