#pragma once

#include <engine.h>

#include <map>

class MockGfx : public SL::Gfx {
public:
    MockGfx() = default;

    void update() override;;

    SL::Image loadImage(const std::string &filename) override;

    void drawImage(SL::Image &image, int32_t x, int32_t y, int32_t sourceX, int32_t sourceY, int32_t w, int32_t h, bool horizontallyFlipped) override;

    void drawBackgroundLayer(SL::Image &image, int32_t offsetX, int32_t offsetY) override;

    // Mocked methods
    void simulateAvailableImage(const std::string &filename, uint32_t width, uint32_t height);

    std::string drawnImage{""};
    std::string drawnLayer{""};

    bool updated{false};

private:
    std::map<std::string, SL::Image> _availableImages;
};


