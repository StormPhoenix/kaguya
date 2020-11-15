//
// Created by Storm Phoenix on 2020/11/2.
//

#include <kaguya/Config.h>
#include <kaguya/tracer/Tracer.h>

namespace kaguya {
    namespace tracer {

        using kaguya::Config;

        Tracer::Tracer() {
            _scene = Config::buildScene();
            _camera = _scene->getCamera();
        }

        void Tracer::run() {
            assert(_camera != nullptr);
            _filmPlane = _camera->buildFilmPlane(SPECTRUM_CHANNEL);

            // rendering
            if (render()) {
                // write to image
                _filmPlane->writeImage(Config::imageFilename.c_str());
            }

            delete _filmPlane;
            _filmPlane = nullptr;
        }
    }
}