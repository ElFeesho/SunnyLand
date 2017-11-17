#include "SFMLGfx.h"

SFMLGfx::SFMLGfx(sf::RenderWindow &window) : _window{window} {

}

SL::Image SFMLGfx::loadImage(const std::__1::string &filename) {
    if (_loadedImages.find(filename) == _loadedImages.end()) {
        sf::Texture newImage{};
        if (!newImage.loadFromFile(filename)) {
            throw std::domain_error("Failed to load " + filename + " image");
        }
        _loadedImages.insert({filename, std::__1::move(newImage)});
    }

    sf::Texture &image = _loadedImages[filename];
    return SL::Image{filename, image.getSize().x, image.getSize().y};
}

void SFMLGfx::drawImage(SL::Image &image, int32_t x, int32_t y, int32_t sourceX, int32_t sourceY, int32_t w, int32_t h, bool horizontallyFlipped) {
    auto &texture = _loadedImages[image.filename()];
    sf::Sprite sprite{texture};
    sprite.setTextureRect(sf::IntRect{sourceX, sourceY, w, h});
    sprite.setPosition((x + (horizontallyFlipped ? w : 0))*2, y*2);
    sprite.setScale(horizontallyFlipped ? -2.0f : 2.0f, 2.0f);

    _window.draw(sprite);
}

void SFMLGfx::drawBackgroundLayer(SL::Image &image, int32_t offsetX, int32_t offsetY) {
    auto &texture = _loadedImages[image.filename()];
    texture.setRepeated(true);
    sf::Sprite sprite{texture};
    const sf::Vector2u &windowSize = _window.getSize();
    float scale = ((float) windowSize.y) / ((float) texture.getSize().y);
    sprite.setScale(scale, scale);

    int32_t textureWidth = static_cast<int32_t>(texture.getSize().x * scale);
    int32_t windowWidth = windowSize.x;

    offsetX = offsetX % textureWidth;

    int32_t startPosition = offsetX;

    for (int32_t i = startPosition; i < windowWidth; i += textureWidth) {
        sprite.setPosition(i, 0);
        _window.draw(sprite);
    }

    for (int32_t i = startPosition - textureWidth; i+textureWidth >= 0; i -= textureWidth) {
        sprite.setPosition(i, 0);
        _window.draw(sprite);
    }
}

void SFMLGfx::update() {
    _window.display();
    _window.clear({128, 128, 128});
}
