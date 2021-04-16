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
                typedef std::shared_ptr<Transform> Ptr;

                Transform();

                Transform(Matrix4F transformMatrix);

                Vector3F transformPoint(const Vector3F &p) const;

                Vector3F transformVector(const Vector3F &v) const;

                Vector3F transformNormal(const Vector3F &n) const;

                Matrix4F mat() const;

                std::shared_ptr<Transform> inverse() const;

            private:
                bool _identity = false;
                Matrix4F _transformMatrix;
            };

        }
    }
}

#endif //KAGUYA_TRANSFORM_H
