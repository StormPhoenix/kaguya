//
// Created by Graphics on 2021/5/30.
//

#ifndef TUNAN_OPTIXSCENE_H
#define TUNAN_OPTIXSCENE_H

#include <kaguya/common.h>
#include <kaguya/material/Material.h>
#include <kaguya/core/light/Light.h>
#include <kaguya/math/Math.h>

#include <string>
#include <vector>
#include <map>

namespace RENDER_NAMESPACE {
    namespace scene {
        using material::Material;
        using core::Light;

        struct ShapeEntity;

        // TODO extend Scene
        class OptiXScene {
        public:
            void buildOptiXData();

        private:
            // Entities
            std::vector<ShapeEntity> entities;

            // Lights
            std::vector<Light> lights;
            std::vector<Light> envLights;

            // Materials
            std::map<std::string, Material> namedMaterial;
            std::vector<Material> materials;
        };

        struct ShapeEntity {
            std::vector<Point3F> vertices;
            std::vector<size_t> vertexIndices;

            std::string materialName;
            int materialIndex;
        };
    }
}

#endif //TUNAN_OPTIXSCENE_H
