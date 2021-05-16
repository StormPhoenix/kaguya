//
// Created by Storm Phoenix on 2021/5/15.
//

#include <kaguya/tracer/PathRecorder.h>
#include <kaguya/tracer/Camera.h>

namespace kaguya {
    namespace tracer {
        // Initialization
        PathRecorder *PathRecorder::_recorder = nullptr;
        std::mutex PathRecorder::_recorderCreationLock;

        PathRecorder *PathRecorder::getRecorder() {
            if (_recorder == nullptr) {
                {
                    std::lock_guard<std::mutex> poolLock(_recorderCreationLock);
                    if (_recorder == nullptr) {
                        _recorder = new PathRecorder();
                    }
                }
            }
            return _recorder;
        }

        PathRecorder::PathRecorder() {
            _typeColorMap[Environment_Record] = Color(0, 0, 255);
            _typeColorMap[Medium_Record] = Color(0, 255, 0);
            _typeColorMap[Specular_Record] = Color(255, 0, 0);
            _typeColorMap[Others_Record] = Color(255, 255, 255);
        }

        const std::map<VertexRecordType, Color> *PathRecorder::getTypeColorMap() const {
            return &_typeColorMap;
        }

        void PathRecorder::recodeTraceVertex(int originRow, int originCol, int iteration,
                                             const Point3F &point, VertexRecordType type, const Camera *camera) {
            if (!_tracePathFlag) {
                return;
            }

            if (!_pathRecorderMap.count({originCol, originRow, iteration})) {
                return;
            }

            auto &intersectionPath = _pathRecorderMap[{originCol, originRow, iteration}];
            Point2I pRaster = camera->worldToRaster(point);
            intersectionPath.push_back({pRaster, type});
        }

        void PathRecorder::addTraceCriteria(const Point2I pRaster, int pathCount) {
            if (!_tracePathFlag) {
                return;
            }

            for (int i = 1; i <= pathCount; i++) {
                _pathRecorderMap[{pRaster.x, pRaster.y, i}] = std::vector<RasterRecord>();
            }
        }

        void PathRecorder::setTraceTag(bool flag) {
            _tracePathFlag = flag;
        }

        const std::map<PathRecorderHeader, std::vector<RasterRecord>> *PathRecorder::getPathRecords() const {
            if (_tracePathFlag) {
                return &_pathRecorderMap;
            } else {
                return nullptr;
            }
        }
    }
}