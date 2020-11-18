//
// Created by Storm Phoenix on 2020/11/17.
//

#include <kaguya/parallel/RenderPool.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>

namespace kaguya {
    namespace parallel {

        bool RenderPool::shutdown = false;

        RenderPool *RenderPool::pool = nullptr;

        std::mutex RenderPool::poolMutex;

        std::mutex RenderPool::taskMutex;

        std::condition_variable RenderPool::taskCondition;

        RenderTask *RenderPool::taskQueue = nullptr;

        RenderPool *RenderPool::getInstance(int threadsCount) {
            if (pool == nullptr) {
                {
                    std::lock_guard<std::mutex> poolLock(poolMutex);
                    if (pool == nullptr) {
                        pool = new RenderPool(threadsCount);
                    }
                }
            }
            return pool;
        }

        void RenderPool::renderFunc(const int threadId,
                                    std::shared_ptr<Barrier> barrier) {
            srand(threadId);
            // wait until all thread reachs to the barrier
            barrier->wait();

            // when notified, release the barrier
            barrier.reset();

            // create sampler2d
            random::Sampler1D *sampler1D = random::Sampler1D::newInstance();

            std::unique_lock<std::mutex> lock(taskMutex);
            // running rendering function
            while (!shutdown) {
                if (taskQueue == nullptr) {
                    // sleep
                    taskCondition.wait(lock);
                } else {
                    // acquire task
                    RenderTask *task = taskQueue;

                    // get rendering task from task
                    int rowStart, rowEnd, colStart, colEnd;
                    if (task->renderRange(rowStart, rowEnd, colStart, colEnd)) {
                        task->activeRender++;
                        // release lock
                        lock.unlock();
                        for (int row = rowStart; row <= rowEnd; row++) {
                            for (int col = colStart; col <= colEnd; col++) {
                                task->func2D(row, col, sampler1D);
                            }
                        }

                        // lock
                        lock.lock();
                        task->activeRender--;
                        if (task->isFinished()) {
                            task->notifyMaster();
                            delete task;
                            taskCondition.notify_all();
                        }
                    } else {
                        taskQueue = task->next;
                    }
                }
            }
            delete sampler1D;
        }

        RenderPool::RenderPool(int threadCount) : _threadCount(threadCount) {
            assert(_threadCount >= 0);

            std::shared_ptr<Barrier> barrier = std::make_shared<Barrier>(_threadCount + 1);
            // initialize threads
            for (int i = 0; i < _threadCount; i++) {
                _threads.push_back(std::thread(renderFunc, i, barrier));
            }

            // wait until all threads are created.
            barrier->wait();
        }

        void RenderPool::addRenderTask(std::function<void(const int, const int, random::Sampler1D *)> func2D,
                                       int renderWidth, int renderHeight) {
            assert(_threadCount > 0);

            RenderTask *task = new RenderTask(std::move(func2D), renderWidth, renderHeight);
            {
                // lock on task queue when add task
                std::lock_guard<std::mutex> lock(taskMutex);
                task->next = taskQueue;
                taskQueue = task;
            }

            taskCondition.notify_all();
            task->waitUntilFinished();
        }
    }
}