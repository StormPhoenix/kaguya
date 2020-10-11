//
// Created by Storm Phoenix on 2020/10/10.
//

#include <kaguya/math/Math.hpp>
#include <kaguya/scene/meta/Triangle.h>
#include <kaguya/scene/meta/TriangleMesh.h>

namespace kaguya {
    namespace scene {

        TriangleMesh::TriangleMesh(std::vector<Vertex> &vertices, std::shared_ptr<Material> material,
                                   std::shared_ptr<Matrix4> transformMatrix)
                : BVH(), _vertices(vertices), _material(material), _transformMatrix(transformMatrix) {
            buildMeshes();
        }

        void TriangleMesh::setId(long long id) {
            for (auto it = _triangles.begin(); it != _triangles.end(); it++) {
                (*it)->setId(id);
            }
            _id = id;
        }

        const long long TriangleMesh::getId() const {
            return _id;
        }

        void TriangleMesh::buildMeshes() {
            assert(_vertices.size() != 0 && _vertices.size() % 3 == 0);
            for (int i = 0; i < _vertices.size(); i += 3) {
                Vertex vertex1 = _vertices[i];
                Vertex vertex2 = _vertices[i + 1];
                Vertex vertex3 = _vertices[i + 2];
                _triangles.push_back(
                        std::make_shared<Triangle>(vertex1, vertex2, vertex3, _material, _transformMatrix)
                );
            }
            BVH::build(_triangles, 0, _triangles.size());
        }
    }
}