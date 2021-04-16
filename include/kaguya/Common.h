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
