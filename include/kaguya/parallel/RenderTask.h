//
// Created by Storm Phoenix on 2020/11/17.
//

#ifndef KAGUYA_RENDERTASK_H
#define KAGUYA_RENDERTASK_H

#include <kaguya/math/Sampler.hpp>

#include <condition_variable>
#include <functional>

namespace kaguya {
    namespace parallel {

        using kaguya::math::random::Sampler;

        class RenderTask {
        public:
            RenderTask(const std::function<void(const int, const int, const Sampler *)> &func2D,
                       int renderWidth, int renderHeight);

/**
             * rendering task assignment
             *
             * @param rowStart
             * @param rowEnd
             * @param colStart
             * @param colEnd
             * @return return false if all render task is allocated
             */
            bool renderRange(int &rowStart, int &rowEnd,
                             int &colStart, int &colEnd);

            bool isFinished();

            void waitUntilFinished();

            void notifyMaster();

            ~RenderTask();

        public:
            // render function
            const std::function<void(const int, const int,const Sampler *)> func2D;

            // next render task
            RenderTask *next = nullptr;

            // active render
            int activeRender = 0;

        private:
            const int _renderWidth;
            const int _renderHeight;
            // task mutex
            std::mutex _finishMutex;
            // task condition
            std::condition_variable _finishCondition;
            // next render range
            int _nextTileXCount;
            int _nextTileYCount;

            const int _tileSize = 20;

            // total tile count
            int _totalTileXCount;
            int _totalTileYCount;

            int tileRecord = 0;
        };

    }
}

#endif //KAGUYA_RENDERTASK_H
