//
// Created by Graphics on 2021/5/30.
//

#ifndef TUNAN_OPTIXSCENEIMPORTER_H
#define TUNAN_OPTIXSCENEIMPORTER_H

#include <kaguya/common.h>
#include <kaguya/scene/OptiXScene.h>

namespace RENDER_NAMESPACE {
    namespace scene {
        namespace importer {
            class OptiXSceneImporter {
            public:
                virtual std::shared_ptr<OptiXScene> importScene(std::string sceneFile) = 0;
            };
        }
    }
}

#endif //TUNAN_OPTIXSCENEIMPORTER_H
