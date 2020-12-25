//
// Created by Storm Phoenix on 2020/12/21.
//

#include <kaguya/core/Transform.h>

namespace kaguya {
    namespace core {
        namespace transform {
            Transform::Transform() : _identity(true) {}

            Transform::Transform(Matrix4 transformMatrix) :
                    _transformMatrix(transformMatrix),
                    _identity(false) {}

            Vector3 Transform::transformPoint(const Vector3 &p) const {
                if (_identity) {
                    return p;
                }

                return _transformMatrix * Vector4(p, 1.);
            }

            Vector3 Transform::transformVector(const Vector3 &v) const {
                if (_identity) {
                    return v;
                }

                return _transformMatrix * Vector4(v, 0.);
            }

            Vector3 Transform::transformNormal(const Vector3 &n) const {
                if (_identity) {
                    return n;
                }
                Vector3 ret = INVERSE_TRANSPOSE(_transformMatrix) * Vector4(n, 0.0f);
                return NORMALIZE(ret);
            }

            std::shared_ptr<Transform> Transform::inverse() const {
                if (_identity) {
                    return std::make_shared<Transform>();
                }

                return std::make_shared<Transform>(INVERSE(_transformMatrix));
            }
        }
    }
}