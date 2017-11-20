#pragma once

#include <engine.h>

class LevelBackground {
public:
    explicit LevelBackground(SL::Engine &engine, SL::Tilemap &map);

    void scroll(int32_t x, int32_t y);

    void draw();

private:
    SL::Parallax _bg;
    SL::Parallax _mg;
};
