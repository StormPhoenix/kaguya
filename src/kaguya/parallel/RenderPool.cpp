//
// Created by Storm Phoenix on 2020/11/17.
//

#include <kaguya/Config.h>
#include <kaguya/parallel/RenderPool.h>

#include <cassert>
#include <cstdlib>

namespace RENDER_NAMESPACE {
    namespace parallel {

        thread_local int threadIdx;

        int maxKernelCores() {
            return std::max(1u, std::thread::hardware_concurrency());
        }

        int renderCores() {
            return Config::Parallel::kernelCount <= 0 ? maxKernelCores() : Config::Parallel::kernelCount;
        }

        void parallelFor1D(const std::function<void(const int)> func, int count, int chunkSize) {
            RenderPool *pool = RenderPool::getInstance();
            pool->addRenderTask1D(func, count, chunkSize);
        }

        void parallelFor2D(const std::function<void(const int, const int)> func, Point2I size) {
            RenderPool *pool = RenderPool::getInstance();
            pool->addRenderTask2D(func, size[0], size[1]);
        }

        bool RenderPool::_shutdown = false;

        RenderPool *RenderPool::_pool = nullptr;

        std::mutex RenderPool::_poolMutex;

        std::mutex RenderPool::_taskMutex;

        std::condition_variable RenderPool::_taskCondition;

        RenderTaskNode *RenderPool::_taskQueue = nullptr;

        RenderPool *RenderPool::getInstance() {
            if (_pool == nullptr) {
                {
                    std::lock_guard<std::mutex> poolLock(_poolMutex);
                    if (_pool == nullptr) {
                        _pool = new RenderPool(renderCores());
                    }
                }
            }
            return _pool;
        }

        void RenderPool::shutdown() {
            _shutdown = true;
            _taskCondition.notify_all();
        }

        void RenderPool::addRenderTask1D(const std::function<void(const int)> func1D, int taskCount, int chunkSize) {
            ASSERT(_threadCount > 0, "Thread count > 0.");

            RenderTask *task = new RenderTask(func1D, taskCount, chunkSize);
            RenderTaskNode *node = new RenderTaskNode(task);
            {
                // lock on task queue when add task
                std::lock_guard<std::mutex> lock(_taskMutex);
                node->next = _taskQueue;
                _taskQueue = node;
            }
            _taskCondition.notify_all();
            task->waitUntilFinished();
        }

        void RenderPool::addRenderTask2D(const std::function<void(const int, const int)> func2D, int nX, int nY) {
            ASSERT(_threadCount > 0, "Thread count > 0.");

            RenderTask *task = new RenderTask(func2D, nX, nY);
            RenderTaskNode *node = new RenderTaskNode(task);
            {
                // lock on task queue when add task
                std::lock_guard<std::mutex> lock(_taskMutex);
                node->next = _taskQueue;
                _taskQueue = node;
            }
            _taskCondition.notify_all();
            task->waitUntilFinished();
        }

        void RenderPool::renderFunc(const int threadId,
                                    std::shared_ptr<Barrier> barrier) {
            threadIdx = threadId;
            // wait until all thread reachs to the barrier
            barrier->wait();

            // when notified, release the barrier
            barrier.reset();

            std::unique_lock<std::mutex> lock(_taskMutex);
            // running rendering function
            while (!_shutdown) {
                if (_taskQueue == nullptr) {
                    // sleep for empty task queue
                    _taskCondition.wait(lock);
                } else {
                    // acquire task
                    RenderTaskNode *taskNode = _taskQueue;
                    ASSERT(taskNode->task != nullptr, "Task is nullptr.");

                    RenderTask *task = taskNode->task;

                    int idx1, idx2;
                    auto mode = task->mode();
                    bool assigned = false;

                    // Assigned task
                    if (mode == RenderTask::One_Dim) {
                        assigned = task->assignTask1D(idx1, idx2);
                    } else if (mode == RenderTask::Two_Dim) {
                        assigned = task->assignTask2D(idx1, idx2);
                    } else {
                        ASSERT(false, "Task mode not support.");
                    }

                    if (assigned) {
                        task->renderEnter();
                        lock.unlock();

                        // Execute task
                        if (mode == RenderTask::One_Dim) {
                            for (int idx = idx1; idx <= idx2; idx++) {
                                task->func1D(idx);
                            }
                        } else if (mode == RenderTask::Two_Dim) {
                            task->func2D(idx1, idx2);
                        }

                        lock.lock();
                        task->renderLeave();
                        if (task->isFinished()) {
                            task->notifyMaster();
                            if (_taskQueue == taskNode) {
                                _taskQueue = _taskQueue->next;
                            }

                            delete task;
                            delete taskNode;
                            _taskCondition.notify_all();
                        }
                    } else {
                        _taskQueue = taskNode->next;
                    }
                }
            }
        }

        RenderPool::RenderPool(int threadCount) : _threadCount(threadCount) {
            assert(_threadCount > 0);

            std::shared_ptr<Barrier> barrier = std::make_shared<Barrier>(_threadCount + 1);
            // initialize threads
            for (int i = 0; i < _threadCount; i++) {
                _threads.push_back(std::thread(renderFunc, i, barrier));
            }

            // wait until all threads are created.
            barrier->wait();
        }

        RenderPool::~RenderPool() {
            RenderTaskNode *node = nullptr;
            while (_taskQueue != nullptr) {
                node = _taskQueue->next;
                delete _taskQueue->task;
                delete _taskQueue;
                _taskQueue = node;
            }
            _pool = nullptr;
        }
    }
}