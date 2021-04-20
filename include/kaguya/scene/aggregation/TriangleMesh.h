//
// Created by Storm Phoenix on 2020/10/10.
//

#ifndef KAGUYA_TRIANGLEMESH_H
#define KAGUYA_TRIANGLEMESH_H

#include <kaguya/core/Transform.h>
#include <kaguya/scene/meta/Shape.h>
#include <kaguya/scene/accumulation/BVH.h>

#include <vector>

namespace kaguya {
    namespace scene {

        // TODO delete
        using namespace scene::meta;
        using kaguya::scene::acc::BVH;
        using kaguya::core::transform::Transform;

        class TriangleMesh final : public BVH {
        public:

            typedef struct Index {
                Index(const int &vertexIndex = 0, const int &normalIndex = 0, const int &texcoordIndex = 0)
                        : vertexIndex(vertexIndex), normalIndex(normalIndex), texcoordIndex(texcoordIndex) {};

                int vertexIndex;
                int normalIndex;
                int texcoordIndex;
            } Index;

            typedef struct TriangleIndex {
                TriangleIndex(const Index &v1, const Index &v2, const Index &v3, const int materialId)
                        : v1(v1), v2(v2), v3(v3), materialId(materialId) {}

                Index v1, v2, v3;
                int materialId;
            } TriIndex;

            typedef std::shared_ptr<TriangleMesh> Ptr;

            TriangleMesh(std::vector<Vector3F> &vertices, std::vector<Normal3F> &normals,
                         std::vector<Point2F> &texcoords, std::vector<TriIndex> &indics,
                         std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>(),
                         bool faceNormal = false);

            const std::shared_ptr<std::vector<Shape::Ptr>> triangles() const;

            virtual ~TriangleMesh() {}

        private:
            std::shared_ptr<std::vector<Shape::Ptr>> _triangles;
        };

        typedef TriangleMesh TriMesh;
    }
}

#endif //KAGUYA_TRIANGLEMESH_H
