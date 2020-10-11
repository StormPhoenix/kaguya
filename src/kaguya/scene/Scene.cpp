//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/Config.h>
#include <kaguya/scene/accumulation/BVH.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/scene/meta/Sphere.h>
#include <kaguya/scene/meta/Wall.h>
#include <kaguya/scene/meta/Light.h>
#include <kaguya/scene/meta/TriangleMesh.h>
#include <kaguya/material/Dielectric.h>
#include <kaguya/material/Lambertian.h>
#include <kaguya/material/Metal.h>
#include <kaguya/material/Emitter.h>
#include <kaguya/material/ConstantTexture.h>
#include <kaguya/utils/ObjLoader.h>

namespace kaguya {
    namespace scene {

        using namespace kaguya::material;
        using namespace kaguya::scene::acc;

        void Scene::testBuildCornelBoxWithBunny() {
            // For testing
            // albedos
            // white
            std::shared_ptr<Texture> white = std::make_shared<ConstantTexture>(Vector3(0.73, 0.73, 0.73));
            // red
            std::shared_ptr<Texture> red = std::make_shared<ConstantTexture>(Vector3(0.65, 0.05, 0.05));
            // lake blue
            std::shared_ptr<Texture> lakeBlue = std::make_shared<ConstantTexture>(
                    Vector3(30.0 / 255, 144.0 / 255, 200.0 / 255));
            // green
            std::shared_ptr<Texture> green = std::make_shared<ConstantTexture>(Vector3(0.12, 0.45, 0.15));
            // blue
            std::shared_ptr<Texture> blue = std::make_shared<ConstantTexture>(Vector3(0.0, 0.0, 1.0));
            // pink
            std::shared_ptr<Texture> pink = std::make_shared<ConstantTexture>(
                    Vector3(255.0 / 255, 192.0 / 255, 203.0 / 255));

            std::shared_ptr<Texture> lightAlbedo = std::make_shared<ConstantTexture>(
                    Vector3(double(249.0) / 255.0 * 12, double(222.0) / 255.0 * 12, double(180.0) / 255.0 * 12));

            // 255 222 99

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Emitter> lightMaterial = std::make_shared<Emitter>(lightAlbedo);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(white, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            // walls
            std::shared_ptr<Hittable> leftWall = std::make_shared<YZWall>(-250, 250, -250, 250, -250, true,
                                                                          lambertLeft);
            std::shared_ptr<Hittable> rightWall = std::make_shared<YZWall>(-250, 250, -250, 250, 250, false,
                                                                           lambertRight);
            std::shared_ptr<Hittable> bottomWall = std::make_shared<ZXWall>(-250, 250, -250, 250, -250, true,
                                                                            lambertBottom);
            std::shared_ptr<Hittable> topWall = std::make_shared<ZXWall>(-250, 250, -250, 250, 250, false,
                                                                         lambertTop);
            std::shared_ptr<Hittable> frontWall = std::make_shared<XYWall>(-250, 250, -250, 250, -250, false,
                                                                           lambertFront);

            std::shared_ptr<Hittable> glassSphere = std::make_shared<Sphere>(Vector3(125, -169, 0), 80, glass);
//            std::shared_ptr<Hittable> glassSphere = std::make_shared<Sphere>(Vector3(75, 0, 0), 80, metal);

            std::shared_ptr<Hittable> metalSphere = std::make_shared<Sphere>(Vector3(-125, -149, 100), 100, metal);

            // light
            std::shared_ptr<ObjectSampler> lightWall = std::make_shared<ZXWall>(-100, 100, -100, 100, 248, false,
                                                                                nullptr);
            std::shared_ptr<Light> light = std::make_shared<Light>(lightMaterial, lightWall);
            _light = light;

            // load model
            std::vector<Vertex> bunnyVertexes = kaguya::utils::ObjLoader::loadModel("./resource/objects/bunny.obj");

            std::shared_ptr<Matrix4> transformMatrix = std::make_shared<Matrix4>(1.0f);
            double scale = 400.0;
            *transformMatrix = TRANSLATE(*transformMatrix, Vector3(-scale / 2, -scale / 2, -scale / 2));
            *transformMatrix = SCALE(*transformMatrix, Vector3(scale, scale, scale));
            std::shared_ptr<Hittable> bunny = std::static_pointer_cast<Hittable>(
                    std::make_shared<TriangleMesh>(bunnyVertexes, metal, transformMatrix));


            // objects
            std::vector<std::shared_ptr<Hittable>> objects;
            objects.push_back(leftWall);
            objects.push_back(rightWall);
            objects.push_back(bottomWall);
            objects.push_back(topWall);
            objects.push_back(frontWall);
            objects.push_back(light);
            objects.push_back(bunny);
//            objects.push_back(glassSphere);
//            objects.push_back(metalSphere);

            // 给所有 object 赋予 id
            for (long long id = 0; id < objects.size(); id++) {
                objects[id]->setId(id);
            }

            // scene
            std::shared_ptr<Hittable> bvh = std::make_shared<BVH>(objects);
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
            // albedos
            // white
            std::shared_ptr<Texture> white = std::make_shared<ConstantTexture>(Vector3(0.73, 0.73, 0.73));
            // red
            std::shared_ptr<Texture> red = std::make_shared<ConstantTexture>(Vector3(0.65, 0.05, 0.05));
            // lake blue
            std::shared_ptr<Texture> lakeBlue = std::make_shared<ConstantTexture>(
                    Vector3(30.0 / 255, 144.0 / 255, 200.0 / 255));
            // green
            std::shared_ptr<Texture> green = std::make_shared<ConstantTexture>(Vector3(0.12, 0.45, 0.15));
            // blue
            std::shared_ptr<Texture> blue = std::make_shared<ConstantTexture>(Vector3(0.0, 0.0, 1.0));
            // pink
            std::shared_ptr<Texture> pink = std::make_shared<ConstantTexture>(
                    Vector3(255.0 / 255, 192.0 / 255, 203.0 / 255));

            std::shared_ptr<Texture> lightAlbedo = std::make_shared<ConstantTexture>(
                    Vector3(double(249.0) / 255.0 * 12, double(222.0) / 255.0 * 12, double(180.0) / 255.0 * 12));

            // 255 222 99

            // lambertian materials
            std::shared_ptr<Material> lambertLeft = std::make_shared<Lambertian>(red);
            std::shared_ptr<Material> lambertRight = std::make_shared<Lambertian>(green);
            std::shared_ptr<Material> lambertBottom = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertTop = std::make_shared<Lambertian>(white);
            std::shared_ptr<Material> lambertFront = std::make_shared<Lambertian>(white);
            std::shared_ptr<Emitter> lightMaterial = std::make_shared<Emitter>(lightAlbedo);
            std::shared_ptr<Material> glass = std::make_shared<Dielectric>(white, 1.5);
            std::shared_ptr<Material> metal = std::make_shared<Metal>();

            // walls
            std::shared_ptr<Hittable> leftWall = std::make_shared<YZWall>(-250, 250, -250, 250, -250, true,
                                                                          lambertLeft);
            std::shared_ptr<Hittable> rightWall = std::make_shared<YZWall>(-250, 250, -250, 250, 250, false,
                                                                           lambertRight);
            std::shared_ptr<Hittable> bottomWall = std::make_shared<ZXWall>(-250, 250, -250, 250, -250, true,
                                                                            lambertBottom);
            std::shared_ptr<Hittable> topWall = std::make_shared<ZXWall>(-250, 250, -250, 250, 250, false,
                                                                         lambertTop);
            std::shared_ptr<Hittable> frontWall = std::make_shared<XYWall>(-250, 250, -250, 250, -250, false,
                                                                           lambertFront);

            std::shared_ptr<Hittable> glassSphere = std::make_shared<Sphere>(Vector3(125, -169, 0), 80, glass);
//            std::shared_ptr<Hittable> glassSphere = std::make_shared<Sphere>(Vector3(75, 0, 0), 80, metal);

            std::shared_ptr<Hittable> metalSphere = std::make_shared<Sphere>(Vector3(-125, -149, 100), 100, metal);

            // light
            std::shared_ptr<ObjectSampler> lightWall = std::make_shared<ZXWall>(-100, 100, -100, 100, 248, false,
                                                                                nullptr);
            std::shared_ptr<Light> light = std::make_shared<Light>(lightMaterial, lightWall);
            _light = light;

            // objects
            std::vector<std::shared_ptr<Hittable>> objects;
            objects.push_back(leftWall);
            objects.push_back(rightWall);
            objects.push_back(bottomWall);
            objects.push_back(topWall);
            objects.push_back(frontWall);
            objects.push_back(light);
            objects.push_back(glassSphere);
            objects.push_back(metalSphere);

            // 给所有 object 赋予 id
            for (long long id = 0; id < objects.size(); id++) {
                objects[id]->setId(id);
            }

            // scene
            std::shared_ptr<Hittable> bvh = std::make_shared<BVH>(objects);
            _world = bvh;

            // build camera
            auto eye = Vector3(0.0, 0.0, 700);
            auto dir = Vector3(0.0f, 0.0f, -1.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir);
            camera->setResolutionWidth(Config::resolutionWidth);
            camera->setResolutionHeight(Config::resolutionHeight);
            _camera = camera;
        }

        void Scene::testBuildTwoSphere() {
            // For testing
            // albedos
            std::shared_ptr<Texture> albedo0 = std::make_shared<ConstantTexture>(Vector3(1.0, 1.0, 1.0));
            std::shared_ptr<Texture> albedo1 = std::make_shared<ConstantTexture>(Vector3(0.5, 0.1, 0.7));
            std::shared_ptr<Texture> albedo2 = std::make_shared<ConstantTexture>(Vector3(0.2, 0.3, 0.9));

            // lambertian materials
            std::shared_ptr<Material> lambertian1 = std::make_shared<Lambertian>(albedo1);
            std::shared_ptr<Material> lambertian2 = std::make_shared<Lambertian>(albedo2);

            // metal material
            std::shared_ptr<Material> metal1 = std::make_shared<Metal>();

            // dielectric material
            std::shared_ptr<Material> dielectric1 = std::make_shared<Dielectric>(albedo0, 1.5);

            // spheres
            std::shared_ptr<Hittable> sphere1 = std::make_shared<Sphere>(Vector3(0, 40, 0), 39, dielectric1);
            std::shared_ptr<Hittable> sphere3 = std::make_shared<Sphere>(Vector3(0, 40, 0), 25, lambertian2);
            std::shared_ptr<Hittable> sphere2 = std::make_shared<Sphere>(Vector3(0, -1000, 0), 1000, lambertian1);
            std::shared_ptr<Hittable> sphere4 = std::make_shared<Sphere>(Vector3(80, 40, -80), 30, metal1);

            // objects
            std::vector<std::shared_ptr<Hittable>> objects;
            objects.push_back(sphere1);
            objects.push_back(sphere2);
            objects.push_back(sphere3);
            objects.push_back(sphere4);
            std::shared_ptr<Hittable> bvh = std::make_shared<BVH>(objects);

            // scene
            _world = bvh;
            // build camera
            auto eye = Vector3(0.0, 100, 200);
            auto dir = Vector3(0.0f, -1.0f, -3.0f);
            std::shared_ptr<Camera> camera = std::make_shared<Camera>(eye, dir);
            _camera = camera;
        }


        bool Scene::hit(const Ray &ray, HitRecord &hitRecord) {
            return _world->hit(ray, hitRecord, 0.001, infinity);
        }
    }
}