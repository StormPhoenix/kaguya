//
// Created by Storm Phoenix on 2021/4/16.
//

#ifndef KAGUYA_SCENEIMPORTER_H
#define KAGUYA_SCENEIMPORTER_H

#include <kaguya/common.h>
#include <kaguya/scene/SimpleScene.h>

namespace RENDER_NAMESPACE {
    namespace scene {
        namespace importer {
            class SimpleSceneImporter {
            public:
                virtual std::shared_ptr<SimpleScene> importScene(std::string sceneFile) = 0;
            };
        }
    }
}

#endif //KAGUYA_SCENEIMPORTER_H
