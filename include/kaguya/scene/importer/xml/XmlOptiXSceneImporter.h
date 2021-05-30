//
// Created by Graphics on 2021/5/30.
//

#ifndef TUNAN_XMLOPTIXSCENEIMPORTER_H
#define TUNAN_XMLOPTIXSCENEIMPORTER_H

#include <kaguya/scene/importer/OptiXSceneImporter.h>

namespace RENDER_NAMESPACE {
    namespace scene {
        namespace importer {
            class XmlOptiXSceneImporter : public OptiXSceneImporter {
            public:
                virtual std::shared_ptr<OptiXScene> importScene(std::string sceneFile) override;
            };
        }
    }
}

#endif //TUNAN_XMLOPTIXSCENEIMPORTER_H
