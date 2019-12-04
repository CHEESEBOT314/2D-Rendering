#include "PngReader.h"

#include <cstdio>
#include <stdexcept>
#include <cstring>
#include <sstream>
#include "png.h"

std::string PngReader::init(const std::string& file) {
    std::string error;
    this->fp = fopen(file.c_str(), "rb");
    if (!this->fp) {
        error = "Could not open file";
        return error;
    }
    try {
        png_byte sig[8];
        if (fread((char *) sig, 1, 8, this->fp) != 8) {
            throw std::runtime_error("Could not read header from file");
        }

        if (png_sig_cmp(sig, 0, 8) != 0) {
            throw std::runtime_error("PNG header does not match");
        }

        this->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!this->png_ptr) {
            throw std::runtime_error("Could not initialise 'png_structp'");
        }
        this->info_ptr = png_create_info_struct(this->png_ptr);
        if (!this->info_ptr) {
            throw std::runtime_error("Could not initialise 'png_infop'");
        }
        png_init_io(this->png_ptr, this->fp);
        png_set_sig_bytes(this->png_ptr, 8);

        png_read_info(this->png_ptr, this->info_ptr);

        if (png_get_color_type(this->png_ptr, this->info_ptr) != PNG_COLOR_TYPE_RGB_ALPHA) {
            png_set_expand(png_ptr);
        }
        if (png_get_bit_depth(this->png_ptr, this->info_ptr) != 8) {
            png_set_strip_16(png_ptr);
        }
        this->width = (uint32_t)png_get_image_width(this->png_ptr, this->info_ptr);
        this->height = (uint32_t)png_get_image_height(this->png_ptr, this->info_ptr);
    }
    catch (const std::exception& e) {
        png_destroy_read_struct(&this->png_ptr, &this->info_ptr, nullptr);
        std::stringstream ss;
        ss << "Exception: " << e.what();
        return (error = ss.str());
    }
    return error;
}

uint32_t PngReader::getWidth() {
    return this->width;
}
uint32_t PngReader::getHeight() {
    return this->height;
}

uint8_t* PngReader::getData() {
    uint8_t* out = nullptr;
    try {
        png_read_update_info(this->png_ptr, this->info_ptr);

        out = new uint8_t[this->width * this->height * 4];

        png_byte* row;
        row = new png_byte[png_get_rowbytes(this->png_ptr, this->info_ptr)];

        for (int i = 0; i < this->height; i++) {
            png_read_row(this->png_ptr, row, nullptr);
            memcpy(out + i * this->width * 4, row, this->width * 4);
        }
        delete[] row;
        png_read_end(this->png_ptr, nullptr);
        png_destroy_read_struct(&this->png_ptr, &this->info_ptr, nullptr);
    }
    catch (const std::exception& e) {
        png_destroy_read_struct(&this->png_ptr, &this->info_ptr, nullptr);
        printf("    Exception: %s\n", e.what());
        return nullptr;
    }
    return out;
}

void PngReader::close() {
    fclose(this->fp);
}

bool PngReader::writeFile(const std::string &file, uint32_t w, uint32_t h, uint8_t *data) {
    FILE* fp = fopen(file.c_str(), "wb");
    printf("Writing file: %s\n", file.c_str());
    if (!fp) {
        printf("    Could not open file\n");
        return false;
    }

    png_structp png_ptr;
    png_infop info_ptr;
    try {
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png_ptr) {
            throw std::runtime_error("Could not initialise 'png_structp'");
        }
        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            throw std::runtime_error("Could not initialise 'png_infop'");
        }
        png_init_io(png_ptr, fp);

        png_set_IHDR(png_ptr, info_ptr, w, h, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);

        png_byte* row;
        row = new png_byte[png_get_rowbytes(png_ptr, info_ptr)];

        for (int i = 0; i < h; i++) {
            memcpy(row, data + i * w * 4, w * 4);
            png_write_row(png_ptr, row);
        }
        delete[] row;
        png_write_end(png_ptr, nullptr);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
    }
    catch (const std::exception& e) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        printf("    Exception: %s\n", e.what());
        return false;
    }
    return true;
}