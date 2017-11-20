#include "engine.h"

SL::Image::Image(const std::string &filename, uint32_t width, uint32_t height) : _filename{filename}, _width{width}, _height{height} {

}

const std::string &SL::Image::filename() const {
    return _filename;
}

uint32_t SL::Image::width() {
    return _width;
}

uint32_t SL::Image::height() {
    return _height;
}
