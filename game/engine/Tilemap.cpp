#include "engine.h"

SL::Tilemap::Tilemap(uint32_t width, uint32_t height, std::vector<Layer> layers, int32_t playerSpawnX, int32_t playerSpawnY, int32_t cameraSpawnX, int32_t cameraSpawnY, Image bgImage, Image mgImage)
        : _w{width}, _h{height}, _layers{std::move(layers)}, _playerSpawnX{playerSpawnX}, _playerSpawnY{playerSpawnY}, _cameraSpawnX{cameraSpawnX}, _cameraSpawnY{cameraSpawnY}, _bgImage{
        std::move(bgImage)}, _mgImage{std::move(mgImage)} {

}

SL::Tilemap::Layer &SL::Tilemap::layer(uint32_t index) {
    return _layers[index];
}

uint32_t SL::Tilemap::width() {
    return _w;
}

uint32_t SL::Tilemap::height() {
    return _h;
}

int32_t SL::Tilemap::playerSpawnX() {
    return _playerSpawnX;
}

int32_t SL::Tilemap::playerSpawnY() {
    return _playerSpawnY;
}

int32_t SL::Tilemap::cameraSpawnX() {
    return _cameraSpawnX;
}

int32_t SL::Tilemap::cameraSpawnY() {
    return _cameraSpawnY;
}

SL::Image &SL::Tilemap::bgImage() {
    return _bgImage;
}

SL::Image &SL::Tilemap::mgImage() {
    return _mgImage;
}

bool SL::Tilemap::checkCollisionDown(double x, double &y, double travelSpeed) {

    uint32_t tx = static_cast<uint32_t>(x / 16);
    uint32_t ty = static_cast<uint32_t>((y + travelSpeed) / 16);

    if (layer(2).tile(tx, ty) != 0) {
        y = ty * 16;
        return true;
    }

    return false;
}

bool SL::Tilemap::checkCollisionRight(double &x, double y, double travelSpeed) {
    uint32_t tx = static_cast<uint32_t>((x+travelSpeed) / 16);
    uint32_t ty = static_cast<uint32_t>(y / 16);

    if (layer(2).tile(tx, ty) != 0) {
        x = tx * 16;
        return true;
    }

    return false;
}

bool SL::Tilemap::checkCollisionLeft(double &x, double y, double travelSpeed) {
    uint32_t tx = static_cast<uint32_t>((x+travelSpeed) / 16);
    uint32_t ty = static_cast<uint32_t>(y / 16);

    if (layer(2).tile(tx, ty) != 0) {
        x = (tx+1) * 16;
        return true;
    }

    return false;
}

bool SL::Tilemap::checkCollisionUp(double x, double &y, double travelSpeed) {
    uint32_t tx = static_cast<uint32_t>(x / 16);
    uint32_t ty = static_cast<uint32_t>((y + travelSpeed) / 16);

    if (layer(2).tile(tx, ty) != 0) {
        y = (ty+1) * 16;
        return true;
    }

    return false;
}

SL::Tilemap::Layer::Layer(Gfx *gfx, Image tileset, uint32_t width, uint32_t height, std::vector<uint32_t> tiles) : _gfx{gfx}, _tileset{std::move(tileset)}, _w{width}, _h{height}, _tiles{
        std::move(tiles)} {
}

int32_t SL::Tilemap::Layer::tile(uint32_t x, uint32_t y) {
    return _tiles[y * _w + x];
}

void SL::Tilemap::Layer::draw(int32_t x, int32_t y) {

    for (uint32_t ty = 0; ty < _h; ty++) {
        for (uint32_t tx = 0; tx < _w; tx++) {
            drawTile(tx, ty, x + tx * 16, y + ty * 16);
        }
    }
}

void SL::Tilemap::Layer::drawTile(uint32_t tileX, uint32_t tileY, int32_t x, int32_t y) {
    auto tileNumber = tile(tileX, tileY);

    if (tileNumber > 0) {
        tileNumber -= 1;
        int32_t sourceX = tileNumber % (_tileset.width() / 16);
        int32_t sourceY = tileNumber / (_tileset.width() / 16);
        _gfx->drawImage(_tileset, x, y, sourceX * 16, sourceY * 16, 16, 16, false);
    }
}