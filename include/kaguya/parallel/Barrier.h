//
// Created by Storm Phoenix on 2020/11/18.
//

#ifndef KAGUYA_BARRIER_H
#define KAGUYA_BARRIER_H

#include <kaguya/Common.h>
#include <cassert>
#include <condition_variable>

namespace RENDER_NAMESPACE {
    namespace parallel {

        class Barrier {
        public:
            Barrier(int threadCount);

            ~Barrier();

            void wait();

        private:
            int _threadCount;

            std::mutex _barrierMutex;

            std::condition_variable _barrierCondition;
        };

    }
}

#endif //KAGUYA_BARRIER_H
