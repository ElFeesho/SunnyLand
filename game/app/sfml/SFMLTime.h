#pragma once

#include <engine.h>
#include <SFML/System.hpp>

class SFMLTime : public SL::Time {
public:
    long currentTime() override;

private:
    sf::Clock _clock{};
};