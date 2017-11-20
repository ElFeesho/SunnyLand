#pragma once

#include <cstdint>

class Camera {
public:
    void position(double x, double y);

    void target(double x, double y);

    void pan();

    int32_t x();
    int32_t y();

private:
    double _camX{0};
    double _camY{0};

    double _camTargetX{0};
    double _camTargetY{0};
};