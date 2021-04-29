//
// Created by Storm Phoenix on 2020/10/10.
//

#include <kaguya/math/Math.h>
#include <kaguya/scene/meta/Triangle.h>
#include <kaguya/scene/TriangleMesh.h>

namespace kaguya {
    namespace scene {

        using meta::Shape;

        TriangleMesh::TriangleMesh() {
            _triangles = std::make_shared<std::vector<Shape::Ptr>>();
        }

        TriangleMesh::TriangleMesh(std::vector<Vector3F> &vertices, std::vector<Normal3F> &normals,
                                   std::vector<Point2F> &texcoords, std::vector<TriIndex> &indics,
                                   std::shared_ptr<Transform> transformMatrix, bool faceNormal) {
            _triangles = std::make_shared<std::vector<Shape::Ptr>>();

            bool texFlag = texcoords.size() > 0;
            bool useNormals = (!faceNormal && normals.size() > 0);

            for (auto it = indics.begin(); it != indics.end(); it++) {
                const TriIndex &index = *it;
                Vector3F v1, v2, v3;
                {
                    v1 = vertices[index.v1.vertexIndex];
                    v2 = vertices[index.v2.vertexIndex];
                    v3 = vertices[index.v3.vertexIndex];
                }

                Normal3F n1(0), n2(0), n3(0);
                {
                    if (useNormals) {
                        n1 = normals[index.v1.normalIndex];
                        n2 = normals[index.v2.normalIndex];
                        n3 = normals[index.v3.normalIndex];
                    }
                }

                Point2F t1(0), t2(0), t3(0);
                {
                    if (texFlag) {
                        t1 = texcoords[index.v1.texcoordIndex];
                        t2 = texcoords[index.v2.texcoordIndex];
                        t3 = texcoords[index.v3.texcoordIndex];
                    }
                }

                Shape::Ptr tri = std::make_shared<Triangle>(v1, v2, v3, n1, n2, n3,
                                                            t1, t2, t3, transformMatrix);
                _triangles->push_back(tri);
            }
        }

        const std::shared_ptr<std::vector<Shape::Ptr>> TriangleMesh::triangles() const {
            return _triangles;
        }

    }
}