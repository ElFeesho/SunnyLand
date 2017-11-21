#include <iostream>
#include "engine.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <utility>

#include "sfml/SFMLGfx.h"
#include "sfml/SFMLInput.h"
#include "sfml/SFMLTime.h"
#include "sfml/SFMLSleeper.h"

#include "fileio.h"

#include "Camera.h"
#include "LevelBackground.h"

class PlayerPhysics {
public:

    void right();

    void left();

    void jump();

    void idle();

    void hitCeiling();

    void hitRightWall();

    void hitLeftWall();

    void hitFloor();

    void update(double &x, double &y);

    double xSpeed();

    double ySpeed();

    bool onFloor();

    void duck();

    bool isDucking();

private:
    double _xSpeed{0};
    double _ySpeed{0};

    bool _onFloor{true};
    double _xSpeedAccel{0.0};
    bool _ducking{false};
};

void PlayerPhysics::right() {
    _xSpeed = 1;
    _xSpeedAccel = 1.1;
}

void PlayerPhysics::left() {
    _xSpeed = -1;
    _xSpeedAccel = 1.1;
}

void PlayerPhysics::jump() {
    if (_onFloor) {
        _ySpeed = -5.0;
        _onFloor = false;
    }
}

void PlayerPhysics::hitCeiling() {
    _ySpeed *= -0.8;
}

void PlayerPhysics::hitRightWall() {
    _xSpeed = 0;
}

void PlayerPhysics::hitLeftWall() {
    _xSpeed = 0;
}

void PlayerPhysics::hitFloor() {
    _onFloor = true;
    _ySpeed = 0.0;
}

void PlayerPhysics::update(double &x, double &y) {
    x += _xSpeed;
    y += _ySpeed;

    _ySpeed += 0.1;
    _xSpeed *= _xSpeedAccel;

    _xSpeed = std::max(std::min(_xSpeed, 3.0), -3.0);

    if (std::abs(_xSpeed) <= 0.1) {
        _xSpeed = 0;
    }
}

double PlayerPhysics::xSpeed() {
    return _xSpeed;
}

double PlayerPhysics::ySpeed() {
    return _ySpeed;
}

bool PlayerPhysics::onFloor() {
    return _onFloor;
}

void PlayerPhysics::idle() {
    _xSpeedAccel = 0.8;
    _ducking = false;
}

void PlayerPhysics::duck() {
    _ducking = true;
}

bool PlayerPhysics::isDucking() {
    return _ducking;
}

class Player {
public:
    enum class State {
        Idle,
        Walk,
        Jump,
        Fall,
        Duck
    };

    explicit Player(std::map<std::string, SL::Sprite> &&spriteSet);

    void draw(long delta, int x, int y);

    void lookLeft();

    void lookRight();

    void walk();

    void jump();

    void idle();

    void fall();

    void duck();

private:
    SL::Sprite _idle;
    SL::Sprite _walk;
    SL::Sprite _jump;
    SL::Sprite _fall;
    SL::Sprite _duck;

    bool _right{true};

    State _state{State::Idle};
};

Player::Player(std::map<std::string, SL::Sprite> &&spriteSet) :
        _idle{spriteSet.at("idle")},
        _walk{spriteSet.at("walk")},
        _jump{spriteSet.at("jump")},
        _fall{spriteSet.at("fall")},
        _duck{spriteSet.at("duck")} {

}

