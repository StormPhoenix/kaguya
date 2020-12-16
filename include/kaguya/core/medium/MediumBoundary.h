//
// Created by Storm Phoenix on 2020/12/16.
//

#ifndef KAGUYA_MEDIUMBOUNDARY_H
#define KAGUYA_MEDIUMBOUNDARY_H

namespace kaguya {
    namespace core {
        namespace medium {
            class Medium;

            class MediumBoundary {
            public:
                MediumBoundary() : _inside(nullptr), _outside(nullptr) {}

                MediumBoundary(const Medium *medium) : _inside(medium), _outside(medium) {}

                MediumBoundary(const Medium *inside,
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

#endif //KAGUYA_MEDIUMBOUNDARY_H
