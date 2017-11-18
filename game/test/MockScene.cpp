#include "MockScene.h"

void MockScene::update(long delta) {
    updatedTimeDelta = delta;
}

void MockScene::keyEvent(SL::KeyType key, SL::ActionType action) {
    if (key == SL::KeyType::Left) {
        keyStream += "L";
    } else if (key == SL::KeyType::Right) {
        keyStream += "R";
    } else if (key == SL::KeyType::Up) {
        keyStream += "U";
    } else if (key == SL::KeyType::Down) {
        keyStream += "D";
    } else if (key == SL::KeyType::Jump) {
        keyStream += "J";
    }

    if(action == SL::ActionType::Press) {
        keyStream += "d";
    }
    else {
        keyStream += "u";
    }
}
