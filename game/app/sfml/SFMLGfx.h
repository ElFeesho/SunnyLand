#pragma once

#include <string>
#include <map>

#include <SFML/Graphics.hpp>
#include <engine.h>

class SFMLGfx : public SL::Gfx {
public:
    explicit SFMLGfx(sf::RenderWindow &window);

    void drawImage(SL::Image &image, int32_t x, int32_t y, int32_t sourceX, int32_t sourceY, int32_t w, int32_t h, bool horizontallyFlipped) override;

    void drawBackgroundLayer(SL::Image &image, int32_t offsetX, int32_t offsetY) override;

    SL::Image loadImage(const std::string &filename) override;

    void update() override;

private:
    sf::RenderWindow &_window;
    std::map<std::string, sf::Texture> _loadedImages;
};