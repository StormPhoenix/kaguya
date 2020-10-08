//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/scene/accumulation/BVH.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/scene/meta/Sphere.h>
#include <kaguya/material/Dielectric.h>
#include <kaguya/material/Lambertian.h>
#include <kaguya/material/Metal.h>
#include <kaguya/material/ConstantTexture.h>

namespace kaguya {
    namespace scene {

        using namespace kaguya::material;
        using namespace kaguya::scene::acc;

        void Scene::testBuildTwoSphere() {
            // For testing
            // albedos
            std::shared_ptr<Texture> albedo0 = std::make_shared<ConstantTexture>(Vector3(1.0, 1.0, 1.0));
            std::shared_ptr<Texture> albedo1 = std::make_shared<ConstantTexture>(Vector3(0.5, 0.1, 0.7));
            std::shared_ptr<Texture> albedo2 = std::make_shared<ConstantTexture>(Vector3(0.2, 0.9, 0.1));

            // lambertian materials
            std::shared_ptr<Material> lambertian1 = std::make_shared<Lambertian>(albedo1);
            std::shared_ptr<Material> lambertian2 = std::make_shared<Lambertian>(albedo2);

            // metal material
            std::shared_ptr<Material> metal1 = std::make_shared<Metal>();

            // dielectric material
            std::shared_ptr<Material> dielectric1 = std::make_shared<Dielectric>(albedo0);

            // spheres
            std::shared_ptr<Hittable> sphere1 = std::make_shared<Sphere>(Vector3(0, 40, 0), 40, dielectric1);
            std::shared_ptr<Hittable> sphere2 = std::make_shared<Sphere>(Vector3(0, -1000, 0), 1000, lambertian1);

            // objects
            std::vector<std::shared_ptr<Hittable>> objects;
            objects.push_back(sphere1);
            objects.push_back(sphere2);
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