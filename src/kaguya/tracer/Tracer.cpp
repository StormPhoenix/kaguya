//
// Created by Storm Phoenix on 2020/11/2.
//

#include <kaguya/Config.h>
#include <kaguya/parallel/RenderPool.h>
#include <kaguya/tracer/Tracer.h>

#include <iostream>

namespace kaguya {
    namespace tracer {

        using kaguya::Config;

        Tracer::Tracer() {}

        void Tracer::run() {
            _scene = Config::nextScene();

            parallel::RenderPool *pool = parallel::RenderPool::getInstance();
            while (_scene != nullptr) {
                _camera = _scene->getCamera();

                assert(_camera != nullptr);
                _filmPlane = _camera->buildFilmPlane(SPECTRUM_CHANNEL);

                // rendering
                auto renderFunc = render();

                // execute rendering task
                int cameraWidth = _camera->getResolutionWidth();
                int cameraHeight = _camera->getResolutionHeight();
                pool->addRenderTask(renderFunc, cameraWidth, cameraHeight);

                // write to image
                std::cout << std::endl << "scene " << _scene->getName() << " completed." << std::endl;
                _filmPlane->writeImage((Config::filenamePrefix + "_" +
                                        _scene->getName() + "_" +
                                        Config::filenameSufix).c_str());

                delete _filmPlane;
                _filmPlane = nullptr;

                _camera = nullptr;
                _scene = Config::nextScene();
            }

            pool->shutdown();
//            delete pool;
        }
    }
}