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

    int32_t playerSpawnX = 0;
    int32_t playerSpawnY = 0;
    int32_t cameraSpawnX = 0;
    int32_t cameraSpawnY = 0;

    SL::Image tileset = _gfx->loadImage(tilesetImage);

    std::vector<SL::Tilemap::Layer> tilemapLayers;

    for (auto &layer : layers) {
        if (layer.find("type") != layer.end() && layer["type"].get<std::string>() == "objectgroup") {
            for (auto &object : layer["objects"]) {
                if (object["type"].get<std::string>() == "player_spawn") {
                    playerSpawnX = object["x"].get<int32_t>();
                    playerSpawnY = object["y"].get<int32_t>();
                }
                else {
                    cameraSpawnX = object["x"].get<int32_t>();
                    cameraSpawnY = object["y"].get<int32_t>();
                }
            }
        } else {
            tilemapLayers.emplace_back(_gfx, tileset, layer["width"].get<uint32_t>(), layer["height"].get<uint32_t>(), layer["data"].get<std::vector<uint32_t>>());
        }
    }

    std::string bgImageName = mapJson["properties"]["background"].get<std::string>();
    std::string mgImageName = mapJson["properties"]["middleground"].get<std::string>();

    return SL::Tilemap(mapJson["width"].get<uint32_t>(), mapJson["height"].get<uint32_t>(), tilemapLayers, playerSpawnX, playerSpawnY, cameraSpawnX, cameraSpawnY, _gfx->loadImage(bgImageName), _gfx->loadImage(mgImageName));
}

SL::Parallax::Parallax(SL::Gfx *gfx, SL::Image image, float travelFactor) : _gfx{gfx}, _image{image}, _travelFactor{travelFactor} {

}

void SL::Parallax::draw() {
    _gfx->drawBackgroundLayer(_image, _x / _travelFactor, _y / _travelFactor);
}

void SL::Parallax::scroll(int32_t scrollX, int32_t scrollY) {
    _x = scrollX;
    _y = scrollY;
}

SL::Tilemap::Tilemap(uint32_t width, uint32_t height, std::vector<Layer> layers, int32_t playerSpawnX, int32_t playerSpawnY, int32_t cameraSpawnX, int32_t cameraSpawnY, Image bgImage, Image mgImage)
        : _w{width}, _h{height}, _layers{layers}, _playerSpawnX{playerSpawnX}, _playerSpawnY{playerSpawnY}, _cameraSpawnX{cameraSpawnX}, _cameraSpawnY{cameraSpawnY}, _bgImage{bgImage}, _mgImage{mgImage} {

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

int32_t SL::Tilemap::playerSpawnX() {
    return _playerSpawnX;
}

int32_t SL::Tilemap::playerSpawnY() {
    return _playerSpawnY;
}

int32_t SL::Tilemap::cameraSpawnX() {
    return _cameraSpawnX;
}

int32_t SL::Tilemap::cameraSpawnY() {
    return _cameraSpawnY;
}

SL::Image &SL::Tilemap::bgImage() {
    return _bgImage;
}

SL::Image &SL::Tilemap::mgImage() {
    return _mgImage;
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

SL::JSONSpriteFactory::JSONSpriteFactory(SL::Engine &engine) : _engine{engine} {}

std::map<std::string, SL::Sprite> SL::JSONSpriteFactory::parse(const std::string &spritesJson) {
    nlohmann::json spriteJson = nlohmann::json::parse(spritesJson);
    std::map<std::string, SL::Sprite> result{};

    for (auto &spriteObj : spriteJson["sprites"]) {
        const std::string &spriteName = spriteObj["name"].get<std::string>();
        const SL::Sprite &sprite = _engine.createSprite(spriteObj["img"].get<std::string>(), spriteObj["cw"].get<uint32_t>(), spriteObj["ch"].get<uint32_t>());
        result.insert(std::pair<std::string, SL::Sprite>(spriteName, sprite));
    }

    return result;
}
