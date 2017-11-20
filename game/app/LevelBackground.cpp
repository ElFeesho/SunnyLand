#include "LevelBackground.h"

LevelBackground::LevelBackground(SL::Engine &engine) : _bg{engine.createParallax("../Resources/island-background.png", 6.0f)},
                                                       _mg{engine.createParallax("../Resources/island-middleground.png", 1.0f)} {

}

void LevelBackground::scroll(int32_t x, int32_t y) {
    _bg.scroll(x, y);
    _mg.scroll(x, y);
}

void LevelBackground::draw() {
    _bg.draw();
    _mg.draw();
}