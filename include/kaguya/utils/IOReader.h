//
// Created by Storm Phoenix on 2021/4/19.
//

#ifndef KAGUYA_IOREADER_H
#define KAGUYA_IOREADER_H

namespace kaguya {
    namespace utils {
        namespace io {
            unsigned char *readImage(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);

            void freeImage(unsigned char *image);
        }
    }
}

#endif //KAGUYA_IOREADER_H
