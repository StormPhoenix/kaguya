//
// Created by Storm Phoenix on 2020/11/17.
//

#ifndef KAGUYA_RENDERPOOL_H
#define KAGUYA_RENDERPOOL_H

#include <kaguya/math/Sampler.hpp>

#include <kaguya/parallel/Barrier.h>
#include <kaguya/parallel/RenderTask.h>

#include <condition_variable>
#include <thread>
#include <vector>

namespace kaguya {
    namespace parallel {

        class RenderPool {
        public:
            /**
             * single instance
             * @return
             */
            static RenderPool *getInstance();

        public:
            void addRenderTask(std::function<void(int, int, const Sampler *)> func2D,
                               int renderWidth, int renderHeight);

            void shutdown();

            // clean all thread resource
            void cleanThreads();

            ~RenderPool();

        private:
            // constructor
            RenderPool(int threadCount);

        private:
            // renderFunc for each thread
            static void renderFunc(const int threadId,
                                   std::shared_ptr<Barrier> barrier);

        private:
            // is _pool shutdown
            static bool _shutdown;

            // _pool instance
            static RenderPool *_pool;

            // lock for _pool creation
            static std::mutex _poolMutex;

            // lock for task queue
            static std::mutex _taskMutex;

            // rendering task queue
            static RenderTask *_taskQueue;

            // notify condition for task queue
            static std::condition_variable _taskCondition;

        private:
            // thread count
            const int _threadCount;

            // thread _pool
            std::vector<std::thread> _threads;

        };

    }
}

#endif //KAGUYA_RENDERPOOL_H
