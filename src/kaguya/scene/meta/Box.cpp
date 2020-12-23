//
// Created by Storm Phoenix on 2020/12/21.
//

#include <kaguya/scene/aggregation/Box.h>
#include <kaguya/scene/meta/Triangle.h>

#include <iostream>

namespace kaguya {
    namespace scene {

        Box::Box(const std::shared_ptr<Material> material,
                 const std::shared_ptr<Medium> inside,
                 const std::shared_ptr<Medium> outside,
                 const std::shared_ptr<Matrix4> transform) :
                _transform(transform),
                _inside(inside),
                _outside(outside),
                _material(material) {
            build();
        }

        const AABB &Box::boundingBox() const {
            return _aabb;
        }

        void Box::build() {
            const Vector3 a1(0, 0, 0);
            const Vector3 a2(1, 0, 0);
            const Vector3 a3(1, 0, 1);
            const Vector3 a4(0, 0, 1);
            const Vector3 a5(0, 1, 0);
            const Vector3 a6(1, 1, 0);
            const Vector3 a7(1, 1, 1);
            const Vector3 a8(0, 1, 1);

            const Vector3 n_left(-1, 0, 0);
            const Vector3 n_right(1, 0, 0);
            const Vector3 n_up(0, 1, 0);
            const Vector3 n_down(0, -1, 0);
            const Vector3 n_front(0, 0, -1);
            const Vector3 n_back(0, 0, 1);

            const Vector2 default_uv(0);

            // TODO UV
            std::shared_ptr<meta::Shape> tri1 = std::make_shared<meta::Triangle>(a5, a4, a1,
                                                                     n_left, n_left, n_left,
                                                                     default_uv, default_uv, default_uv,
                                                                     _transform);
            std::shared_ptr<Geometry> gt1 = std::make_shared<Geometry>(tri1, _material, _inside, _outside, nullptr);
            _objects.push_back(gt1);

            std::shared_ptr<meta::Shape> tri2 = std::make_shared<meta::Triangle>(a5, a8, a4,
                                                                     n_left, n_left, n_left,
                                                                     default_uv, default_uv, default_uv,
                                                                     _transform);
            std::shared_ptr<Geometry> gt2 = std::make_shared<Geometry>(tri2, _material, _inside, _outside, nullptr);
            _objects.push_back(gt2);


            std::shared_ptr<meta::Shape> tri3 = std::make_shared<meta::Triangle>(a8, a3, a4,
                                                                     n_back, n_back, n_back,
                                                                     default_uv, default_uv, default_uv,
                                                                     _transform);
            std::shared_ptr<Geometry> gt3 = std::make_shared<Geometry>(tri3, _material, _inside, _outside, nullptr);
            _objects.push_back(gt3);


            std::shared_ptr<meta::Shape> tri4 = std::make_shared<meta::Triangle>(a8, a7, a3,
                                                                     n_back, n_back, n_back,
                                                                     default_uv, default_uv, default_uv,
                                                                     _transform);
            std::shared_ptr<Geometry> gt4 = std::make_shared<Geometry>(tri4, _material, _inside, _outside, nullptr);
            _objects.push_back(gt4);


            std::shared_ptr<meta::Shape> tri5 = std::make_shared<meta::Triangle>(a7, a2, a3,
                                                                     n_right, n_right, n_right,
                                                                     default_uv, default_uv, default_uv,
                                                                     _transform);
            std::shared_ptr<Geometry> gt5 = std::make_shared<Geometry>(tri5, _material, _inside, _outside, nullptr);
            _objects.push_back(gt5);


            std::shared_ptr<meta::Shape> tri6 = std::make_shared<meta::Triangle>(a7, a6, a2,
                                                                     n_right, n_right, n_right,
                                                                     default_uv, default_uv, default_uv,
                                                                     _transform);
            std::shared_ptr<Geometry> gt6 = std::make_shared<Geometry>(tri6, _material, _inside, _outside, nullptr);
            _objects.push_back(gt6);


            std::shared_ptr<meta::Shape> tri7 = std::make_shared<meta::Triangle>(a6, a1, a2,
                                                                     n_front, n_front, n_front,
                                                                     default_uv, default_uv, default_uv,
                                                                     _transform);
            std::shared_ptr<Geometry> gt7 = std::make_shared<Geometry>(tri7, _material, _inside, _outside, nullptr);
            _objects.push_back(gt7);


            std::shared_ptr<meta::Shape> tri8 = std::make_shared<meta::Triangle>(a6, a5, a1,
                                                                     n_front, n_front, n_front,
                                                                     default_uv, default_uv, default_uv,
                                                                     _transform);
            std::shared_ptr<Geometry> gt8 = std::make_shared<Geometry>(tri8, _material, _inside, _outside, nullptr);
            _objects.push_back(gt8);


            std::shared_ptr<meta::Shape> tri9 = std::make_shared<meta::Triangle>(a5, a7, a8,
                                                                     n_up, n_up, n_up,
                                                                     default_uv, default_uv, default_uv,
                                                                     _transform);
            std::shared_ptr<Geometry> gt9 = std::make_shared<Geometry>(tri9, _material, _inside, _outside, nullptr);
            _objects.push_back(gt9);


            std::shared_ptr<meta::Shape> tri10 = std::make_shared<meta::Triangle>(a5, a6, a7,
                                                                      n_up, n_up, n_up,
                                                                      default_uv, default_uv, default_uv,
                                                                      _transform);
            std::shared_ptr<Geometry> gt10 = std::make_shared<Geometry>(tri10, _material, _inside, _outside, nullptr);
            _objects.push_back(gt10);


            std::shared_ptr<meta::Shape> tri11 = std::make_shared<meta::Triangle>(a4, a3, a2,
                                                                      n_down, n_down, n_down,
                                                                      default_uv, default_uv, default_uv,
                                                                      _transform);
            std::shared_ptr<Geometry> gt11 = std::make_shared<Geometry>(tri11, _material, _inside, _outside, nullptr);
            _objects.push_back(gt11);


            std::shared_ptr<meta::Shape> tri12 = std::make_shared<meta::Triangle>(a4, a2, a1,
                                                                      n_down, n_down, n_down,
                                                                      default_uv, default_uv, default_uv,
                                                                      _transform);
            std::shared_ptr<Geometry> gt12 = std::make_shared<Geometry>(tri12, _material, _inside, _outside, nullptr);
            _objects.push_back(gt12);

            _aabb = mergeBoundingBox(_objects);
        }

        const std::vector<std::shared_ptr<Intersectable>> Box::aggregation() const {
            return _objects;
        }
    }
}