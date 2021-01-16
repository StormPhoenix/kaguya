//
// Created by Storm Phoenix on 2020/10/2.
//

#ifndef KAGUYA_WALL_H
#define KAGUYA_WALL_H

#include <kaguya/core/Transform.h>
#include <kaguya/scene/meta/Shape.h>
#include <kaguya/scene/accumulation/AABB.h>

namespace kaguya {
    namespace scene {
        namespace meta {
            using core::transform::Transform;
            using kaguya::scene::acc::AABB;

            class Wall : public Shape {
            public:
                Wall() {}

                // XY 上的二维片面
                Wall(Float width, Float height,
                     std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>());

                virtual bool intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const override;

                virtual const AABB &bound() const override;

                virtual Float area() const override;

                virtual SurfaceInteraction sampleSurfacePoint(Sampler *sampler1D) const override;

                virtual Float surfacePointPdf(const SurfaceInteraction &si) const override;

                virtual ~Wall() {}

            protected:
                /**
                 * build bounding box
                 */
                virtual void init();

            private:
                Float _width;
                Float _height;
                Vector3F _leftBottom;
                Vector3F _leftTop;
                Vector3F _rightBottom;
                Vector3F _rightTop;

                Vector3F _transformedLeftBottom;
                Vector3F _transformedRightBottom;
                Vector3F _transformedRightTop;
                Vector3F _transformedLeftTop;
                Vector3F _transformedNormal;

                std::shared_ptr<Transform> _transformMatrix = nullptr;
                std::shared_ptr<Transform> _invTransformMatrix = nullptr;

            protected:
                Vector3F _normal;
                AABB _aabb;
                Float _area;
            };

            class ZXWall : public Wall {
            public:
                /**
                 * ZX 2D 平面
                 * @param z0
                 * @param z1
                 * @param x0
                 * @param x1
                 * @param y
                 * @param upward
                 */
                ZXWall(Float z0, Float z1, Float x0, Float x1, Float y, bool upward);

                virtual void init() override;

                virtual bool intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const override;

                virtual Float area() const override;

                virtual SurfaceInteraction sampleSurfacePoint(Sampler *sampler1D) const override;

                virtual Float surfacePointPdf(const SurfaceInteraction &si) const override;

            protected:
                Float _y;
                Float _z0;
                Float _z1;
                Float _x0;
                Float _x1;
            };

            class YZWall : public Wall {
            public:
                /**
                 * YZ wall
                 * @param y0
                 * @param y1
                 * @param z0
                 * @param z1
                 * @param x
                 * @param rightward
                 */
                YZWall(Float y0, Float y1, Float z0, Float z1, Float x, bool rightward);

                virtual void init() override;

                virtual bool intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const override;

                virtual Float area() const override;

                virtual SurfaceInteraction sampleSurfacePoint(Sampler *sampler1D) const override;

                virtual Float surfacePointPdf(const SurfaceInteraction &point) const override;

            protected:
                Float _x;
                Float _y0;
                Float _y1;
                Float _z0;
                Float _z1;
            };

            class XYWall : public Wall {
            public:
                /**
                 * XY 2D 平面
                 * @param x0
                 * @param x1
                 * @param y0
                 * @param y1
                 * @param z
                 * @param frontward
                 */
                XYWall(Float x0, Float x1, Float y0, Float y1, Float z, bool frontward);

                virtual void init() override;

                virtual bool intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const override;

                virtual Float area() const override;

                virtual SurfaceInteraction sampleSurfacePoint(Sampler *sampler1D) const override;

                virtual Float surfacePointPdf(const SurfaceInteraction &si) const override;

            protected:
                Float _z;
                Float _x0;
                Float _x1;
                Float _y0;
                Float _y1;
            };
        }
    }
}

#endif //KAGUYA_WALL_H
