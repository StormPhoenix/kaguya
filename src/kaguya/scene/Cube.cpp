//
// Created by Storm Phoenix on 2020/12/21.
//

#include <kaguya/scene/Cube.h>
#include <kaguya/scene/meta/Triangle.h>

#include <iostream>

namespace RENDER_NAMESPACE {
    namespace scene {

        Cube::Cube(Matrix4F transfromMat) : _transformMat(transfromMat) {
            build();
        }

        void Cube::build() {
            Vector3F vertices[] = {
                    {1,  -1, -1},
                    {1,  -1, 1},
                    {-1, -1, 1},
                    {-1, -1, -1},
                    {1,  1,  -1},
                    {-1, 1,  -1},
                    {-1, 1,  1},
                    {1,  1,  1}
            };

            Vector3i indices[] = {
                    {7, 2, 1},
                    {7, 6, 2},
                    {4, 1, 0},
                    {4, 7, 1},
                    {5, 0, 3},
                    {5, 4, 0},
                    {6, 3, 2},
                    {6, 5, 3},
                    {4, 6, 7},
                    {4, 5, 6},
                    {1, 2, 3},
                    {1, 3, 0}
            };

            Transform::Ptr toWorld = std::make_shared<Transform>(_transformMat);

            for (int i = 0; i < 12; i++) {
                Vector3i index = indices[i];
                Vector3F v1 = vertices[index[0]];
                Vector3F v2 = vertices[index[1]];
                Vector3F v3 = vertices[index[2]];
                Normal3F normal = NORMALIZE(CROSS(v2 - v1, v3 - v1));
                Vector2F textureCoord = Vector2F(0, 0);
                Shape::Ptr shape = std::make_shared<Triangle>(
                        v1, v2, v3, normal, normal, normal,
                        textureCoord, textureCoord, textureCoord,
                        toWorld);
                _triangles->push_back(shape);
            }
        }
    }
}