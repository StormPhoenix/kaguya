//
// Created by StormPhoenix on 2021/5/28.
//

#ifndef KAGUYA_SCENEDATA_H
#define KAGUYA_SCENEDATA_H

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

        struct ShapeEntity {
            std::vector<Point3F> vertices;
            std::vector<size_t> vertexIndices;

            std::string materialName;
            int materialIndex;
        };

        class SceneData {
        public:
            // Entities
            std::vector<ShapeEntity> entities;

            // Lights
            std::vector<Light> lights;
            std::vector<Light> envLights;

            // Materials
            std::map<std::string, Material> namedMaterial;
            std::vector<Material> materials;
        };

    }
}

#endif //KAGUYA_SCENEDATA_H
