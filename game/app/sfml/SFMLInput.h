#pragma once

#include <functional>

#include <engine.h>
#include <SFML/Graphics.hpp>

class SFMLInput : public SL::Input {
public:
    explicit SFMLInput(sf::RenderWindow &window);

    void update() override;

    void addKeyHandler(std::function<void(SL::KeyType, SL::ActionType)> keyHandler) override;

    void addQuitHandler(std::function<void()> quitHandler) override;

private:
    sf::RenderWindow &_window;
    std::function<void()> _quitHandler;
    std::function<void(SL::KeyType, SL::ActionType)> _keyHandler;
};