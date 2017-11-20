#include "engine.h"

SL::Parallax::Parallax(Gfx *gfx, Image image, float travelFactor) : _gfx{gfx}, _image{std::move(image)}, _travelFactor{travelFactor} {

}

void SL::Parallax::draw() {
    _gfx->drawBackgroundLayer(_image, static_cast<int32_t>(_x / _travelFactor), static_cast<int32_t>(_y / _travelFactor));
}

void SL::Parallax::scroll(int32_t scrollX, int32_t scrollY) {
    _x = scrollX;
    _y = scrollY;
}