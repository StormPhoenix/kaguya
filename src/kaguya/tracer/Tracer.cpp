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

        std::shared_ptr<Light> Tracer::uniformSampleLight(std::shared_ptr<Scene> scene, Float *lightPdf, Sampler *sampler) {
            // Sample from multiple light
            auto lights = scene->getLights();
            int nLights = lights.size();
            if (nLights == 0) {
                return nullptr;
            }
            *lightPdf = Float(1.0) / nLights;

            // Randomly select a light
            int lightIndex = std::min(int(sampler->sample1D() * nLights), nLights - 1);
            return lights[lightIndex];
        }
    }
}