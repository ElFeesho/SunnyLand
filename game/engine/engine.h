#pragma once
#include <string>
#include <functional>
#include <vector>
#include "json.hpp"

namespace SL {

    enum class KeyType {
        Left,
        Right,
        Up,
        Down,
        Jump
    };

    enum class ActionType {
        Press,
        Release
    };

    class Gfx;

    class Image {
    public:
        Image(const std::string &filename, uint32_t width, uint32_t height);

        const std::string &filename() const;
        uint32_t height();
        uint32_t width();
    private:
        std::string _filename;
        uint32_t _width;
        uint32_t _height;
    };

    class Parallax {
    public:
        Parallax(Gfx *gfx, Image image, float travelFactor);
        void draw();

        void scroll(int32_t scrollX, int32_t scrollY);

    private:
        Gfx *_gfx;
        Image _image;
        float _travelFactor;
        int32_t _x{0};
        int32_t _y{0};
    };

    class Sprite {
    public:
        Sprite(Gfx *gfx, Image image, uint32_t cellWidth, uint32_t cellHeight);

        uint32_t frameCount();

        void draw(int32_t x, int32_t y, bool horizontallyFlipped = false);

        void update(long timeDelta);

    private:
        Gfx *_gfx;
        Image _image;
        uint32_t _cellWidth;
        uint32_t _cellHeight;
        uint32_t _frames;

        uint32_t _lastTicks{0};
        uint32_t _frame{0};
    };

    class Gfx {
    public:
        virtual void update() = 0;
        virtual Image loadImage(const std::string &basic_string) = 0;
        virtual void drawImage(Image &image, int32_t x, int32_t y, int32_t sourceX, int32_t sourceY, int32_t w, int32_t h, bool horizontallyFlipped) = 0;
        virtual void drawBackgroundLayer(Image &image, int32_t offsetX, int32_t offsetY) = 0;
    };

    class Sleeper {
    public:
        virtual void sleep(long currentTime) = 0;
    };

    class Input {
    public:
        virtual void update() = 0;

        virtual void addKeyHandler(std::function<void(KeyType, ActionType)> keyHandler) = 0;
        virtual void addQuitHandler(std::function<void()> quitHandler) = 0;
    };

    class Time {
    public:
        virtual long currentTime() = 0;
    };

    class Scene {
    public:
        virtual void update(long delta) = 0;
        virtual void keyEvent(KeyType key, ActionType action) = 0;
    };

    class Tilemap {
    public:
        class Layer {
        public:
            Layer(Gfx *gfx, Image tileset, uint32_t width, uint32_t height, std::vector<uint32_t> tiles);
            int32_t tile(uint32_t x, uint32_t y);

            void draw(int32_t x, int32_t y);
        private:
            void drawTile(uint32_t tileX, uint32_t tileY, int32_t x, int32_t y);
            Gfx *_gfx;
            Image _tileset;
            uint32_t _w;
            uint32_t _h;
            std::vector<uint32_t> _tiles;
        };

        Tilemap(uint32_t width, uint32_t height, std::vector<Layer> layers, int32_t playerSpawnX, int32_t playerSpawnY, int32_t cameraSpawnX, int32_t cameraSpawnY, Image bgImage, Image mgImage);

        Layer &layer(uint32_t index);

        int32_t playerSpawnX();
        int32_t playerSpawnY();

        int32_t cameraSpawnX();
        int32_t cameraSpawnY();

        Image &bgImage();
        Image &mgImage();

        uint32_t width();
        uint32_t height();

        bool checkCollisionDown(double x, double &y, double travelSpeed);

        bool checkCollisionRight(double &x, double y, double travelSpeed);

        bool checkCollisionLeft(double &x, double y, double travelSpeed);

        bool checkCollisionUp(double x, double &y, double travelSpeed);

    private:
        std::vector<Layer> _layers;
        uint32_t _w;
        uint32_t _h;

        int32_t _playerSpawnX;
        int32_t _playerSpawnY;
        int32_t _cameraSpawnX;
        int32_t _cameraSpawnY;

        Image _bgImage;
        Image _mgImage;
    };


    class Engine {
    public:
        Engine(Gfx *gfx, Input *input, Time *time, Sleeper *sleeper);

        bool update();

        void displayScene(Scene *scene);

        Sprite createSprite(const std::string &filename, uint32_t cellWidth, uint32_t cellHeight);

        Parallax createParallax(const std::string &filename, float travelDampening);

        Tilemap createMap(std::string mapData, std::string tilesetFile);

        Sprite createSprite(const std::string &imageFilename);

    private:
        Gfx *_gfx{nullptr};
        Input *_input{nullptr};
        Time *_time{nullptr};
        Sleeper *_sleeper{nullptr};
        Scene *_activeScene{nullptr};


        bool _alive{true};
        long _lastTime{0L};
    };

    class JSONSpriteFactory {
    public:
        explicit JSONSpriteFactory(Engine &engine);

        std::map<std::string, SL::Sprite> parse(const std::string &spritesJson);

    private:
        Engine &_engine;
    };
}


