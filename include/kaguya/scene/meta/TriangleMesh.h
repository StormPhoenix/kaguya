//
// Created by Storm Phoenix on 2020/10/10.
//

#ifndef KAGUYA_TRIANGLEMESH_H
#define KAGUYA_TRIANGLEMESH_H

#include <kaguya/scene/Shape.h>
#include <kaguya/scene/meta/Vertex.h>
#include <kaguya/scene/accumulation/BVH.h>

#include <vector>

namespace kaguya {
    namespace scene {

        using kaguya::scene::acc::BVH;

        class TriangleMesh : public BVH {
        public:
            TriangleMesh(std::vector<Vertex> &vertices,
                         std::shared_ptr<Material> material,
                         std::shared_ptr<Matrix4> transformMatrix = nullptr);

            virtual const long long getId() const override;

            virtual void setId(long long id) override;

            virtual ~TriangleMesh() {}

        protected:
            void buildMeshes();

        private:
            std::vector<Vertex> &_vertices;
            std::vector<std::shared_ptr<Shape>> _triangles;
            std::shared_ptr<Material> _material = nullptr;
            std::shared_ptr<Matrix4> _transformMatrix = nullptr;
        };

    }
}

#endif //KAGUYA_TRIANGLEMESH_H
