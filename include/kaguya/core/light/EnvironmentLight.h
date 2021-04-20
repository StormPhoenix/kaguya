//
// Created by Storm Phoenix on 2021/4/16.
//

#ifndef KAGUYA_ENVIRONMENTLIGHT_H
#define KAGUYA_ENVIRONMENTLIGHT_H

#include <kaguya/core/light/Light.h>
#include <kaguya/core/Transform.h>
#include <kaguya/scene/Scene.h>

namespace kaguya {
    namespace core {

        using namespace transform;
        using namespace scene;

        // TODO 对纹理图的重要性采样 ...

        class EnvironmentLight : public Light {
        public:
            EnvironmentLight(Float intensity, std::string texturePath, const MediumBoundary &mediumBoundary,
                             Transform::Ptr lightToWorld = std::make_shared<Transform>());

            virtual Spectrum Le(const Ray &ray) const override;

            virtual Spectrum sampleLi(const Interaction &eye, Vector3F *wi, Float *pdf,
                                      Sampler *sampler, VisibilityTester *visibilityTester) override;

            virtual Float pdfLi(const Interaction &eye, const Vector3F &dir) override;

            virtual Spectrum sampleLe(Ray *ray, Vector3F *normal, Float *pdfPos,
                                      Float *pdfDir, Sampler *sampler) override;

            virtual void pdfLe(const Ray &ray, const Vector3F &normal,
                               Float *pdfPos, Float *pdfDir) const override;

            void worldBound(const Scene::Ptr scene);

            ~EnvironmentLight();

        private:
            Spectrum sampleTexture(Point2F uv) const;

        private:
            Float _intensity = 12;
            Transform::Ptr _lightToWorld = nullptr;
            Transform::Ptr _worldToLight = nullptr;
            Float _worldRadius = 20000;
            Point3F _worldCenter;
            // Texture
            unsigned char *_texture = nullptr;
            int _width, _height;
            int _channel;
        };
    }
}

#endif //KAGUYA_ENVIRONMENTLIGHT_H
