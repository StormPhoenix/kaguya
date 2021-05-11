//
// Created by Storm Phoenix on 2021/4/19.
//

#include <kaguya/utils/FileReader.h>

namespace kaguya {
    namespace utils {
        namespace io {
            bool extensionEquals(const std::string &filename, const std::string &extension) {
                if (extension.size() > filename.size()) return false;
                return std::equal(
                        extension.rbegin(), extension.rend(), filename.rbegin(),
                        [](char a, char b) { return std::tolower(a) == std::tolower(b); });
            }
        }
    }
}
