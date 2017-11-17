#include <cstdint>
#include "engine.h"
#include "json.hpp"

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

SL::Engine::Engine(SL::Gfx *gfx, SL::Input *input, SL::Time *time, SL::Sleeper *sleeper) : _gfx{gfx}, _input{input}, _time{time}, _sleeper{sleeper} {
    _input->addQuitHandler([&] {
        _alive = false;
    });

    _input->addKeyHandler([&](SL::KeyType key, SL::ActionType action) {
        _activeScene->keyEvent(key, action);
    });
}

bool SL::Engine::update() {
    _gfx->update();
    _input->update();

    _activeScene->update(_time->currentTime() - _lastTime);
    _lastTime = _time->currentTime();

    _sleeper->sleep(_time->currentTime());

    return _alive;
}

void SL::Engine::displayScene(SL::Scene *scene) {
    _activeScene = scene;
}

SL::Sprite SL::Engine::createSprite(const std::string &filename, uint32_t cellWidth, uint32_t cellHeight) {
    auto image = _gfx->loadImage(filename);
    return {_gfx, image, cellWidth, cellHeight};
}

SL::Parallax SL::Engine::createParallax(const std::string &filename, float travelDampening) {
    auto image = _gfx->loadImage(filename);
    return {_gfx, image, travelDampening};
}

SL::Tilemap SL::Engine::createMap(const std::string mapData, const std::string tilesetImage) {
    auto mapJson = nlohmann::json::parse(mapData);
    auto layers = mapJson["layers"];

    SL::Image tileset = _gfx->loadImage(tilesetImage);

    std::vector<SL::Tilemap::Layer> tilemapLayers;

    for (auto &layer : layers) {
        tilemapLayers.emplace_back(_gfx, tileset, layer["width"].get<uint32_t>(), layer["height"].get<uint32_t>(), layer["data"].get<std::vector<uint32_t>>());
    }

    return SL::Tilemap(mapJson["width"].get<uint32_t>(), mapJson["height"].get<uint32_t>(), tilemapLayers);
}

SL::Parallax::Parallax(SL::Gfx *gfx, SL::Image image, float travelFactor) : _gfx{gfx}, _image{image}, _travelFactor{travelFactor} {

}

void SL::Parallax::draw() {
    _gfx->drawBackgroundLayer(_image, _x / _travelFactor, _y / _travelFactor);
}

void SL::Parallax::scroll(int32_t scrollX, int32_t scrollY) {
    _x += scrollX;
    _y += scrollY;
}

SL::Tilemap::Tilemap(uint32_t width, uint32_t height, std::vector<SL::Tilemap::Layer> layers) : _w{width}, _h{height}, _layers{layers} {

}

SL::Tilemap::Layer &SL::Tilemap::layer(uint32_t index) {
    return _layers[index];
}

uint32_t SL::Tilemap::width() {
    return _w;
}

uint32_t SL::Tilemap::height() {
    return _h;
}

SL::Tilemap::Layer::Layer(Gfx *gfx, Image tileset, uint32_t width, uint32_t height, std::vector<uint32_t> tiles) : _gfx{gfx}, _tileset{tileset}, _w{width}, _h{height}, _tiles{tiles} {
}

int32_t SL::Tilemap::Layer::tile(uint32_t x, uint32_t y) {
    return _tiles[y * _w + x];
}

void SL::Tilemap::Layer::draw(int32_t x, int32_t y) {

    for (uint32_t ty = 0; ty < _h; ty++) {
        for (uint32_t tx = 0; tx < _w; tx++) {
            drawTile(tx, ty, x + tx * 16, y + ty * 16);
        }
    }
}

void SL::Tilemap::Layer::drawTile(uint32_t tileX, uint32_t tileY, int32_t x, int32_t y) {
    auto tileNumber = tile(tileX, tileY);

    if (tileNumber > 0) {
        tileNumber -= 1;
        int32_t sourceX = tileNumber % (_tileset.width() / 16);
        int32_t sourceY = tileNumber / (_tileset.width() / 16);
        _gfx->drawImage(_tileset, x, y, sourceX * 16, sourceY * 16, 16, 16, false);
    }
}
