//
// Created by Storm Phoenix on 2020/10/10.
//

#include <kaguya/utils/ObjLoader.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <ext/tinyobjloader/tiny_obj_loader.h>

namespace RENDER_NAMESPACE {
    namespace utils {
        namespace io {
            using kaguya::scene::TriMesh;

            bool ObjLoader::loadObj(const std::string path,
                                    std::vector<Vector3F> &vertices,
                                    std::vector<Normal3F> &normals,
                                    std::vector<Point2F> &texcoords,
                                    std::vector<TriMesh::TriIndex> &indics) {
                tinyobj::attrib_t attrib;
                std::vector<tinyobj::shape_t> shapes;
                std::vector<tinyobj::material_t> materials;

                std::string warn;
                std::string err;
                bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
                if (!err.empty()) {
                    std::cerr << err << std::endl;
                    return false;
                }

                if (!ret) {
                    std::cerr << ret << std::endl;
                    return false;
                }

//            vertices.resize(attrib.vertices.size() / 3);
                for (int i = 0; i < attrib.vertices.size(); i += 3) {
                    Float x = attrib.vertices[i];
                    Float y = attrib.vertices[i + 1];
                    Float z = attrib.vertices[i + 2];
                    vertices.push_back({x, y, z});
                }

//            normals.resize(attrib.vertices.size() / 3);
                for (int i = 0; i < attrib.normals.size(); i += 3) {
                    Float x = attrib.normals[i];
                    Float y = attrib.normals[i + 1];
                    Float z = attrib.normals[i + 2];
                    normals.push_back({x, y, z});
                }

//            texcoords.resize(attrib.vertices.size() / 2);
                for (int i = 0; i < attrib.texcoords.size(); i += 2) {
                    Float x = attrib.texcoords[i];
                    Float y = attrib.texcoords[i + 1];
                    texcoords.push_back({x, y});
                }

                for (int s = 0; s < shapes.size(); s++) {
                    int index_offset = 0;
                    for (int f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                        int fv = shapes[s].mesh.num_face_vertices[f];
                        ASSERT(fv == 3, "Only support triangle mesh");;

                        TriMesh::Index tri[3];
                        for (int v = 0; v < fv; v++) {
                            // access to vertex
                            tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                            tri[v] = {idx.vertex_index, idx.normal_index, idx.texcoord_index};
                        }
                        int materialId = shapes[s].mesh.material_ids[f];
                        indics.push_back(TriMesh::TriIndex(tri[0], tri[1], tri[2], materialId));
                        index_offset += fv;
                    }
                }
                return true;
            }
        }
    }
}