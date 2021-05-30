//
// Created by Graphics on 2021/5/30.
//

#ifndef TUNAN_OPTIX_UTILS_H
#define TUNAN_OPTIX_UTILS_H

#include <kaguya/common.h>

#include <optix.h>
#include <optix_function_table_definition.h>
#include <optix_stubs.h>

#define OPTIX_CHECK(exp) \
    do { \
        OptixResult ret = exp; \
        ASSERT(ret == OPTIX_SUCCESS, "OptiX error: " + std::string(optixGetErrorString(ret))); \
    } while (false)

#endif //TUNAN_OPTIX_UTILS_H
