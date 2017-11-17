#include "SFMLInput.h"

SFMLInput::SFMLInput(sf::RenderWindow &window) : _window{window} {

}

void SFMLInput::update() {
    static auto interestingKeys = std::vector<std::pair<sf::Keyboard::Key, SL::KeyType>>{{sf::Keyboard::Key::Left,  SL::KeyType::Left},
                                                                                         {sf::Keyboard::Key::Right, SL::KeyType::Right},
                                                                                         {sf::Keyboard::Key::Down,  SL::KeyType::Down},
                                                                                         {sf::Keyboard::Key::Up,    SL::KeyType::Up},
                                                                                         {sf::Keyboard::Key::Space, SL::KeyType::Jump}};

    sf::Event e{};
    while (_window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            _quitHandler();
        }

        for (auto &keymap : interestingKeys) {
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == keymap.first) {
                    _keyHandler(keymap.second, SL::ActionType::Press);
                }
            }

            if (e.type == sf::Event::KeyReleased) {
                if (e.key.code == keymap.first) {
                    _keyHandler(keymap.second, SL::ActionType::Release);
                }
            }
        }
    }
}

void SFMLInput::addQuitHandler(std::function<void()> quitHandler) {
    _quitHandler = quitHandler;
}

void SFMLInput::addKeyHandler(std::function<void(SL::KeyType, SL::ActionType)> keyHandler) {
    _keyHandler = keyHandler;
}
