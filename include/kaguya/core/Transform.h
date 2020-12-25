//
// Created by Storm Phoenix on 2020/12/21.
//

#ifndef KAGUYA_TRANSFORM_H
#define KAGUYA_TRANSFORM_H

#include <kaguya/core/Core.h>

#include <cstdlib>

namespace kaguya {
    namespace core {
        namespace transform {

            class Transform {
            public:
                Transform();

                Transform(Matrix4 transformMatrix);

                Vector3 transformPoint(const Vector3 &p) const;

                Vector3 transformVector(const Vector3 &v) const;

                Vector3 transformNormal(const Vector3 &n) const;

                std::shared_ptr<Transform> inverse() const;

            private:
                bool _identity = false;
                Matrix4 _transformMatrix;
            };

        }
    }
}

#endif //KAGUYA_TRANSFORM_H
