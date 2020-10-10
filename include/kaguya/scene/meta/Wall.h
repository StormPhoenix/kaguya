//
// Created by Storm Phoenix on 2020/10/2.
//

#ifndef KAGUYA_WALL_H
#define KAGUYA_WALL_H

#include <kaguya/scene/Hittable.h>
#include <kaguya/scene/ObjectSampler.h>
#include <kaguya/scene/accumulation/AABB.h>

namespace kaguya {
    namespace scene {

        using kaguya::scene::acc::AABB;

        class Wall : public ObjectSampler {
        public:
            Wall() {}

            // XY 上的二维片面
            Wall(float width, float height, std::shared_ptr<Material> material,
                 std::shared_ptr<Matrix4> transformMatrix = nullptr);

            virtual bool hit(const Ray &ray, HitRecord &hitRecord, double stepMin, double stepMax) override;

            virtual const AABB &boundingBox() const override;

            virtual Vector3 samplePoint(double &pdf, Vector3 &normal) override;

            virtual double samplePointPdf(Vector3 &point) override;

            const Vector3 &getNormal() {
                return _normal;
            }

        protected:
            /**
             * build bounding box
             */
            virtual void buildBoundingBox();

        private:
            double _width;
            double _height;
            Vector3 _leftBottom;
            Vector3 _leftTop;
            Vector3 _rightBottom;
            Vector3 _rightTop;
            std::shared_ptr<Matrix4> _transformMatrix = nullptr;

        protected:
            std::shared_ptr<Material> _material = nullptr;
            Vector3 _normal;
            AABB _aabb;
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
            ZXWall(float z0, float z1, float x0, float x1, float y, bool upward, std::shared_ptr<Material> material);

            virtual void buildBoundingBox() override;

            virtual bool hit(const Ray &ray, HitRecord &hitRecord, double stepMin, double stepMax) override;

            virtual Vector3 samplePoint(double &pdf, Vector3 &normal) override;

            virtual double samplePointPdf(Vector3 &point) override;

        protected:
            float _y;
            float _z0;
            float _z1;
            float _x0;
            float _x1;
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
            YZWall(float y0, float y1, float z0, float z1, float x, bool rightward, std::shared_ptr<Material> material);

            virtual void buildBoundingBox() override;

            virtual bool hit(const Ray &ray, HitRecord &hitRecord, double stepMin, double stepMax) override;

            virtual Vector3 samplePoint(double &pdf, Vector3 &normal) override;

            virtual double samplePointPdf(Vector3 &point) override;

        protected:
            float _x;
            float _y0;
            float _y1;
            float _z0;
            float _z1;
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
            XYWall(float x0, float x1, float y0, float y1, float z, bool frontward, std::shared_ptr<Material> material);

            virtual void buildBoundingBox() override;

            virtual bool hit(const Ray &ray, HitRecord &hitRecord, double stepMin, double stepMax) override;

            virtual Vector3 samplePoint(double &pdf, Vector3 &normal) override;

            virtual double samplePointPdf(Vector3 &point) override;

        protected:
            float _z;
            float _x0;
            float _x1;
            float _y0;
            float _y1;
        };
    }
}

#endif //KAGUYA_WALL_H
