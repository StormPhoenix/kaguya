//
// Created by Storm Phoenix on 2020/10/16.
//

#ifndef KAGUYA_INTERACTION_H
#define KAGUYA_INTERACTION_H

#include <kaguya/core/core.h>
#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/medium/MediumInterface.h>
#include <kaguya/core/phase/PhaseFunction.h>
#include <kaguya/material/Material.h>
#include <kaguya/utils/memory/MemoryAllocator.h>
#include <kaguya/tracer/Ray.h>

namespace RENDER_NAMESPACE {

    namespace scene {
        class Intersectable;
    }

    namespace material {
        class Material;
    }

    namespace tracer {
        class Camera;
    }

    namespace core {
        using bsdf::TransportMode;
        using bsdf::BSDF;


        class AreaLight;

        class Light;

        using scene::Intersectable;
        using medium::Medium;
        using medium::MediumInterface;
        using kaguya::tracer::Camera;
        using kaguya::tracer::Ray;
        using kaguya::material::Material;
        using kaguya::memory::MemoryAllocator;
        using bssrdf::BSSRDF;

        inline Vector3F offsetOrigin(const Vector3F &origin, const Vector3F &error,
                                     const Vector3F &normal, const Vector3F &direction);


        /**
        * Ray 与 Scene 的交点记录
        */
        class Interaction {
        public:
            Interaction() : _mediumBoundary(nullptr, nullptr) {}

            Interaction(const Vector3F &point, const Vector3F &direction, const Vector3F &normal,
                        const MediumInterface &mediumBoundary, Material material = Material());

            virtual bool isMediumInteraction() const {
                return false;
            }

            /**
             * Generate ray alone @param dir from origin
             * @param dir
             */
            virtual Ray sendRay(const Vector3F &dir) const;

            virtual Ray sendRayTo(const Point3F &target) const;

            virtual Ray sendRayTo(const Interaction &it) const;

            void setMediumBoundary(const MediumInterface &mediumBoundary) {
                _mediumBoundary = mediumBoundary;
            }

        protected:
            /**
             * 清空 Interaction 中部分属性
             */
            virtual void reset() {
                _material = nullptr;
            }

            const Medium *getMedium(const Vector3F &dir) const;

        public:
            // Shading data
            struct {
                Normal3F normal;
            } rendering;
            // 发射 Interaction 的光线的方向
            Vector3F direction;
            // f(wo, wi) 的参数
            Vector3F wo;
            // 击中点
            Vector3F point;
            // 击中点法线方向，发现永远指向物体表面外侧
            Vector3F normal = Vector3F(0., 0., 0.);
            // Error range
            Vector3F error = Vector3F(0., 0., 0.);
        protected:
            // 击中材质种类
            Material _material;
            // medium boundary
            MediumInterface _mediumBoundary;

        public:
            const Material getMaterial() const {
                return _material;
            }

            void setMaterial(Material material) {
                _material = material;
            }
        };

        /**
         * 击中点为表面类型
         */
        class SurfaceInteraction : public Interaction {
        public:
            SurfaceInteraction() : Interaction() {}

            SurfaceInteraction(const Vector3F &point, const Vector3F &direction, const Vector3F &normal,
                               MediumInterface &mediumBoundary, Float u = 0, Float v = 0,
                               Material material = Material());

            void buildScatteringFunction(MemoryAllocator &allocator, TransportMode mode = TransportMode::RADIANCE);

            virtual void reset() override {
                Interaction::reset();
                bsdf = nullptr;
                _areaLight = nullptr;
            }

            // 击中点纹理坐标
            Float u, v;
            // 击中点处的 BSDF
            BSDF *bsdf = nullptr;
            BSSRDF *bssrdf = nullptr;

        protected:
            // 如果被击中物体是 AreaLight，则这一项应该被赋值
            AreaLight *_areaLight = nullptr;

        public:
            const AreaLight *getAreaLight() const;

            void setAreaLight(AreaLight *areaLight);
        };

        using medium::Medium;

        class MediumInteraction : public Interaction {
        public:
            MediumInteraction() : Interaction(), _phase(nullptr) {}

            MediumInteraction(const Vector3F &point, const Vector3F &direction,
                              const Medium *medium, const PhaseFunction *phase);

            bool isValid() const;

            virtual bool isMediumInteraction() const override {
                return true;
            }

            const PhaseFunction *getPhaseFunction() const {
                return _phase;
            }

        private:
            const Medium *_medium;
            const PhaseFunction *_phase;
        };

        class StartEndInteraction : public Interaction {
        public:
            union {
                const Camera *camera;
                const Light *light;
            };

            StartEndInteraction() : Interaction() {}

            StartEndInteraction(const Camera *camera, const Ray &ray);

            StartEndInteraction(const Light *light, const Interaction &interaction);

            /**
             * 创建 光源 / 相机 路径点
             * @param light 光源
             * @param p 点位置
             * @param dir 入射方向
             * @param n 击中点法线
             */
            StartEndInteraction(const Light *light, const Vector3F &p, const Vector3F &dir,
                                const Vector3F &n);

        };
    }
}

#endif //KAGUYA_INTERACTION_H
