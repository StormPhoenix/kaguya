//
// Created by Storm Phoenix on 2021/5/19.
//

#include <kaguya/Common.h>
#include <kaguya/core/light/Light.h>

namespace RENDER_NAMESPACE {
    namespace core {
        Light::Light(LightType type, const MediumInterface &mediumInterface) :
                _type(type), _mediumInterface(mediumInterface) {}

        Spectrum Light::Le(const Ray &ray) const { return Spectrum(0.0); }

        void Light::worldBound(const std::shared_ptr<Scene> scene) {}

        bool Light::isDeltaType() const {
            return (_type & (DELTA_DIRECTION | DELTA_POSITION)) > 0;
        }
    }
}