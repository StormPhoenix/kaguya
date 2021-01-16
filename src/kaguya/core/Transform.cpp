//
// Created by Storm Phoenix on 2020/12/21.
//

#include <kaguya/core/Transform.h>

namespace kaguya {
    namespace core {
        namespace transform {
            Transform::Transform() : _identity(true) {}

            Transform::Transform(Matrix4d transformMatrix) :
                    _transformMatrix(transformMatrix),
                    _identity(false) {}

            Vector3F Transform::transformPoint(const Vector3F &p) const {
                if (_identity) {
                    return p;
                }

                return _transformMatrix * Vector4d(p, 1.);
            }

            Vector3F Transform::transformVector(const Vector3F &v) const {
                if (_identity) {
                    return v;
                }

                return _transformMatrix * Vector4d(v, 0.);
            }

            Vector3F Transform::transformNormal(const Vector3F &n) const {
                if (_identity) {
                    return n;
                }
                Vector3F ret = INVERSE_TRANSPOSE(_transformMatrix) * Vector4d(n, 0.0f);
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