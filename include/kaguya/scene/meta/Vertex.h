//
// Created by Storm Phoenix on 2020/10/10.
//

#ifndef KAGUYA_VERTEX_H
#define KAGUYA_VERTEX_H

#include <kaguya/math/Math.hpp>

namespace kaguya {
    namespace scene {

        /**
         * 顶点类型
         */
        typedef enum VertexTypeEnum {
            CAMERA, LIGHT, SURFACE, MEDIUM
        } VertexTypeEnum;

        struct Vertex {
            // 定点坐标
            Vector3 position;
            // 法线
            Vector3 normal;
            // 纹理坐标
            double u, v;
            // 顶点类型
            VertexTypeEnum vertexType;
        };

    }
}

#endif //KAGUYA_VERTEX_H
