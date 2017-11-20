#include "engine.h"

SL::Sprite::Sprite(SL::Gfx *gfx, SL::Image image, uint32_t cellWidth, uint32_t cellHeight) : _gfx{gfx}, _image{image}, _cellWidth{cellWidth}, _cellHeight{cellHeight} {
    _frames = (image.width() / cellWidth) * (image.height() / cellHeight);
}

void SL::Sprite::draw(int32_t x, int32_t y, bool horizontallyFlipped) {
    _gfx->drawImage(_image, x, y, _frame * _cellWidth, 0, _cellWidth, _cellHeight, horizontallyFlipped);
}

uint32_t SL::Sprite::frameCount() {
    return _frames;
}

void SL::Sprite::update(long timeDelta) {
    _lastTicks += timeDelta;
    if (_lastTicks >= 83L) {
        _lastTicks -= 83L;
        _frame++;
        if (_frame >= frameCount()) {
            _frame = 0;
        }
    }
}