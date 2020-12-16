//
// Created by Storm Phoenix on 2020/10/16.
//

#ifndef KAGUYA_INTERACTION_H
#define KAGUYA_INTERACTION_H

#include <kaguya/core/Core.h>
#include <kaguya/core/phase/PhaseFunction.h>
#include <kaguya/core/medium/MediumBound.h>
#include <kaguya/utils/MemoryArena.h>
#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/tracer/Ray.h>

namespace kaguya {

    namespace material {
        class Material;
    }

    namespace tracer {
        class Camera;
    }

    namespace core {
        class AreaLight;

        class Light;

        using medium::MediumBound;
        using kaguya::tracer::Camera;
        using kaguya::tracer::Ray;
        using kaguya::material::Material;
        using kaguya::memory::MemoryArena;

        /**
        * Ray 与 Scene 的交点记录
        */
        class Interaction {
        public:
            Interaction() : _mediumBoundary(nullptr, nullptr) {}

            Interaction(const Vector3 &point, const Vector3 &direction, const Vector3 &normal,
                        const MediumBound &mediumBoundary, Material *material = nullptr);

            const Vector3 getDirection() const {
                return _direction;
            }

            const Vector3 getPoint() const {
                return _point;
            }

            const Vector3 getNormal() const {
                return _normal;
            }

            void setNormal(const Vector3 &normal) {
                _normal = normal;
            }

            const Material *getMaterial() const {
                return _material;
            }

            void setMaterial(Material *material) {
                _material = material;
            }

            void setPoint(const Vector3 &point) {
                _point = point;
            }

            void setId(long long id) {
//                reset();
                _id = id;
            }

            /**
             * 检测是否是体积碰撞
             * @return
             */
            virtual bool isMediumInteraction() const {
                return false;
            }

            /**
             * Generate ray alone @param dir from origin
             * @param dir
             */
            virtual Ray sendRay(const Vector3 &dir) const;

            virtual Ray sendRayTo(const Interaction &it) const;

            void setMediumBoundary(MediumBound &mediumBoundary) {
                _mediumBoundary = mediumBoundary;
            }

        protected:
            /**
             * 清空 Interaction 中部分属性
             */
            virtual void reset() {
                _material = nullptr;
            }

        protected:
            // 发生 Interaction 的光线的方向
            Vector3 _direction;
            // 击中点
            Vector3 _point;
            // 击中点法线方向，发现永远指向物体表面外侧
            Vector3 _normal;
            // 击中材质种类
            Material *_material = nullptr;
            // 击中物体的 ID
            long long _id = -1;
            // medium boundary
            MediumBound _mediumBoundary;
        };

        /**
         * 击中点为表面类型
         */
        class SurfaceInteraction : public Interaction {
        public:
            SurfaceInteraction() : Interaction() {}

            SurfaceInteraction(const Vector3 &point, const Vector3 &direction, const Vector3 &normal,
                               MediumBound &mediumBoundary, double u = 0, double v = 0,
                               Material *material = nullptr);

            BSDF *buildBSDF(MemoryArena &memoryArena, TransportMode mode = TransportMode::RADIANCE);

            /**
             * TODO delete
             * 设置击中位置处的法线
             * @param outwardNormal
             * @param hitDirection
             */
            void setOutwardNormal(const Vector3 &outwardNormal, const Vector3 &hitDirection) {
                _direction = hitDirection;
                _normal = outwardNormal;
            }

            const double getU() {
                return _u;
            }

            void setU(double u) {
                _u = u;
            }

            const double getV() {
                return _v;
            }

            void setV(double v) {
                _v = v;
            }

            const AreaLight *getAreaLight() const {
                return _areaLight;
            }

            void setAreaLight(AreaLight *areaLight) {
                _areaLight = areaLight;
            }

            const BSDF *getBSDF() const {
                return _bsdf;
            }

            virtual void reset() override {
                Interaction::reset();
                _bsdf = nullptr;
                _areaLight = nullptr;
            }

        protected:
            // 击中点纹理坐标
            double _u;
            double _v;
            // 击中点处的 BSDF
            BSDF *_bsdf = nullptr;
            // 如果被击中物体是 AreaLight，则这一项应该被赋值
            AreaLight *_areaLight = nullptr;
        };

        using kaguya::core::medium::Medium;

        class MediumInteraction : public Interaction {
        public:
            MediumInteraction() : Interaction(), _phase(nullptr) {}

            MediumInteraction(const Vector3 &point, const Vector3 &direction,
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
            StartEndInteraction(const Light *light, const Vector3 &p, const Vector3 &dir,
                                const Vector3 &n);

        };
    }
}

#endif //KAGUYA_INTERACTION_H
