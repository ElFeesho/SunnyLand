#include <iostream>
#include "engine.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "sfml/SFMLGfx.h"
#include "sfml/SFMLInput.h"
#include "sfml/SFMLTime.h"

#include <fstream>

std::string loadMap(const std::string &tilemap) {
    std::string mapData;

    std::fstream mapFile{tilemap, std::ios::in};

    std::copy(std::istream_iterator<char>{mapFile}, std::istream_iterator<char>{}, std::back_inserter(mapData));

    mapFile.close();

    return mapData;
}


class MainMenuScene : public SL::Scene {
public:
    explicit MainMenuScene(SL::Engine &engine) :
            _engine{engine},
            _idle{_engine.createSprite("../Resources/fox-player-idle.png", 33, 32)},
            _skip{_engine.createSprite("../Resources/fox-player-run.png", 33, 32)},
            _bg{_engine.createParallax("../Resources/island-background.png", 8.0f)},
            _mg{_engine.createParallax("../Resources/island-middleground.png", 4.0f)},
            _map{_engine.createMap(loadMap("../Resources/first.json"), "../Resources/forest-tileset.png")}
             {
    }

    void keyEvent(SL::KeyType key, SL::ActionType action) override {
        if (key == SL::KeyType::Right || key == SL::KeyType::Left) {
            if (action == SL::ActionType::Press) {
                _activeSprite = &_skip;
                _facingLeft = key == SL::KeyType::Left;
                _bgSpeed = _facingLeft?1:-1;
            }
            else {
                _activeSprite = &_idle;
                _bgSpeed = 0;
            }
        }
    }

    void update(long delta) override {
        _bg.scroll(_bgSpeed, 0);
        _mg.scroll(_bgSpeed, 0);
        _bg.draw();
        _mg.draw();

        _map.layer(0).draw(0, 0);
        _map.layer(1).draw(0, 0);

        _activeSprite->update(delta);
        _activeSprite->draw(100, 100, _facingLeft);
    }
private:
    SL::Engine &_engine;
    SL::Sprite _idle;
    SL::Sprite _skip;
    SL::Sprite *_activeSprite{&_idle};
    SL::Parallax _bg;
    SL::Parallax _mg;
    SL::Tilemap _map;
    bool _facingLeft{false};
    int32_t _bgSpeed{0};
};

class SFMLSleeper : public SL::Sleeper {
public:
    void sleep(long currentTime) override {
        if (currentTime-_lastTime < 1000/60) {
            sf::sleep(sf::milliseconds((1000/60)-(currentTime-_lastTime)));
        }
        _lastTime = currentTime;
    }

private:
    long _lastTime{0L};
};


int main(int argc, char **argv) {
    sf::RenderWindow window{{800, 600}, "SunnyLand"};

    SFMLGfx gfx{window};
    SFMLInput input{window};
    SFMLTime time{};
    SFMLSleeper sleeper{};

    SL::Engine engine{&gfx, &input, &time, &sleeper};

    MainMenuScene mainMenuScene{engine};

    engine.displayScene(&mainMenuScene);

    while (engine.update()) {

    }

    return 0;
}