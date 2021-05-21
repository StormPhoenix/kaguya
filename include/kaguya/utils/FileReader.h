//
// Created by Storm Phoenix on 2021/4/19.
//

#ifndef KAGUYA_FILEREADER_H
#define KAGUYA_FILEREADER_H

#include <kaguya/Common.h>

namespace RENDER_NAMESPACE {
    namespace utils {
        namespace io {
            bool extensionEquals(const std::string &filename, const std::string &extension);
        }
    }
}

#endif //KAGUYA_FILEREADER_H
