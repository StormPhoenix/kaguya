//
// Created by Storm Phoenix on 2020/10/10.
//

#include <kaguya/math/Math.h>
#include <kaguya/scene/meta/Triangle.h>
#include <kaguya/scene/aggregation/TriangleMesh.h>

namespace kaguya {
    namespace scene {

        TriangleMesh::TriangleMesh(std::vector<Vertex> &vertices,
                                   const std::shared_ptr<Material> material,
                                   const std::shared_ptr<Medium> inside,
                                   const std::shared_ptr<Medium> outside,
                                   const std::shared_ptr<AreaLight> areaLight,
                                   std::shared_ptr<Transform> transformMatrix)
                : BVH(), _vertices(vertices), _material(material), _inside(inside),
                  _outside(outside), _areaLight(areaLight), _transformMatrix(transformMatrix) {
            buildMeshes();
        }

        bool TriangleMesh::intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const {
            bool ret = BVH::intersect(ray, si, minStep, maxStep);
            if (ret) {
                si.setGeometry(this);
            }
            return ret;
        }

        void TriangleMesh::buildMeshes() {
            assert(_vertices.size() != 0 && _vertices.size() % 3 == 0);
            for (int i = 0; i < _vertices.size(); i += 3) {
                Vertex vertex1 = _vertices[i];
                Vertex vertex2 = _vertices[i + 1];
                Vertex vertex3 = _vertices[i + 2];
                std::shared_ptr<meta::Shape> triangle = std::make_shared<meta::Triangle>(vertex1, vertex2, vertex3,
                                                                                         _transformMatrix);
                std::shared_ptr<Geometry> trig = std::make_shared<Geometry>(triangle, _material, _inside, _outside,
                                                                            _areaLight);
                _triangles.push_back(trig);
            }
            BVH::build(_triangles, 0, _triangles.size());
        }
    }
}