#include <iostream>
#include "engine.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "sfml/SFMLGfx.h"
#include "sfml/SFMLInput.h"
#include "sfml/SFMLTime.h"
#include "sfml/SFMLSleeper.h"

#include "fileio.h"

class Player {
public:
    enum class State {
        Idle,
        Walk,
        Jump,
        Fall
    };

    Player(std::map<std::string, SL::Sprite> &&spriteSet);

    void draw(long delta, int x, int y);

    void lookLeft();

    void lookRight();

    void walk();

    void jump();

    void idle();

    void fall();

private:
    SL::Sprite _idle;
    SL::Sprite _walk;
    SL::Sprite _jump;
    SL::Sprite _fall;

    bool _right{true};

    State _state{State::Idle};
};

Player::Player(std::map<std::string, SL::Sprite> &&spriteSet) :
        _idle{spriteSet.at("idle")},
        _walk{spriteSet.at("walk")},
        _jump{spriteSet.at("jump")},
        _fall{spriteSet.at("fall")} {

}

void Player::draw(long delta, int x, int y) {
    SL::Sprite *sprite = &_idle;

    if (_state == State::Walk) {
        sprite = &_walk;
    } else if (_state == State::Jump) {
        sprite = &_jump;
    } else if (_state == State::Fall) {
        sprite = &_fall;
    }

    sprite->update(delta);
    sprite->draw(x, y, !_right);
}

void Player::lookLeft() {
    _right = false;
}

void Player::lookRight() {
    _right = true;
}

void Player::walk() {
    _state = State::Walk;
}

void Player::jump() {
    _state = State::Jump;
}

void Player::idle() {
    _state = State::Idle;
}

void Player::fall() {
    _state = State::Fall;
}

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

class MainMenuScene : public SL::Scene {
public:
    explicit MainMenuScene(SL::Engine &engine) :
            _engine{engine},
            _bg{_engine.createParallax("../Resources/island-background.png", 6.0f)},
            _mg{_engine.createParallax("../Resources/island-middleground.png", 1.0f)},
            _map{_engine.createMap(readFile("../Resources/first.json"), "../Resources/forest-tileset.png")},
            _player{SL::JSONSpriteFactory{engine}.parse(readFile("../Resources/rabbit.json"))} {

        _camera.position(_map.cameraSpawnX(), _map.cameraSpawnY());
        _camera.target(_map.playerSpawnX(), _map.playerSpawnY()-100);

        _playerX = _map.playerSpawnX();
        _playerY = _map.playerSpawnY();
    }

    void keyEvent(SL::KeyType key, SL::ActionType action) override {
        if (action == SL::ActionType::Press) {
            if (key == SL::KeyType::Right) {
                if (_playerYSpeed > -0.1 && _playerYSpeed < 0.1) {
                    _player.walk();
                    _player.lookRight();
                }
                _playerXSpeed = 2;
            } else if (key == SL::KeyType::Left) {
                if (_playerYSpeed > -0.1 && _playerYSpeed < 0.1) {
                    _player.walk();
                    _player.lookLeft();
                }
                _playerXSpeed = -2;
            } else if (key == SL::KeyType::Jump) {
                if (_playerYSpeed > -0.1 && _playerYSpeed < 0.1) {
                    _playerYSpeed = -5.0;
                    _player.jump();
                }
            }
        } else {
            if (key == SL::KeyType::Right || key == SL::KeyType::Left) {
                if (_playerYSpeed > -0.1 && _playerYSpeed < 0.1) {
                    _player.idle();
                }
                _playerXSpeed = 0;
            }
        }
    }

    void update(long delta) override {
        _bg.scroll(static_cast<int32_t>(-_camera.x()), static_cast<int32_t>(-_camera.y()));
        _mg.scroll(static_cast<int32_t>(-_camera.x()), static_cast<int32_t>(-_camera.y()));
        _bg.draw();
        _mg.draw();

        _map.layer(0).draw(static_cast<int32_t>(-_camera.x()), static_cast<int32_t>(-_camera.y()));
        _map.layer(1).draw(static_cast<int32_t>(-_camera.x()), static_cast<int32_t>(-_camera.y()));

        _playerX += _playerXSpeed;
        _playerY += _playerYSpeed;

        _playerYSpeed += 0.1;
        if (_playerYSpeed > 0.2) {
            _player.fall();
        }

        if (_playerY > _map.playerSpawnY()) {
            _playerY = _map.playerSpawnY();

            if (_playerXSpeed == 0) {
                _player.idle();
            } else {
                _player.walk();
            }

            _playerYSpeed = 0;
        }

        _camera.target(_playerX - 100, _playerY - 100);

        _player.draw(delta, static_cast<int>(_playerX - _camera.x()), static_cast<int>(_playerY - _camera.y()));

        _camera.pan();

    }

private:

    SL::Engine &_engine;
    SL::Parallax _bg;
    SL::Parallax _mg;
    SL::Tilemap _map;
    Player _player;

    double _playerX;
    double _playerY;

    double _playerXSpeed{0};
    double _playerYSpeed{0};

    Camera _camera{};
};


int main(int argc, char **argv) {
    sf::RenderWindow window{{800, 600}, "SunnyLand"};
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    SFMLGfx gfx{window};
    SFMLInput input{window};
    SFMLTime time{};
    SFMLSleeper sleeper{};

    SL::Engine engine{&gfx, &input, &time, &sleeper};

    MainMenuScene mainMenuScene{engine};

    engine.displayScene(&mainMenuScene);

    while (engine.update());

    return 0;
}