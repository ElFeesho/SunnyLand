#include "Camera.h"

void Camera::position(double x, double y) {
    _camX = x;
    _camY = y;
}

void Camera::target(double x, double y) {
    _camTargetX = x;
    _camTargetY = y;
}

void Camera::pan() {
    _camX += (_camTargetX - _camX) / 8.0;
    _camY += (_camTargetY - _camY) / 8.0;
}

int32_t Camera::x() {
    return static_cast<int32_t>(_camX);
}

int32_t Camera::y() {
    return static_cast<int32_t>(_camY);
}