void Player::draw(long delta, int x, int y) {
    SL::Sprite *sprite = &_idle;

    if (_state == State::Walk) {
        sprite = &_walk;
    } else if (_state == State::Jump) {
        sprite = &_jump;
    } else if (_state == State::Fall) {
        sprite = &_fall;
    } else if (_state == State::Duck) {
        sprite = &_duck;
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

void Player::duck() {
    _state = State::Duck;
}

class MainMenuScene : public SL::Scene {
public:
    explicit MainMenuScene(SL::Engine &engine) :
            _engine{engine},
            _map{_engine.createMap(readFile("../Resources/first.json"), "../Resources/forest-tileset.png")},
            _bg{engine, _map},
            _player{SL::JSONSpriteFactory{engine}.parse(readFile("../Resources/fox.json"))} {

        _camera.position(_map.cameraSpawnX(), _map.cameraSpawnY());
        _camera.target(_map.playerSpawnX(), _map.playerSpawnY() - 100);

        _playerX = _map.playerSpawnX();
        _playerY = _map.playerSpawnY();
    }

    void keyEvent(SL::KeyType key, SL::ActionType action) override {
        if (action == SL::ActionType::Press) {
            if (key == SL::KeyType::Right) {
                if (_playerPhysics.onFloor()) {
                    _player.walk();
                    _player.lookRight();
                }

                _playerPhysics.right();

            } else if (key == SL::KeyType::Left) {
                if (_playerPhysics.onFloor()) {
                    _player.walk();
                    _player.lookLeft();
                }

                _playerPhysics.left();
            } else if (key == SL::KeyType::Jump) {
                _playerPhysics.jump();
            } else if (key == SL::KeyType::Down) {
                _playerPhysics.duck();
                _player.duck();
            }
        } else {
            if (key == SL::KeyType::Right || key == SL::KeyType::Left) {
                _playerPhysics.idle();
                if (_playerPhysics.onFloor()) {
                    _player.idle();
                }
            } else if (key == SL::KeyType::Down) {
                _player.idle();
                _playerPhysics.idle();
            }
        }
    }

    void update(long delta) override {
        _bg.scroll(-_camera.x(), -_camera.y());
        _bg.draw();

        _map.layer(0).draw(static_cast<int32_t>(-_camera.x()), static_cast<int32_t>(-_camera.y()));
        _map.layer(1).draw(static_cast<int32_t>(-_camera.x()), static_cast<int32_t>(-_camera.y()));

        _playerPhysics.update(_playerX, _playerY);

        if (_playerPhysics.ySpeed() > 0.2) {
            _player.fall();
        } else if (_playerPhysics.ySpeed() < 0.2) {
            _player.jump();
        }

        if (_playerPhysics.ySpeed() > 0) {
            double playerYOffset = _playerY + 32;

            if (_map.checkCollisionDown(_playerX + 2, playerYOffset, _playerPhysics.ySpeed()) || _map.checkCollisionDown(_playerX + 30, playerYOffset, _playerPhysics.ySpeed())) {

                _playerY = playerYOffset - 32;
                _playerPhysics.hitFloor();

                if (_playerPhysics.xSpeed() == 0.0) {
                    if (_playerPhysics.isDucking()) {
                        _player.duck();
                    } else {
                        _player.idle();
                    }
                } else {
                    _player.walk();
                }
            }
        }

        if (_playerPhysics.ySpeed() < 0) {
            if (_map.checkCollisionUp(_playerX + 2, _playerY, _playerPhysics.ySpeed()) || _map.checkCollisionUp(_playerX + 30, _playerY, _playerPhysics.ySpeed())) {
                _playerPhysics.hitCeiling();
            }
        }

        if (_playerPhysics.xSpeed() < 0) {
            if (_map.checkCollisionLeft(_playerX, _playerY+2, _playerPhysics.xSpeed()) || _map.checkCollisionLeft(_playerX, _playerY+30, _playerPhysics.xSpeed())) {
                _playerPhysics.hitLeftWall();
            }
        } else if (_playerPhysics.xSpeed() > 0) {
            double playerXOffset = _playerX+32;
            if (_map.checkCollisionRight(playerXOffset, _playerY+2, _playerPhysics.xSpeed()) || _map.checkCollisionLeft(playerXOffset, _playerY+30, _playerPhysics.xSpeed())) {
                _playerX = playerXOffset-32;
                _playerPhysics.hitRightWall();
            }
        }

        _camera.target(_playerX - 100, _playerY - 100);

        _player.draw(delta, static_cast<int>(_playerX - _camera.x()), static_cast<int>(_playerY - _camera.y()));

        _camera.pan();
    }

private:

    SL::Engine &_engine;
    SL::Tilemap _map;
    LevelBackground _bg;
    Player _player;

    double _playerX;
    double _playerY;

    PlayerPhysics _playerPhysics{};

    Camera _camera{};
};

class TitleScene : public SL::Scene {
public:
    TitleScene(SL::Engine &engine, std::function<void()> closeScreen) : _bg{engine.createParallax("../Resources/island-background.png", 4.0f)}, _mg{engine.createParallax("../Resources/island-middleground.png", 1.0f)}, _title{engine.createSprite("../Resources/title.png")}, _closeScreen{
            std::move(closeScreen)} {

    }

    void update(long delta) override {
        _bg.scroll(_scroll, 0);
        _mg.scroll(_scroll, 0);
        _scroll++;
        _bg.draw();
        _mg.draw();

        _title.draw(100, 100+std::sin(_phase)*20.0);
        _phase += 0.05;
    }

    void keyEvent(SL::KeyType key, SL::ActionType action) override {
        if (action == SL::ActionType::Release && key == SL::KeyType::Jump) {
            _closeScreen();
        }
    }

private:
    SL::Sprite _title;
    std::function<void()> _closeScreen;
    SL::Parallax _bg;
    SL::Parallax _mg;

    int32_t _scroll{0};

    double _phase{0.0};
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
    TitleScene titleScene{engine, [&]{
        engine.displayScene(&mainMenuScene);
    }};

    engine.displayScene(&titleScene);

    while (engine.update());

    return 0;
}