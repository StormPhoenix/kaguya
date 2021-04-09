//
// Created by Storm Phoenix on 2021/4/8.
//

#include <kaguya/tracer/pm/SPPMTracer.h>

namespace kaguya {
    namespace tracer {

        // TODO VisiblePoint struct

        // indexOfGrid(pos: Vector3F)

        /* TODO
         * 每次发射一堆 photons 却只处理一个 visible points，效率非常低下
         *
         * RenderPool 每次只渲染一个 pixel 的结构必须要重构了。这个不会很麻烦，只要将
         * RenderTask 里面的 func2D 修改成 range 形式即可（现在是按照 pixel-wise 的方式渲染的）
         **/
        std::function<void (const int, const int, Sampler *)> SPPMTracer::render() {
            // 创建保存 3D-Grid 的 HashMap 数据结构
            // Build pixel tile SPPM Pixels

            // Trace camera ray for each pixel
                // Find visible point
                    // Do intersection
                    // Sample direct light
                    // Judge visible points
                        // Save visible points into 3D-Grid
                    // Tracing 过程中计算 beta
                    // Continue tracing

            // 计算所有 visible points 占用的空间大小，并确定每个 grid size
            // 遍历所有的 visible points，加入 hash map

            // Trace photons
                // Loop depth
                    // Find diffuse material point, and search visible points
                        // 这里需要一个 HashMap 查找附近所有的 visible points

            // Density estimate
                // 估算光照，加入到 SPPM Pixel 里
                    // ?? Intensity 是如何体现在 photon 上面的
                    // 每收到一个 paritcle
                        // 增加 M ++
                        // 增加 Phi -> 粒子对 visible point 的贡献

                // 修改 R （radius）
                // 修改 Ld

            // Writing file plane
        }

    }
}