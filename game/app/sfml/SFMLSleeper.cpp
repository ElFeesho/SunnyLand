#include "SFMLSleeper.h"

void SFMLSleeper::sleep(long currentTime) {
    if (currentTime-_lastTime < 1000/60) {
        sf::sleep(sf::milliseconds((1000/60)-(currentTime-_lastTime)));
    }
    _lastTime = currentTime;
}
