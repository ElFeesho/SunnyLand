#pragma once

#include <engine.h>

class MockInput : public SL::Input {
public:
    MockInput() = default;

    void update() override;

    void addKeyHandler(std::function<void(SL::KeyType, SL::ActionType)> keyHandler) override;

    void addQuitHandler(std::function<void()> quitHandler) override;

    // Mock methods
    void simulateQuit();

    void simulateLeftPress();

    void simulateLeftRelease();

    void simulateRightPress();

    void simulateRightRelease();

    void simulateUpPress();

    void simulateUpRelease();

    void simulateDownPress();

    void simulateDownRelease();

    void simulateJumpPress();

    void simulateJumpRelease();

    bool updated{false};

    std::function<void()> _quitHandler{};
    std::function<void(SL::KeyType, SL::ActionType)> _keyHandler{};
};


