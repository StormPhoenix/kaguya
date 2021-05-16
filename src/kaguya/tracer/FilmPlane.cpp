//
// Created by Storm Phoenix on 2020/11/11.
//

#include <cstring>
#include <iostream>

#include <kaguya/Config.h>
#include <kaguya/tracer/PathRecorder.h>
#include <kaguya/tracer/FilmPlane.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <ext/stb/stb_image_write.h>

#define COLOR_WHITE Color(255, 255, 255)
#define COLOR_RED Color(255, 0, 0)
#define COLOR_GREEN Color(0, 255, 0)
#define COLOR_BLUE Color(0, 0, 255)

namespace kaguya {
    namespace tracer {

        FilmTile::FilmTile(Point2I offset, int width, int height) :
                _width(width), _height(height), _offsetInFilm(offset) {
            int tileSize = width * height;
            ASSERT(tileSize > 0, "FilmTile size should be greater than zero. ");
            _filmTile = std::unique_ptr<Pixel[]>(new Pixel[tileSize]);
        }

        void FilmTile::addSpectrum(const Spectrum &spectrum, int row, int col) {
            if (row < 0 || row >= _height ||
                col < 0 || col >= _width) {
                return;
            }

            int offset = (row * _width + col);
            Pixel &pixel = _filmTile[offset];
            pixel.spectrum += spectrum;
        }

        void FilmTile::setSpectrum(const Spectrum &spectrum, int row, int col) {
            if (row < 0 || row >= _height ||
                col < 0 || col >= _width) {
                return;
            }

            int offset = (row * _width + col);
            Pixel &pixel = _filmTile[offset];
            pixel.spectrum = spectrum;
        }

        static inline void drawPoint(unsigned char *image, int width, int height, Point2I pos,
                                     Color color = COLOR_WHITE, float radius = 0) {
            int minX = pos.x - radius;
            int minY = pos.y - radius;
            int maxX = pos.x + radius + 0.5;
            int maxY = pos.y + radius + 0.5;

            int channel = 3;
            for (int x = minX; x <= maxX; x++) {
                for (int y = minY; y <= maxY; y++) {
                    if (x < 0 || x >= width || y < 0 || y >= height) {
                        continue;
                    }

                    int dx = std::abs(x - pos.x);
                    int dy = std::abs(y - pos.y);
                    float len2 = (dx * dx) + (dy * dy);
                    if (len2 > radius * radius) {
                        continue;
                    }

                    int offset = ((height - y - 1) * width + x) * channel;
                    for (int ch = 0; ch < channel; ch++) {
                        *(image + offset + ch) = color[ch];
                    }
                }
            }
        }

        static inline void bresenham(unsigned char *image, int width, int height,
                                     Point2I start, Point2I end,
                                     Color startColor = COLOR_WHITE, Color endColor = COLOR_WHITE,
                                     Color lineColor = COLOR_WHITE, float radius = 0,
                                     bool swapXY = false, bool reverseX = false, bool reverseY = false) {
            float dx = end.x - start.x;
            float dy = end.y - start.y;

            ASSERT(dx > 0 && dy > 0, "Bresenham algorithm only only support positive direction. ");

            float k = dy / dx;
            ASSERT(k > 0 && k <= 1, "Bresenham algorithm only support [0, 1] range K. ");

            int xi = start.x;
            int yi = start.y;
            float delta = 2 * dy - dx;

            while (xi <= end.x) {
                Color color = lineColor;
                if (xi == start.x) {
                    color = startColor;
                } else if (xi == end.x) {
                    color = endColor;
                }

                if (delta < 0) {
                    delta += 2 * dy;
                    xi++;
                } else {
                    delta += 2 * dy - 2 * dx;
                    xi++;
                    yi++;
                }

                int retX = xi;
                int retY = yi;
                if (swapXY) {
                    std::swap(retX, retY);
                }

                if (reverseX) {
                    retX *= -1;
                }

                if (reverseY) {
                    retY *= -1;
                }
                drawPoint(image, width, height, Point2I(retX, retY), color, radius);
            }
        }

        static inline void drawLine(unsigned char *image, int width, int height,
                                    const Point2I &start, const Point2I &end,
                                    Color startColor = COLOR_WHITE, Color endColor = COLOR_WHITE,
                                    Color lineColor = COLOR_WHITE, float radius = 0.0f) {
            float dx = end.x - start.x;
            float dy = end.y - start.y;

            if (dx == 0) {
                int min = std::min(start.y, end.y);
                int max = std::max(start.y, end.y);
                for (int i = min; i <= max; i++) {
                    Color color = lineColor;
                    if (i == start.y) {
                        color = startColor;
                    } else if (i == end.y) {
                        color = endColor;
                    }
                    drawPoint(image, width, height, Point2I(start.x, i), color, radius);
                }
                return;
            }

            if (dy == 0) {
                int min = std::min(start.x, end.x);
                int max = std::max(start.x, end.x);
                for (int i = min; i <= max; i++) {
                    Color color = lineColor;
                    if (i == start.x) {
                        color = startColor;
                    } else if (i == end.x) {
                        color = endColor;
                    }
                    drawPoint(image, width, height, Point2I(i, start.y), color, radius);
                }
                return;
            }

            float k = std::abs(dy / dx);

            int reverseX = dx > 0 ? 1 : -1;
            int reverseY = dy > 0 ? 1 : -1;

            int sx = reverseX * start.x;
            int sy = reverseY * start.y;

            int tx = reverseX * end.x;
            int ty = reverseY * end.y;

            if (k <= 1) {
                bresenham(image, width, height, Point2I(sx, sy), Point2I(tx, ty),
                          startColor, endColor, lineColor, radius,
                          false, reverseX == 1 ? false : true, reverseY == 1 ? false : true);
            } else {
                bresenham(image, width, height, Point2I(sy, sx), Point2I(ty, tx),
                          startColor, endColor, lineColor, radius,
                          true, reverseX == 1 ? false : true, reverseY == 1 ? false : true);
            }
        }

