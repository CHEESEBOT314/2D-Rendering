#ifndef TEXTUREPACKAGER_PNGREADER_H
#define TEXTUREPACKAGER_PNGREADER_H

#include <string>
#include <png.h>

class PngReader {
public:
    std::string init(const std::string& file);

    uint32_t getWidth();
    uint32_t getHeight();

    uint8_t* getData();

    void close();

    static bool writeFile(const std::string& file, uint32_t w, uint32_t h, uint8_t* data);

private:
    FILE* fp = nullptr;
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    uint32_t width = std::numeric_limits<uint32_t>::max();
    uint32_t height = std::numeric_limits<uint32_t>::max();
};

#endif //TEXTUREPACKAGER_PNGREADER_H
