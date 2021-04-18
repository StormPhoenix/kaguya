//
// Created by Storm Phoenix on 2021/4/16.
//

#ifndef KAGUYA_COMMON_H
#define KAGUYA_COMMON_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#if __has_include(<filesystem>)
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif
#ifndef GHC_USE_STD_FS
#include <ext/ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

namespace kaguya {

#define ASSERT(condition, description) \
    do { \
        if (!(condition)) {                \
            std::cerr << std::endl << "Assertion occured at " << std::endl \
                << __FUNCTION__ << " " << __FILE__ << " " << __LINE__ << "(" << description << ")", \
                throw std::runtime_error(description); \
        } \
    } while (0);
}

#endif //KAGUYA_COMMON_H
