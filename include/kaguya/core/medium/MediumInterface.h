//
// Created by Storm Phoenix on 2020/12/16.
//

#ifndef KAGUYA_MEDIUMINTERFACE_H
#define KAGUYA_MEDIUMINTERFACE_H

#include <kaguya/common.h>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace medium {
            class Medium;

            class MediumInterface {
            public:
                MediumInterface() : _inside(nullptr), _outside(nullptr) {}

                MediumInterface(const Medium *medium) : _inside(medium), _outside(medium) {}

                MediumInterface(const Medium *inside,
                                const Medium *outside) :
                        _inside(inside), _outside(outside) {}

                const Medium *inside() const {
                    return _inside;
                }

                const Medium *outside() const {
                    return _outside;
                }

            private:
                const Medium *_inside;
                const Medium *_outside;
            };
        }
    }
}

#endif //KAGUYA_MEDIUMINTERFACE_H
