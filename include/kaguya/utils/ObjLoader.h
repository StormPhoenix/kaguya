//
// Created by Storm Phoenix on 2020/10/10.
//

#ifndef KAGUYA_OBJLOADER_H
#define KAGUYA_OBJLOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <kaguya/scene/meta/Vertex.h>
#include <vector>

namespace kaguya {
    namespace utils {

        using kaguya::scene::Vertex;

        class ObjLoader {
        public:
            static std::vector<Vertex> loadModel(const std::string &path);

        private:

            static void loadNode(const aiNode *node, const aiScene *scene, std::vector<Vertex> &vertices);

            static void loadMesh(const aiMesh *mesh, const aiScene *scene, std::vector<Vertex> &vertices);
        };

    }
}

#endif //KAGUYA_OBJLOADER_H
