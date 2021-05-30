//
// Created by Graphics on 2021/5/30.
//

#include <kaguya/scene/OptiXScene.h>
#include <kaguya/gpu/optix_utils.h>

namespace RENDER_NAMESPACE {
    namespace scene {
        void OptiXScene::buildOptiXData() {
            // OptiX initialization
            OPTIX_CHECK(optixInit());
            OptixDeviceContextOptions ctxOptions = {};
            ctxOptions.logCallbackLevel = 4;

            // TODO
        }
    }
}