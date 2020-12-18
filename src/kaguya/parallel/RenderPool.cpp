//
// Created by Storm Phoenix on 2020/11/17.
//

#include <kaguya/Config.h>
#include <kaguya/parallel/RenderPool.h>

#include <cassert>
#include <cstdlib>

namespace kaguya {
    namespace parallel {

        bool RenderPool::_shutdown = false;

        RenderPool *RenderPool::_pool = nullptr;

        std::mutex RenderPool::_poolMutex;

        std::mutex RenderPool::_taskMutex;

        std::condition_variable RenderPool::_taskCondition;

        RenderTask *RenderPool::_taskQueue = nullptr;

        RenderPool *RenderPool::getInstance() {
            const int threadsCount = Config::kernelCount;
            if (_pool == nullptr) {
                {
                    std::lock_guard<std::mutex> poolLock(_poolMutex);
                    if (_pool == nullptr) {
                        _pool = new RenderPool(threadsCount);
                    }
                }
            }
            return _pool;
        }

        void RenderPool::shutdown() {
            _shutdown = true;
            _taskCondition.notify_all();
        }

        void RenderPool::renderFunc(const int threadId,
                                    std::shared_ptr<Barrier> barrier) {
            srand(threadId);
            // wait until all thread reachs to the barrier
            barrier->wait();

            // when notified, release the barrier
            barrier.reset();

            // create sampler2d
            Sampler1D *sampler1D = Sampler1D::newInstance();

            std::unique_lock<std::mutex> lock(_taskMutex);
            // running rendering function
            while (!_shutdown) {
                if (_taskQueue == nullptr) {
                    // sleep
                    _taskCondition.wait(lock);
                } else {
                    // acquire task
                    RenderTask *task = _taskQueue;

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
                            _taskCondition.notify_all();
                        }
                    } else {
                        _taskQueue = task->next;
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

        void RenderPool::addRenderTask(std::function<void(const int, const int, const Sampler1D *)> func2D,
                                       int renderWidth, int renderHeight) {
            assert(_threadCount > 0);

            RenderTask *task = new RenderTask(std::move(func2D), renderWidth, renderHeight);
            {
                // lock on task queue when add task
                std::lock_guard<std::mutex> lock(_taskMutex);
                task->next = _taskQueue;
                _taskQueue = task;
            }

            _taskCondition.notify_all();
            task->waitUntilFinished();
        }

        RenderPool::~RenderPool() {
            RenderTask *task = nullptr;
            while (_taskQueue != nullptr) {
                task = _taskQueue->next;
                delete _taskQueue;
                _taskQueue = task;
            }
        }
    }
}