//
// Created by Storm Phoenix on 2020/12/20.
//

#ifndef KAGUYA_CUBE_H
#define KAGUYA_CUBE_H

#include <kaguya/scene/TriangleMesh.h>

namespace kaguya {
    namespace scene {
        class Cube : public TriangleMesh {
        public:
            typedef std::shared_ptr<Cube> Ptr;

            Cube(Matrix4F tranformMat);

        private:
            void build();

        private:
            // transform matrix
            Matrix4F _transformMat;
        };

    }
}

#endif //KAGUYA_CUBE_H
