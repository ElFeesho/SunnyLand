#include <iostream>
#include "engine.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "sfml/SFMLGfx.h"
#include "sfml/SFMLInput.h"
#include "sfml/SFMLTime.h"
#include "sfml/SFMLSleeper.h"

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
            _bg{_engine.createParallax("../Resources/island-background.png", 4.0f)},
            _mg{_engine.createParallax("../Resources/island-middleground.png", 2.0f)},
            _map{_engine.createMap(loadMap("../Resources/first.json"), "../Resources/forest-tileset.png")}
             {
    }

    void keyEvent(SL::KeyType key, SL::ActionType action) override {
        if (key == SL::KeyType::Right || key == SL::KeyType::Left) {
            if (action == SL::ActionType::Press) {
                _activeSprite = &_skip;
                _facingLeft = key == SL::KeyType::Left;
                _bgSpeed = _facingLeft?1:-1;

                _pxspeed = _facingLeft? -1.0:1.0;
            }
            else {
                _activeSprite = &_idle;
                _bgSpeed = 0;
                _pxspeed = 0.0;
            }
        }

        if (key == SL::KeyType::Jump && action == SL::ActionType::Press && _pyspeed == 0.0) {
            _pyspeed = -5.0;
        }
    }

    void update(long delta) override {
        _bg.scroll(-_px, 0);
        _mg.scroll(-_px, 0);
        _bg.draw();
        _mg.draw();

        _map.layer(0).draw(0, 0);
        _map.layer(1).draw(0, 0);

        _activeSprite->update(delta);
        _activeSprite->draw(_px, _py-32, _facingLeft);

        if (_pyspeed < 3.0) {
            _pyspeed += 0.1;
        }

        _py += _pyspeed;
        _px += _pxspeed;

        if (_map.layer(2).tile((_px+9)/16, (_py)/16)!=0 || _map.layer(2).tile((_px+23)/16, (_py)/16)!=0) {
            _py = (_py/16)*16;
            _pyspeed = 0.0;
        }

        if (_map.layer(2).tile((_px+9)/16, (_py-24)/16)!=0 || _map.layer(2).tile((_px+23)/16, (_py-24)/16)!=0) {
            _pyspeed = 1.0;
        }


        if (_map.layer(2).tile((_px+8)/16, (_py-1)/16)!=0 || _map.layer(2).tile((_px+8)/16, (_py-23)/16)!=0) {
            _px = (_px/16)*16 + 8;
        }
        if (_map.layer(2).tile((_px+24)/16, (_py-1)/16)!=0 || _map.layer(2).tile((_px+24)/16, (_py-23)/16)!=0) {
            _px = ((_px+24)/16)*16 - 24;
        }

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

    int32_t _px{100};
    int32_t _py{100};

    double _pxspeed{0.0};
    double _pyspeed{1.0};
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