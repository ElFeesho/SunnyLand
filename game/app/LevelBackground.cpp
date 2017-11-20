#include "LevelBackground.h"

LevelBackground::LevelBackground(SL::Engine &engine, SL::Tilemap &map) : _bg{engine.createParallax(map.bgImage().filename(), 6.0f)},
                                                                         _mg{engine.createParallax(map.mgImage().filename(), 1.0f)} {

}

void LevelBackground::scroll(int32_t x, int32_t y) {
    _bg.scroll(x, y);
    _mg.scroll(x, y);
}

void LevelBackground::draw() {
    _bg.draw();
    _mg.draw();
}