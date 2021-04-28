//
// Created by Storm Phoenix on 2021/4/19.
//

#define STB_IMAGE_IMPLEMENTATION

#include <ext/stb/stb_image.h>

namespace kaguya {
    namespace utils {
        namespace io {
            unsigned char *
            readImage(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels) {
                return stbi_load(filename, x, y, channels_in_file, desired_channels);
            }

            void freeImage(unsigned char *image) {
                stbi_image_free(image);
            }
        }
    }
}
