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

                Transform(Matrix4d transformMatrix);

                Vector3d transformPoint(const Vector3d &p) const;

                Vector3d transformVector(const Vector3d &v) const;

                Vector3d transformNormal(const Vector3d &n) const;

                std::shared_ptr<Transform> inverse() const;

            private:
                bool _identity = false;
                Matrix4d _transformMatrix;
            };

        }
    }
}

#endif //KAGUYA_TRANSFORM_H
