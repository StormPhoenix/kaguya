//
// Created by Storm Phoenix on 2020/12/21.
//

#ifndef KAGUYA_TRANSFORM_H
#define KAGUYA_TRANSFORM_H

#include <kaguya/core/Core.h>

#include <cstdlib>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace transform {

            class Transform {
            public:
                typedef std::shared_ptr<Transform> Ptr;

                Transform();

                Transform(Matrix4F transformMatrix);

                Vector3F transformPoint(const Point3F &p) const;

                Vector3F transformVector(const Vector3F &v) const;

                Vector3F transformNormal(const Vector3F &n) const;

                Matrix4F mat() const;

                Transform inverse() const;

                Transform operator*(const Transform &t) const;

                Ptr ptr() const;

                static Transform perspective(Float fov, Float nearClip, Float farClip);

                static Transform translate(Float x, Float y, Float z);

                static Transform scale(Float sx, Float sy, Float sz);

                static Transform lookAt(Point3F origin, Point3F target, Vector3F up);

            private:
                bool _identity = false;
                Matrix4F _transformMatrix;
            };

        }
    }
}

#endif //KAGUYA_TRANSFORM_H
