#include "SFMLTime.h"

long SFMLTime::currentTime() {
    return _clock.getElapsedTime().asMilliseconds();
}
