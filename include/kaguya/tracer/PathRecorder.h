//
// Created by Storm Phoenix on 2021/5/15.
//

#ifndef KAGUYA_PATHRECORDER_H
#define KAGUYA_PATHRECORDER_H

#define RENDER_RECORD_TRACE_PATH
#ifdef RENDER_RECORD_TRACE_PATH

#define RECORD_TRACE_PATH(x, y, iteration, point, type, camera) \
    kaguya::tracer::PathRecorder::getRecorder()->recodeTraceVertex(y, x, iteration, point, type, camera);
#define SET_TRACE_PATH_FLAG(flag) kaguya::tracer::PathRecorder::getRecorder()->setTraceTag(flag);
#define ADD_TRACE_CRITERIA(pRaster, pathCount) kaguya::tracer::PathRecorder::getRecorder()->addTraceCriteria(pRaster, pathCount);

#else

#define RECORD_TRACE_PATH(x, y, iteration, point, camera)
#define SET_TRACE_PATH_FLAG(flag)
#define ADD_TRACE_CRITERIA(pRaster, pathCount)

#endif

#define RENDER_RECORDER kaguya::tracer::PathRecorder::getRecorder()

#include <kaguya/math/Math.h>
#include <vector>
#include <mutex>
#include <set>

namespace kaguya {
    namespace tracer {
        class Camera;

        typedef enum VertexRecordType {
            Specular_Record,
            Medium_Record,
            Environment_Record,
            Others_Record
        } VertexRecordType;

        typedef struct PathRecorderHeader {
            int originCol, originRow;
            int iteration;

            PathRecorderHeader(int originCol, int originRow, int iteration) :
                    originCol(originCol), originRow(originRow), iteration(iteration) {}

            bool operator<(const PathRecorderHeader &pr) const {
                return originCol < pr.originCol ||
                       ((originCol == pr.originCol) && (originRow < pr.originRow)) ||
                       ((originCol == pr.originCol) && (originRow == pr.originRow) && iteration < pr.iteration);
            }
        } PathRecorderHeader;

        typedef struct RasterRecord {
            Point2I pRaster;
            VertexRecordType type;
        } RasterRecord;

        // TODO Rename
        class PathRecorder {
        public:
            static PathRecorder *getRecorder();

            PathRecorder();

            void recodeTraceVertex(int originRow, int originCol, int iteration,
                                   const Point3F &point, VertexRecordType type, const Camera *camera);

            void addTraceCriteria(const Point2I pRaster, int pathCount);

            const std::map<VertexRecordType, Color> *getTypeColorMap() const;

            void setTraceTag(bool flag);

            const std::map<PathRecorderHeader, std::vector<RasterRecord>> *getPathRecords() const;

        private:
            bool _tracePathFlag = false;
            std::map<PathRecorderHeader, std::vector<RasterRecord>> _pathRecorderMap;
            std::map<VertexRecordType, Color> _typeColorMap;

        private:
            // Instance
            static PathRecorder *_recorder;
            // Creation lock
            static std::mutex _recorderCreationLock;
        };
    }
}
#endif //KAGUYA_PATHRECORDER_H
