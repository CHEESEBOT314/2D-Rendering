#include "Atlas.h"

#include <algorithm>
#include <cstring>

#include "PngReader.h"

struct Node {
    Node* right = nullptr;
    Node* down = nullptr;
    bool e = true;
    uint32_t x, y, w, h;
    Node(uint32_t xIn, uint32_t yIn, uint32_t wIn, uint32_t hIn) {
        this->x = xIn;
        this->y = yIn;
        this->w = wIn;
        this->h = hIn;
    }
    bool empty() { return this->e; }
    Node* set(uint32_t wIn, uint32_t hIn) {
        if (this->empty()) {
            if (!(wIn > this->w || hIn > this->h)) {
                this->right = new Node(this->x + wIn, this->y, this->w - wIn, hIn);
                this->down = new Node(this->x, this->y + hIn, this->w, this->h - hIn);
                this->e = false;
                this->w = wIn;
                this->h = hIn;
                return this;
            }
        }
        else {
            Node* out = this->right->set(wIn, hIn);
            return (out != nullptr) ? out : this->down->set(wIn, hIn);
        }
        return nullptr;
    }
    void destroy() {
        if (!this->empty()) {
            this->right->destroy();
            delete this->right;
            this->down->destroy();
            delete this->down;
        }
    }
};

Atlas::Atlas(const std::string& base, const std::string& out) {
    this->baseDir = base;
    this->outBase = out;
}

void Atlas::addTexture(const std::string& name, const std::string& file) {
    if (this->stage < 2) {
        this->stage = 1;
        this->images.push_back({name, file, 0, 0, 0, 0});
    }
}

bool Atlas::loadInfo() {
    if (this->stage == 1) {
        for (Image& image : this->images) {
            PngReader reader;
            std::string success = reader.init(image.file);
            reader.close();
            if (!success.empty()) {
                printf("Failed to load info:\n    %s - %s\n", image.name.c_str(), success.c_str());
                return false;
            }
            image.w = reader.getWidth();
            image.h = reader.getHeight();
        }
        this->stage = 2;
        return true;
    }
    return false;
}

bool Atlas::packRectangles(uint32_t w, uint32_t h) {
    if (this->stage != 2) return false;

    std::sort(this->images.begin(), this->images.end(), [](const Image& i1, const Image& i2) {
        if (i1.w > i2.w) return true;
        if (i1.w == i2.w) return i1.h > i2.h;
        return false;
    });

    printf("Placing textures:\n");

    std::vector<Node*> layers;
    layers.push_back(new Node(0, 0, w + 1, h + 1));

    bool success = true;

    for (Image& img : this->images) {
        uint32_t layer = 0;
        for (Node* l : layers) {
            Node* actual = l->set(img.w + 1, img.h + 1);
            if (actual != nullptr) {
                img.x = actual->x;
                img.y = actual->y;
                img.layer = layer;
                printf("    %s at (%d, %d, %d)\n", img.name.c_str(), img.x, img.y, img.layer);
                break;
            }
            layer++;
        }
        if (img.layer == std::numeric_limits<uint32_t>::max()) {
            Node* nextLayer = new Node(0, 0, w + 1, h + 1);
            if (nextLayer->set(img.w + 1, img.h + 1) != nullptr) {
                img.x = nextLayer->x;
                img.y = nextLayer->y;
                img.layer = layer;
                layers.push_back(nextLayer);
                printf("    %s at (%d, %d, %d)\n", img.name.c_str(), img.x, img.y, img.layer);
                continue;
            }
            printf("    Failed %s\n", img.name.c_str());
            success = false;
            break;
        }
    }
    this->layerCount = (uint32_t)layers.size();
    while (!layers.empty()) {
        Node* n = layers.back();
        layers.pop_back();
        n->destroy();
        delete n;
    }
    this->stage = 3;
    this->width = w;
    this->height = h;
    return success;
}

bool Atlas::build() {
    if (this->stage != 3) return false;

    uint8_t* data;
    data = new uint8_t[this->width * this->height * 4 * this->layerCount];

    for (const Image& img : this->images) {
        PngReader reader;
        std::string success = reader.init(img.file);
        if (!success.empty()) {
            printf("Failed to load info:\n    %s - %s\n", img.name.c_str(), success.c_str());
            reader.close();
            delete[] data;
            return false;
        }
        if (reader.getWidth() != img.w || reader.getHeight() != img.h) {
            reader.close();
            delete[] data;
            return false;
        }
        uint8_t* imgData = reader.getData();
        reader.close();
        uint32_t rowLength = img.w * 4;
        for (int i = 0; i < img.h; i++) {
            memcpy(data + (((img.layer * this->height) + img.y + i) * this->width + img.x) * 4, imgData + i * rowLength, rowLength);
        }
        delete[] imgData;
    }

    for (int l = 0; l < this->layerCount; l++) {
        if (!PngReader::writeFile(this->outBase + std::to_string(l) + ".png", this->width, this->height, data + this->width * this->height * 4 * l)) {
            delete[] data;
            return false;
        }
    }
    this->stage = 4;
    delete[] data;
    return true;
}

static void write4Byte(FILE* fp, uint32_t d) {
    static const uint8_t u255 = 255;
    uint8_t o[4];
    o[0] = (uint8_t)(d >> 24) & u255;
    o[1] = (uint8_t)(d >> 16) & u255;
    o[2] = (uint8_t)(d >> 8) & u255;
    o[3] = (uint8_t)(d >> 0) & u255;
    fwrite(o, 1, 4, fp);
}

static void read4Byte(FILE* fp, uint32_t& d) {
    static const uint8_t u255 = 255;
    uint8_t o[4];
    fread(o, 1, 4, fp);
    d = ((uint32_t)o[0] & u255) << 24;
    d += ((uint32_t)o[1] & u255) << 16;
    d += ((uint32_t)o[2] & u255) << 8;
    d += ((uint32_t)o[3] & u255) << 0;
}

bool Atlas::writeData() {
    static const uint8_t u0 = 0;
    FILE* fp = fopen((this->outBase + ".ats").c_str(), "wb");
    if (!fp) {
        return false;
    }
    write4Byte(fp, (uint32_t)this->images.size());
    for (const Image& img : this->images) {
        write4Byte(fp, img.x);
        write4Byte(fp, img.y);
        write4Byte(fp, img.layer);
        write4Byte(fp, img.w);
        write4Byte(fp, img.h);
        fprintf(fp, "%s", img.name.c_str());
        fwrite(&u0, 1, 1, fp);
    }
    fclose(fp);
    return true;
}