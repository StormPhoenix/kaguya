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
                Transform(std::shared_ptr<Matrix4> transformMatrix);

                Vector3 transformPoint(const Vector3 &point) const;

                Vector3 transformVector(const Vector3 &vector) const;

                Vector3 transformNormal(const Vector3 &vector3) const;

                std::shared_ptr<Transform> & inverse() const;

            private:
                std::shared_ptr<Matrix4> _transformMatrix;
            };

        }
    }
}

#endif //KAGUYA_TRANSFORM_H
