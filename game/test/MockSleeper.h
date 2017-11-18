#pragma once

#include <engine.h>

class MockSleeper : public SL::Sleeper {
public:
    void sleep(long currentTime) override;

    long sleepInvokedTime{0L};
};
