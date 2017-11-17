#pragma once

#include <SFML/System.hpp>
#include <engine.h>

class SFMLSleeper : public SL::Sleeper {
public:
    void sleep(long currentTime) override;

private:
    long _lastTime{0L};
};
;


