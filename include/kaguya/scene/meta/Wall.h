//
// Created by Storm Phoenix on 2020/10/2.
//

#ifndef KAGUYA_WALL_H
#define KAGUYA_WALL_H

#include <kaguya/scene/ShapeSampler.h>
#include <kaguya/scene/accumulation/AABB.h>

namespace kaguya {
    namespace scene {

        using kaguya::scene::acc::AABB;

        class Wall : public ShapeSampler {
        public:
            Wall() {}

            // XY 上的二维片面
            Wall(double width, double height, std::shared_ptr<Material> material,
                 std::shared_ptr<Matrix4> transformMatrix = nullptr);

            virtual bool insect(const Ray &ray, SurfaceInteraction &hitRecord, double stepMin, double stepMax) override;

            virtual const AABB &boundingBox() const override;

            virtual double area() override;

            virtual SurfaceInteraction sampleSurfacePoint() override;

            virtual double surfacePointPdf(SurfaceInteraction &si) override;

            virtual ~Wall() {}

        protected:
            /**
             * build bounding box
             */
            virtual void init();

        private:
            double _width;
            double _height;
            Vector3 _leftBottom;
            Vector3 _leftTop;
            Vector3 _rightBottom;
            Vector3 _rightTop;

            Vector3 _transformedLeftBottom;
            Vector3 _transformedRightBottom;
            Vector3 _transformedRightTop;
            Vector3 _transformedLeftTop;
            Vector3 _transformedNormal;

            std::shared_ptr<Matrix4> _transformMatrix = nullptr;

        protected:
            std::shared_ptr<Material> _material = nullptr;
            Vector3 _normal;
            AABB _aabb;
            double _area;
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
             * @param material
             */
            ZXWall(double z0, double z1, double x0, double x1, double y, bool upward,
                   std::shared_ptr<Material> material);

            virtual void init() override;

            virtual bool insect(const Ray &ray, SurfaceInteraction &hitRecord, double stepMin, double stepMax) override;

            virtual double area() override;

            virtual SurfaceInteraction sampleSurfacePoint() override;

            virtual double surfacePointPdf(SurfaceInteraction &si) override;

        protected:
            double _y;
            double _z0;
            double _z1;
            double _x0;
            double _x1;
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
             * @param material
             */
            YZWall(double y0, double y1, double z0, double z1, double x, bool rightward,
                   std::shared_ptr<Material> material);

            virtual void init() override;

            virtual bool insect(const Ray &ray, SurfaceInteraction &hitRecord, double stepMin, double stepMax) override;

            virtual double area() override;

            virtual SurfaceInteraction sampleSurfacePoint() override;

            virtual double surfacePointPdf(SurfaceInteraction &point) override;

        protected:
            double _x;
            double _y0;
            double _y1;
            double _z0;
            double _z1;
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
             * @param material
             */
            XYWall(double x0, double x1, double y0, double y1, double z, bool frontward,
                   std::shared_ptr<Material> material);

            virtual void init() override;

            virtual bool insect(const Ray &ray, SurfaceInteraction &hitRecord, double stepMin, double stepMax) override;

            virtual double area() override;

            virtual SurfaceInteraction sampleSurfacePoint() override;

            virtual double surfacePointPdf(SurfaceInteraction &si) override;

        protected:
            double _z;
            double _x0;
            double _x1;
            double _y0;
            double _y1;
        };
    }
}

#endif //KAGUYA_WALL_H
