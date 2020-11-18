//
// Created by Storm Phoenix on 2020/11/18.
//

#include <kaguya/parallel/Barrier.h>

namespace kaguya {
    namespace parallel {

        Barrier::Barrier(int threadCount) :
                _threadCount(threadCount) {
            assert(_threadCount > 0);
        }

        Barrier::~Barrier() {
            assert(_threadCount == 0);
        }

        void Barrier::wait() {
            std::unique_lock<std::mutex> lock(_barrierMutex);
            _threadCount--;
            if (_threadCount == 0) {
                // all threads reach to the barrier
                _barrierCondition.notify_all();
            } else {
                // if there are some threads haven't reached to the barrier,
                // then current thread will wait on the lock
                _barrierCondition.wait(lock, [this]() -> bool { return _threadCount == 0; });
            }
        }

    }
}