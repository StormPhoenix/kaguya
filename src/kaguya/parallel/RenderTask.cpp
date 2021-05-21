//
// Created by Storm Phoenix on 2020/11/18.
//

#include <kaguya/parallel/RenderTask.h>
#include <iostream>

namespace RENDER_NAMESPACE {
    namespace parallel {

        RenderTask::RenderTask(const std::function<void(const int)> func1D, int taskCount, int chunkSize)
                : func1D(std::move(func1D)),
                  _maxTaskCount(taskCount),
                  _nextTaskIndex(0),
                  _chunkSize(chunkSize),
                  _mode(One_Dim) {
            ASSERT(_maxTaskCount > 0 && _chunkSize > 0,
                   "Render task count > 0 and chunkSize > 0 (defaut 1)");
        }

        RenderTask::RenderTask(const std::function<void(const int, const int)> func2D, int nX, int nY)
                : func2D(std::move(func2D)),
                  _nX(nX), _nY(nY),
                  _maxTaskCount(nX * nY),
                  _nextTaskIndex(0),
                  _chunkSize(1),
                  _mode(Two_Dim) {
            ASSERT(_nX > 0 && _nY > 0,
                   "Render 2D task _nX > 0 and _nY > 0 (default 1)");
        }

        bool RenderTask::assignTask1D(int &idxStart, int &idxEnd) {
            if (_mode != One_Dim || _nextTaskIndex >= _maxTaskCount) {
                return false;
            }

            idxStart = _nextTaskIndex;
            _nextTaskIndex += _chunkSize;
            idxEnd = std::min(_nextTaskIndex - 1, _maxTaskCount - 1);
            return true;
        }

        bool RenderTask::assignTask2D(int &idxX, int &idxY) {
            if (_mode != Two_Dim || _nextTaskIndex >= _maxTaskCount) {
                return false;
            }

            int idx = _nextTaskIndex;
            _nextTaskIndex += _chunkSize;

            idxX = idx % _nX;
            idxY = idx / _nX;
            return true;
        }

        RenderTask::TaskMode RenderTask::mode() {
            return _mode;
        }

        bool RenderTask::isFinished() {
            return _nextTaskIndex >= _maxTaskCount && (_activeRender == 0);
        }

        void RenderTask::renderEnter() {
            _activeRender++;
        }

        void RenderTask::renderLeave() {
            _activeRender--;
        }

        void RenderTask::waitUntilFinished() {
            std::unique_lock<std::mutex> lock(_taskFinishedMutex);
            _finishCondition.wait(lock);
        }

        void RenderTask::notifyMaster() {
            _finishCondition.notify_all();
        }

        RenderTask::~RenderTask() {}

    }
}