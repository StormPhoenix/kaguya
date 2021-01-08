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

            Vector3d Transform::transformPoint(const Vector3d &p) const {
                if (_identity) {
                    return p;
                }

                return _transformMatrix * Vector4d(p, 1.);
            }

            Vector3d Transform::transformVector(const Vector3d &v) const {
                if (_identity) {
                    return v;
                }

                return _transformMatrix * Vector4d(v, 0.);
            }

            Vector3d Transform::transformNormal(const Vector3d &n) const {
                if (_identity) {
                    return n;
                }
                Vector3d ret = INVERSE_TRANSPOSE(_transformMatrix) * Vector4d(n, 0.0f);
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