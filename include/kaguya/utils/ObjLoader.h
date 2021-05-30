//
// Created by Storm Phoenix on 2020/10/10.
//

#ifndef KAGUYA_OBJLOADER_H
#define KAGUYA_OBJLOADER_H

#include <vector>
#include <kaguya/scene/meta/TriangleMesh.h>

namespace RENDER_NAMESPACE {
    namespace utils {
        namespace io {
            using kaguya::scene::TriMesh;

            class ObjLoader {
            public:
                static bool loadObj(const std::string path,
                                    std::vector<Vector3F> &vertices,
                                    std::vector<Normal3F> &normals,
                                    std::vector<Point2F> &texcoords,
                                    std::vector<TriMesh::TriIndex> &indics);
            };
        }
    }
}

#endif //KAGUYA_OBJLOADER_H
