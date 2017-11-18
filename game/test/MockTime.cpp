#include "MockTime.h"

long MockTime::currentTime() {
    return _currentTime;
}

void MockTime::simulateTime(long simulatedCurrentTime) {
    _currentTime = simulatedCurrentTime;
}