        FilmPlane::FilmPlane(int resolutionWidth, int resolutionHeight, int channel) :
                _resolutionWidth(resolutionWidth), _resolutionHeight(resolutionHeight),
                _channel(channel) {
            int filmSize = _resolutionWidth * _resolutionHeight;
            _film = std::unique_ptr<Pixel[]>(new Pixel[filmSize]);
        }

        void FilmPlane::addExtra(const Spectrum &spectrum, int row, int col) {
            if (row < 0 || row >= _resolutionHeight ||
                col < 0 || col >= _resolutionWidth) {
                return;
            }

            int offset = (row * _resolutionWidth + col);
            Pixel &pixel = _film[offset];
            for (int ch = 0; ch < _channel; ch++) {
                pixel.extra[ch].add(spectrum[ch]);
            }
        }

        void FilmPlane::addSpectrum(const Spectrum &spectrum, int row, int col) {
            assert(_film != nullptr);
            if (row < 0 || row >= _resolutionHeight ||
                col < 0 || col >= _resolutionWidth) {
                return;
            }

            int offset = (row * _resolutionWidth + col);
            Pixel &pixel = _film[offset];
            pixel.spectrum += spectrum;
        }

        void FilmPlane::setSpectrum(const Spectrum &spectrum, int row, int col) {
            assert(_film != nullptr);
            if (row < 0 || row >= _resolutionHeight ||
                col < 0 || col >= _resolutionWidth) {
                return;
            }

            int offset = (row * _resolutionWidth + col);
            Pixel &pixel = _film[offset];
            pixel.spectrum = spectrum;
        }

        void FilmPlane::mergeTile(FilmTile::Ptr tile) {
            // Check offset
            Point2I tileOffset = tile->_offsetInFilm;
            if (tileOffset.x < 0 || tileOffset.x >= _resolutionWidth ||
                tileOffset.y < 0 || tileOffset.y >= _resolutionHeight) {
                std::cout << "Merge tile failed (tile offset out of range). " << std::endl;
                return;
            }

            int tileWidth = tile->_width;
            int tileHeight = tile->_height;
            if (tileOffset.x + tileWidth > _resolutionWidth ||
                tileOffset.y + tileHeight > _resolutionHeight) {
                std::cout << "Merge tile failed (tile size out of range). " << std::endl;
                return;
            }

            {
                std::lock_guard<std::mutex> lock(_writeLock);
                for (int row = 0; row < tileHeight; row++) {
                    for (int col = 0; col < tileWidth; col++) {
                        int offsetInTile = row * tileWidth + col;
                        Pixel &tilePixel = tile->_filmTile[offsetInTile];

                        int offsetInFilm = (tileOffset.y + row) * _resolutionWidth + (tileOffset.x + col);
                        Pixel &filmPixel = _film[offsetInFilm];

                        // Add spectrum
                        filmPixel.spectrum += tilePixel.spectrum;

                        // Add extra spectrum
                        for (int ch = 0; ch < _channel; ch++) {
                            filmPixel.extra[ch].add(tilePixel.extra[ch].get());
                        }
                    }
                }
            }
        }

        void FilmPlane::writeImage(char const *filename, Float weight) {
            // Create image buffer
            unsigned char *image = new unsigned char[_resolutionWidth * _resolutionHeight * _channel];
            {
                std::lock_guard<std::mutex> lock(_writeLock);
                // 将光谱转化到 image buffer
                for (int row = _resolutionHeight - 1; row >= 0; row--) {
                    for (int col = 0; col < _resolutionWidth; col++) {
                        int imageOffset = ((_resolutionHeight - 1 - row) * _resolutionWidth + col) * _channel;
                        int pixelOffset = row * _resolutionWidth + col;
                        Pixel &pixel = _film[pixelOffset];
                        for (int ch = 0; ch < _channel; ch++) {
                            (image + imageOffset)[ch] = static_cast<unsigned char>(
                                    256 *
                                    math::clamp(std::sqrt((pixel.spectrum[ch] + pixel.extra[ch].get()) * weight), 0.0,
                                                0.999));
                        }
                    }
                }
            }

            const std::map<PathRecorderHeader, std::vector<RasterRecord>> *paths = PathRecorder::getRecorder()->getPathRecords();
            if (paths != nullptr) {
                for (auto iter = paths->begin(); iter != paths->end(); iter++) {
                    const std::vector<RasterRecord> &path = iter->second;
                    for (int i = 1; i < path.size(); i++) {
                        const Point2I &start = path[i - 1].pRaster;
                        Color startColor = RENDER_RECORDER->getTypeColorMap()->find(path[i - 1].type)->second;

                        const Point2I &end = path[i].pRaster;
                        Color endColor = RENDER_RECORDER->getTypeColorMap()->find(path[i].type)->second;

                        Color lineColor = COLOR_WHITE;
                        drawLine(image, _resolutionWidth, _resolutionHeight, start, end,
                                 startColor, endColor, lineColor,
                                 Config::TraceRecorder::drawPathRadius);

                        drawPoint(image, _resolutionWidth, _resolutionHeight, start, startColor,
                                  Config::TraceRecorder::drawPathRadius + 1);
                        drawPoint(image, _resolutionWidth, _resolutionHeight, end, endColor,
                                  Config::TraceRecorder::drawPathRadius + 1);
                    }
                }
            }

            // 写入 image file
            stbi_write_png(filename, _resolutionWidth, _resolutionHeight, 3, image, 0);
            delete[] image;
        }
    }
}