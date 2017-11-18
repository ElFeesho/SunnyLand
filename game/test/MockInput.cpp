#include "MockInput.h"

void MockInput::update() {
    updated = true;
}

void MockInput::addKeyHandler(std::function<void(SL::KeyType, SL::ActionType)> keyHandler) {
    _keyHandler = keyHandler;
}

void MockInput::addQuitHandler(std::function<void()> quitHandler) {
    _quitHandler = quitHandler;
}

void MockInput::simulateQuit() {
    _quitHandler();
}

void MockInput::simulateLeftPress() {
    _keyHandler(SL::KeyType::Left, SL::ActionType::Press);
}

void MockInput::simulateLeftRelease() {
    _keyHandler(SL::KeyType::Left, SL::ActionType::Release);
}

void MockInput::simulateRightPress() {
    _keyHandler(SL::KeyType::Right, SL::ActionType::Press);
}

void MockInput::simulateRightRelease() {
    _keyHandler(SL::KeyType::Right, SL::ActionType::Release);
}

void MockInput::simulateUpPress() {
    _keyHandler(SL::KeyType::Up, SL::ActionType::Press);
}

void MockInput::simulateUpRelease() {
    _keyHandler(SL::KeyType::Up, SL::ActionType::Release);
}

void MockInput::simulateDownPress() {
    _keyHandler(SL::KeyType::Down, SL::ActionType::Press);
}

void MockInput::simulateDownRelease() {
    _keyHandler(SL::KeyType::Down, SL::ActionType::Release);
}

void MockInput::simulateJumpPress() {
    _keyHandler(SL::KeyType::Jump, SL::ActionType::Press);
}

void MockInput::simulateJumpRelease() {
    _keyHandler(SL::KeyType::Jump, SL::ActionType::Release);
}
