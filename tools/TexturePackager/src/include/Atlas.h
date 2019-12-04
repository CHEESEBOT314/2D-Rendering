#ifndef TEXTUREPACKAGER_ATLAS_H
#define TEXTUREPACKAGER_ATLAS_H

#include <string>
#include <vector>

struct Image {
    std::string name;
    std::string file;
    uint32_t w, h;
    uint32_t x, y;
    uint32_t layer = std::numeric_limits<uint32_t>::max();
};

class Atlas {
public:
    Atlas(const std::string& base, const std::string& out);

    void addTexture(const std::string& name, const std::string& file);
    bool loadInfo();
    bool packRectangles(uint32_t w, uint32_t h);
    bool build();
    bool writeData();
private:
    int stage = 0;
    std::string baseDir;
    std::string outBase;
    std::vector<Image> images;
    uint32_t layerCount = 0;
    uint32_t  width = 0, height = 0;
};

#endif//TEXTUREPACKAGER_ATLAS_H
