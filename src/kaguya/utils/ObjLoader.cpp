//
// Created by Storm Phoenix on 2020/10/10.
//

#include <kaguya/utils/ObjLoader.h>

namespace kaguya {
    namespace utils {

        std::vector<Vertex> ObjLoader::loadModel(const std::string &path) {
            std::vector<Vertex> vertexes;

            // read file via ASSIMP
            Assimp::Importer importer;
            const aiScene *scene = importer.ReadFile(path,
                                                     aiProcess_Triangulate |
                                                     aiProcess_FlipUVs |
                                                     aiProcess_CalcTangentSpace);
            // check for errors
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
                return std::vector<Vertex>();

            // retrieve the directory path of the filepath
            //directory = path.substr(0, path.find_last_of('/'));
            // process ASSIMP's root node recursively
            loadNode(scene->mRootNode, scene, vertexes);

            return vertexes;
        }

        void ObjLoader::loadNode(const aiNode *node, const aiScene *scene, std::vector<Vertex> &vertices) {
            // process each mesh located at the current node
            for (size_t i = 0; i < node->mNumMeshes; i++) {
                // the node object only contains indices to index the actual objects in the scene.
                // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
                loadMesh(mesh, scene, vertices);
            }
            // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
            for (size_t i = 0; i < node->mNumChildren; i++) {
                loadNode(node->mChildren[i], scene, vertices);
            }
        }

        void ObjLoader::loadMesh(const aiMesh *mesh, const aiScene *scene, std::vector<Vertex> &vertices) {

            // walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
            for (size_t i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                // retrieve all indices of the face and store them in the indices vector
                for (size_t j = 0; j < face.mNumIndices; j++)// mNumIndices == 3
                {
                    size_t idx = face.mIndices[j];
                    //indices.push_back(face.mIndices[j]);
                    Vertex vertex;
                    glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

                    // positions
                    vector.x = mesh->mVertices[idx].x;
                    vector.y = mesh->mVertices[idx].y;
                    vector.z = mesh->mVertices[idx].z;
                    vertex.position = vector;

                    // normals
                    if (mesh->mNormals != NULL) {
                        vector.x = mesh->mNormals[idx].x;
                        vector.y = mesh->mNormals[idx].y;
                        vector.z = mesh->mNormals[idx].z;
                    }

                    vertex.normal = vector;
                    // texture coordinates
                    if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
                    {
                        glm::vec2 vec;
                        // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                        // use ObjLoaders where a vertex can have multiple texture coordinates so we always take the first set (0).
                        vec.x = mesh->mTextureCoords[0][idx].x;
                        vec.y = mesh->mTextureCoords[0][idx].y;
                        vertex.u = vec.x;
                        vertex.v = vec.y;
                    } else {
                        vertex.u = 0;
                        vertex.u = 0;
                    }
                    vertices.push_back(vertex);
                }
            }
        }
    }
}