#include "engine.h"

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
