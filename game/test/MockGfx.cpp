#include "MockGfx.h"

void MockGfx::update() {
    updated = true;
}

SL::Image MockGfx::loadImage(const std::string &filename) {
    return _availableImages.at(filename);
}

void MockGfx::drawImage(SL::Image &image, int32_t x, int32_t y, int32_t sourceX, int32_t sourceY, int32_t w, int32_t h, bool horizontallyFlipped) {
    drawnImage = image.filename()+","+std::to_string(x)+","+std::to_string(y)+","+std::to_string(sourceX)+","+std::to_string(sourceY)+","+std::to_string(w)+","+std::to_string(h)+","+std::to_string(horizontallyFlipped);
}

void MockGfx::drawBackgroundLayer(SL::Image &image, int32_t offsetX, int32_t offsetY) {
    drawnLayer = image.filename()+","+std::to_string(offsetX)+","+std::to_string(offsetY);
}

void MockGfx::simulateAvailableImage(const std::string &filename, uint32_t width, uint32_t height) {
    _availableImages.insert({filename, SL::Image{filename, width, height}});
}
