//
// Created by Storm Phoenix on 2020/10/10.
//

#ifndef KAGUYA_VERTEX_H
#define KAGUYA_VERTEX_H

#include <kaguya/math/Math.h>

namespace kaguya {
    namespace scene {

        struct Vertex {
            // 定点坐标
            Vector3d position;
            // 法线
            Vector3d normal;
            // 纹理坐标
            double u, v;
        };

    }
}

#endif //KAGUYA_VERTEX_H
