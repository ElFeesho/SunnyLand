#include <cstdint>
#include <utility>
#include "engine.h"

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
    return {_gfx, _gfx->loadImage(filename), cellWidth, cellHeight};
}

SL::Parallax SL::Engine::createParallax(const std::string &filename, float travelDampening) {
    return {_gfx, _gfx->loadImage(filename), travelDampening};
}

SL::Tilemap SL::Engine::createMap(const std::string mapData, const std::string tilesetImage) {
    auto mapJson = nlohmann::json::parse(mapData);
    auto layers = mapJson["layers"];

    int32_t playerSpawnX = 0;
    int32_t playerSpawnY = 0;
    int32_t cameraSpawnX = 0;
    int32_t cameraSpawnY = 0;

    std::vector<SL::Tilemap::Layer> tilemapLayers;
    SL::Image tileset = _gfx->loadImage(tilesetImage);

    for (auto &layer : layers) {
        if (layer.find("type") != layer.end() && layer["type"].get<std::string>() == "objectgroup") {
            for (auto &object : layer["objects"]) {
                if (object["type"].get<std::string>() == "player_spawn") {
                    playerSpawnX = object["x"].get<int32_t>();
                    playerSpawnY = object["y"].get<int32_t>();
                } else {
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

    const uint32_t &width = mapJson["width"].get<uint32_t>();
    const uint32_t &height = mapJson["height"].get<uint32_t>();
    return SL::Tilemap(width, height, tilemapLayers, playerSpawnX, playerSpawnY, cameraSpawnX, cameraSpawnY, _gfx->loadImage(bgImageName),
                       _gfx->loadImage(mgImageName));
}