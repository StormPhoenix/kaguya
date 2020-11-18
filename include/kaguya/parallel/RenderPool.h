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
             * @param threadsCount
             * @return
             */
            static RenderPool *getInstance(int threadsCount);

        public:
            void addRenderTask(std::function<void(int, int, random::Sampler1D *)> func2D,
                               int renderWidth, int renderHeight);

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
            // is pool shutdown
            static bool shutdown;

            // pool instance
            static RenderPool *pool;

            // lock for pool creation
            static std::mutex poolMutex;

            // lock for task queue
            static std::mutex taskMutex;

            // rendering task queue
            static RenderTask *taskQueue;

            // notify condition for task queue
            static std::condition_variable taskCondition;

        private:
            // thread count
            const int _threadCount;

            // thread pool
            std::vector<std::thread> _threads;

        };

    }
}

#endif //KAGUYA_RENDERPOOL_H
