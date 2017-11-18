#pragma once

#include <engine.h>

class MockTime : public SL::Time {
public:
    MockTime() = default;

    long currentTime() override;

    // Mock methods
    void simulateTime(long simulatedCurrentTime);

    long _currentTime{0L};
};



