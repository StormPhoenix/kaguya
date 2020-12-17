//
// Created by Storm Phoenix on 2020/11/18.
//

#include <kaguya/parallel/RenderTask.h>
#include <iostream>

namespace kaguya {
    namespace parallel {

        RenderTask::RenderTask(const std::function<void(const int, const int, const Sampler1D *)> &func2D,
                               int renderWidth, int renderHeight) :
                _renderHeight(renderHeight), _renderWidth(renderWidth),
                func2D(func2D) {

            _totalTileXCount = (_renderWidth + _tileSize - 1) / _tileSize;
            _totalTileYCount = (_renderHeight + _tileSize - 1) / _tileSize;

            _nextTileXCount = 0;
            _nextTileYCount = 0;
        }

        bool RenderTask::renderRange(int &rowStart, int &rowEnd, int &colStart, int &colEnd) {
            if (_nextTileXCount >= 0 && _nextTileXCount < _totalTileXCount &&
                _nextTileYCount >= 0 && _nextTileYCount < _totalTileYCount) {

                rowStart = _nextTileYCount * _tileSize;
                // range [0, _renderHeight - 1]
                rowEnd = (rowStart + _tileSize - 1) > (_renderHeight - 1) ?
                         (_renderHeight - 1) : (rowStart + _tileSize - 1);

                colStart = _nextTileXCount * _tileSize;
                // range [0, _renderWidth - 1]
                colEnd = (colStart + _tileSize - 1) > (_renderWidth - 1) ?
                         (_renderWidth - 1) : (colStart + _tileSize - 1);

                // update next tile
                if (_nextTileXCount + 1 >= _totalTileXCount) {
                    _nextTileXCount = 0;
                    _nextTileYCount++;
                } else {
                    _nextTileXCount++;
                }

                tileRecord++;
                std::cout << "\r" << float(tileRecord) * 100 / (_totalTileYCount * _totalTileXCount) << " %"
                          << std::flush;
                return true;
            } else {
                return false;
            }
        }

        bool RenderTask::isFinished() {
            return !(_nextTileXCount >= 0 && _nextTileXCount < _totalTileXCount &&
                     _nextTileYCount >= 0 && _nextTileYCount < _totalTileYCount) && (activeRender == 0);
        }

        void RenderTask::waitUntilFinished() {
            std::unique_lock<std::mutex> lock(_finishMutex);
            _finishCondition.wait(lock);
        }

        void RenderTask::notifyMaster() {
            _finishCondition.notify_all();
        }

        RenderTask::~RenderTask() {}

    }
}