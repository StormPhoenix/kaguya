//
// Created by Storm Phoenix on 2020/10/16.
//

#ifndef KAGUYA_INTERACTION_H
#define KAGUYA_INTERACTION_H

#include <kaguya/core/Core.h>
#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/medium/MediumBound.h>
#include <kaguya/core/phase/PhaseFunction.h>
#include <kaguya/utils/MemoryArena.h>
#include <kaguya/tracer/Ray.h>

namespace kaguya {

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
        class AreaLight;

        class Light;

        using scene::Intersectable;
        using medium::Medium;
        using medium::MediumBound;
        using kaguya::tracer::Camera;
        using kaguya::tracer::Ray;
        using kaguya::material::Material;
        using kaguya::memory::MemoryArena;

        inline Vector3d offsetOrigin(const Vector3d &origin, const Vector3d &error,
                                     const Vector3d &normal, const Vector3d &direction);


        /**
        * Ray 与 Scene 的交点记录
        */
        class Interaction {
        public:
            Interaction() : _mediumBoundary(nullptr, nullptr) {}

            Interaction(const Vector3d &point, const Vector3d &direction, const Vector3d &normal,
                        const MediumBound &mediumBoundary, Material *material = nullptr);

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
            virtual Ray sendRay(const Vector3d &dir) const;

            virtual Ray sendRayTo(const Point3d &target) const;

            virtual Ray sendRayTo(const Interaction &it) const;

            void setMediumBoundary(const MediumBound &mediumBoundary) {
                _mediumBoundary = mediumBoundary;
            }

        protected:
            /**
             * 清空 Interaction 中部分属性
             */
            virtual void reset() {
                _material = nullptr;
            }

            const Medium *getMedium(const Vector3d &dir) const;

        protected:
            // 发生 Interaction 的光线的方向
            Vector3d _direction;
            // 击中点
            Vector3d _point;
            // 击中点法线方向，发现永远指向物体表面外侧
            Vector3d _normal = Vector3d(0., 0., 0.);
            // 击中材质种类
            Material *_material = nullptr;
            // medium boundary
            MediumBound _mediumBoundary;
            // Error range
            Vector3d _error = Vector3d(0., 0., 0.);

        public:
            const Vector3d getDirection() const {
                return _direction;
            }

            const Vector3d getPoint() const {
                return _point;
            }

            const Vector3d getNormal() const {
                return _normal;
            }

            const Vector3d getError() const {
                return _error;
            }

            void setNormal(const Vector3d &normal) {
                _normal = normal;
            }

            const Material *getMaterial() const {
                return _material;
            }

            void setMaterial(Material *material) {
                _material = material;
            }

            void setPoint(const Vector3d &point) {
                _point = point;
            }

            void setError(const Vector3d &error) {
                _error = error;
            }
        };

        /**
         * 击中点为表面类型
         */
        class SurfaceInteraction : public Interaction {
        public:
            SurfaceInteraction() : Interaction() {}

            SurfaceInteraction(const Vector3d &point, const Vector3d &direction, const Vector3d &normal,
                               MediumBound &mediumBoundary, double u = 0, double v = 0,
                               Material *material = nullptr);

            BSDF *buildBSDF(MemoryArena &memoryArena, TransportMode mode = TransportMode::RADIANCE);

            /**
             * TODO delete
             * 设置击中位置处的法线
             * @param outwardNormal
             * @param hitDirection
             */
            void setOutwardNormal(const Vector3d &outwardNormal, const Vector3d &hitDirection) {
                _direction = hitDirection;
                _normal = outwardNormal;
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
            // TODO modify to geometry
            const Intersectable *_geometry;

        public:
            void setGeometry(const Intersectable *geometry) {
                _geometry = geometry;
            }

            const Intersectable *getGeometry() const {
                return _geometry;
            }

            void setDirection(const Vector3d &direction) {
                _direction = direction;
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

            BSDF *getBSDF() const {
                return _bsdf;
            }

            void setBSDF(BSDF *bsdf) {
                _bsdf = bsdf;
            }
        };

        using medium::Medium;

        class MediumInteraction : public Interaction {
        public:
            MediumInteraction() : Interaction(), _phase(nullptr) {}

            MediumInteraction(const Vector3d &point, const Vector3d &direction,
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
            StartEndInteraction(const Light *light, const Vector3d &p, const Vector3d &dir,
                                const Vector3d &n);

        };
    }
}

#endif //KAGUYA_INTERACTION_H
