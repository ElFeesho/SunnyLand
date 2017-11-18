#pragma once

#include <engine.h>

class MockScene : public SL::Scene {
public:
    MockScene() = default;

    void update(long delta) override;;

    void keyEvent(SL::KeyType key, SL::ActionType action) override;

    std::string keyStream{""};
    long updatedTimeDelta{0};
};


