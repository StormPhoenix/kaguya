//
// Created by Storm Phoenix on 2020/11/17.
//

#ifndef KAGUYA_RENDERTASK_H
#define KAGUYA_RENDERTASK_H

#include <kaguya/Config.h>

#include <condition_variable>
#include <functional>

namespace RENDER_NAMESPACE {
    namespace parallel {

        using sampler::Sampler;

        class RenderTask {
        public:

            // Task mode
            typedef enum TaskMode {
                One_Dim,
                Two_Dim
            } TaskMode;

            // For 1D task
            RenderTask(const std::function<void(const int)> func1D, int taskCount, int chunkSize = 1);

            // For 2D task
            RenderTask(const std::function<void(const int, const int)> func2D, int nX, int nY);

            // Assign rendering task index
            bool assignTask1D(int &idxStart, int &idxEnd);

            bool assignTask2D(int &idxX, int &idxY);

            TaskMode mode();

            void renderEnter();

            void renderLeave();

            bool isFinished();

            void waitUntilFinished();

            void notifyMaster();

            ~RenderTask();

            // render function
            const std::function<void(const int)> func1D;
            const std::function<void(const int, const int)> func2D;

        private:
            TaskMode _mode;

            int _maxTaskCount;
            int _nextTaskIndex;
            int _chunkSize;

            // For 2D render task
            int _nX = -1, _nY = -1;

            // Active render
            int _activeRender = 0;
            // task mutex
            std::mutex _taskFinishedMutex;
            // task condition
            std::condition_variable _finishCondition;
        };

    }
}

#endif //KAGUYA_RENDERTASK_H
