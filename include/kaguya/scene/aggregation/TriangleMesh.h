//
// Created by Storm Phoenix on 2020/10/10.
//

#ifndef KAGUYA_TRIANGLEMESH_H
#define KAGUYA_TRIANGLEMESH_H

#include <kaguya/scene/meta/Shape.h>
#include <kaguya/scene/meta/Vertex.h>
#include <kaguya/scene/accumulation/BVH.h>

#include <vector>

namespace kaguya {
    namespace scene {

        using kaguya::scene::acc::BVH;

        class TriangleMesh : public BVH {
        public:
            TriangleMesh(std::vector<Vertex> &vertices,
                         const std::shared_ptr<Material> material,
                         const std::shared_ptr<Medium> inside = nullptr,
                         const std::shared_ptr<Medium> outside = nullptr,
                         const std::shared_ptr<AreaLight> areaLight = nullptr,
                         std::shared_ptr<Matrix4> transformMatrix = nullptr);

            virtual ~TriangleMesh() {}

        protected:
            void buildMeshes();

        private:
            std::vector<Vertex> &_vertices;
            std::vector<std::shared_ptr<Intersectable>> _triangles;
            const std::shared_ptr<Material> _material;
            const std::shared_ptr<Medium> _inside;
            const std::shared_ptr<Medium> _outside;
            const std::shared_ptr<AreaLight> _areaLight;
            std::shared_ptr<Matrix4> _transformMatrix = nullptr;
        };

    }
}

#endif //KAGUYA_TRIANGLEMESH_H
