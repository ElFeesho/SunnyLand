#include "../engine/engine.h"

class MockGfx : public SL::Gfx {
public:
    MockGfx() = default;

    void update() override {
        updated = true;
    };

    SL::Image loadImage(const std::string &filename) override {
        return _availableImages.at(filename);
    }

    void drawImage(SL::Image &image, int32_t x, int32_t y, int32_t sourceX, int32_t sourceY, int32_t w, int32_t h, bool horizontallyFlipped) override {
        drawnImage = image.filename()+","+std::to_string(x)+","+std::to_string(y)+","+std::to_string(sourceX)+","+std::to_string(sourceY)+","+std::to_string(w)+","+std::to_string(h)+","+std::to_string(horizontallyFlipped);
    }

    void drawBackgroundLayer(SL::Image &image, int32_t offsetX, int32_t offsetY) override {
        drawnLayer = image.filename()+","+std::to_string(offsetX)+","+std::to_string(offsetY);
    }

    // Mocked methods
    void simulateAvailableImage(const std::string &filename, uint32_t width, uint32_t height) {
        _availableImages.insert({filename, SL::Image{filename, width, height}});
    }

    std::string drawnImage{""};
    std::string drawnLayer{""};

    bool updated{false};

private:
    std::map<std::string, SL::Image> _availableImages;
};

class MockInput : public SL::Input {
public:
    MockInput() = default;

    void update() override {
        updated = true;
    }

    void addKeyHandler(std::function<void(SL::KeyType, SL::ActionType)> keyHandler) override {
        _keyHandler = keyHandler;
    }

    void addQuitHandler(std::function<void()> quitHandler) override {
        _quitHandler = quitHandler;
    }

    // Mock methods
    void simulateQuit() {
        _quitHandler();
    }

    void simulateLeftPress() {
        _keyHandler(SL::KeyType::Left, SL::ActionType::Press);
    }

    void simulateLeftRelease() {
        _keyHandler(SL::KeyType::Left, SL::ActionType::Release);
    }

    void simulateRightPress() {
        _keyHandler(SL::KeyType::Right, SL::ActionType::Press);
    }

    void simulateRightRelease() {
        _keyHandler(SL::KeyType::Right, SL::ActionType::Release);
    }

    void simulateUpPress() {
        _keyHandler(SL::KeyType::Up, SL::ActionType::Press);
    }

    void simulateUpRelease() {
        _keyHandler(SL::KeyType::Up, SL::ActionType::Release);
    }

    void simulateDownPress() {
        _keyHandler(SL::KeyType::Down, SL::ActionType::Press);
    }

    void simulateDownRelease() {
        _keyHandler(SL::KeyType::Down, SL::ActionType::Release);
    }

    void simulateJumpPress() {
        _keyHandler(SL::KeyType::Jump, SL::ActionType::Press);
    }

    void simulateJumpRelease() {
        _keyHandler(SL::KeyType::Jump, SL::ActionType::Release);
    }

    bool updated{false};

    std::function<void()> _quitHandler{};
    std::function<void(SL::KeyType, SL::ActionType)> _keyHandler{};
};

class MockTime : public SL::Time {
public:
    MockTime() = default;

    long currentTime() override {
        return _currentTime;
    }

    // Mock methods
    void simulateTime(long simulatedCurrentTime) {
        _currentTime = simulatedCurrentTime;
    }

    long _currentTime{0L};
};

class MockScene : public SL::Scene {
public:
    MockScene() = default;

    void update(long delta) override {
        updatedTimeDelta = delta;
    };

    void keyEvent(SL::KeyType key, SL::ActionType action) override {
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

    std::string keyStream{""};
    long updatedTimeDelta{0};
};

class MockSleeper : public SL::Sleeper {
public:
    void sleep(long currentTime) override {
        sleepInvokedTime = currentTime;
    }

    long sleepInvokedTime{0L};
};

TEST_CASE("[Engine]") {
    MockGfx mockGfx;
    MockInput mockInput;
    MockTime mockTime;
    MockScene mockScene;
    MockSleeper mockSleeper;

    mockGfx.simulateAvailableImage("test.xyz", 128, 32);
    mockGfx.simulateAvailableImage("layer.xyz", 200, 200);
    mockGfx.simulateAvailableImage("tilemap.xyz", 160, 160);

    SL::Engine engine{&mockGfx, &mockInput, &mockTime, &mockSleeper};

    engine.displayScene(&mockScene);

    SECTION("On update, graphics is updated") {
        engine.update();

        REQUIRE(mockGfx.updated);
    }


    SECTION("On update, input is updated") {
        engine.update();

        REQUIRE(mockInput.updated);
    }

    SECTION("Input can indicate game quit") {
        mockInput.simulateQuit();

        REQUIRE(!engine.update());
    }

    SECTION("Scene will be updated with the delta time since last update") {

        mockTime.simulateTime(0L);

        engine.update();

        REQUIRE(mockScene.updatedTimeDelta == 0L);

        mockTime.simulateTime(10L);

        engine.update();

        REQUIRE(mockScene.updatedTimeDelta == 10L);

        mockTime.simulateTime(30L);

        engine.update();

        REQUIRE(mockScene.updatedTimeDelta == 20L);
    }

    SECTION("Sleeper invoked on update") {
        mockTime.simulateTime(500L);
        engine.update();

        REQUIRE(mockSleeper.sleepInvokedTime == 500L);
    }

    SECTION("Engine can create a sprite from an image") {
        SL::Sprite sprite = engine.createSprite("test.xyz", 32, 32);

        REQUIRE(sprite.frameCount() == 4);
    }

    SECTION("Created sprites can render") {
        SL::Sprite sprite = engine.createSprite("test.xyz", 32, 32);

        sprite.draw(10, 20);

        REQUIRE(mockGfx.drawnImage == "test.xyz,10,20,0,0,32,32,0");
    }


    SECTION("Created sprites can render flipped horizontally") {
        SL::Sprite sprite = engine.createSprite("test.xyz", 32, 32);

        sprite.draw(10, 20, true);

        REQUIRE(mockGfx.drawnImage == "test.xyz,10,20,0,0,32,32,1");
    }

    SECTION("Sprites update their frames render") {

        SL::Sprite sprite = engine.createSprite("test.xyz", 32, 32);

        sprite.update(83L); // 12fps
        sprite.draw(10, 20);

        REQUIRE(mockGfx.drawnImage == "test.xyz,10,20,32,0,32,32,0");

        sprite.update(83L); // 12fps
        sprite.draw(10, 20);

        REQUIRE(mockGfx.drawnImage == "test.xyz,10,20,64,0,32,32,0");

        sprite.update(83L); // 12fps
        sprite.draw(10, 20);

        REQUIRE(mockGfx.drawnImage == "test.xyz,10,20,96,0,32,32,0");

        sprite.update(83L); // 12fps
        sprite.draw(10, 20);

        REQUIRE(mockGfx.drawnImage == "test.xyz,10,20,0,0,32,32,0");
    }

    SECTION("Scene given opportunity to observe input") {
        mockInput.simulateLeftPress();
        mockInput.simulateLeftRelease();

        mockInput.simulateRightPress();
        mockInput.simulateRightRelease();

        mockInput.simulateUpPress();
        mockInput.simulateUpRelease();

        mockInput.simulateDownPress();
        mockInput.simulateDownRelease();

        mockInput.simulateJumpPress();
        mockInput.simulateJumpRelease();

        REQUIRE(mockScene.keyStream == "LdLuRdRuUdUuDdDuJdJu");
    }

    SECTION("Parallax can be drawn") {

        SL::Parallax layer = engine.createParallax("layer.xyz", 1.0f);

        layer.draw();

        REQUIRE(mockGfx.drawnLayer == "layer.xyz,0,0");

        layer.scroll(5, 5);
        layer.draw();

        REQUIRE(mockGfx.drawnLayer == "layer.xyz,5,5");
    }

    SECTION("Parallax can be drawn with dampening") {

        SL::Parallax layer = engine.createParallax("layer.xyz", 2.0f);

        layer.draw();

        REQUIRE(mockGfx.drawnLayer == "layer.xyz,0,0");

        layer.scroll(10, 10);
        layer.draw();

        REQUIRE(mockGfx.drawnLayer == "layer.xyz,5,5");
    }

    SECTION("Tilemap can be parsed") {
        const std::string tilemap =
                "{"
                "   \"width\":2,"
                "   \"height\":2,"
                "   \"layers\":["
                "      {"
                "          \"name\":\"Background\","
                "          \"width\":2,"
                "          \"height\":2,"
                "          \"data\":["
                "              0, 1,"
                "              2, 3"
                "          ]"
                "      },"
                "      {"
                "          \"name\":\"Middleground\","
                "          \"width\":2,"
                "          \"height\":2,"
                "          \"data\":["
                "              4, 5,"
                "              6, 7"
                "          ]"
                "      }"
                "   ]"
                "}";

        auto gameMap = engine.createMap(tilemap, "tilemap.xyz");

        REQUIRE(gameMap.layer(0).tile(0, 0) == 0);
        REQUIRE(gameMap.layer(0).tile(1, 0) == 1);
        REQUIRE(gameMap.layer(0).tile(0, 1) == 2);
        REQUIRE(gameMap.layer(0).tile(1, 1) == 3);

        REQUIRE(gameMap.layer(1).tile(0, 0) == 4);
        REQUIRE(gameMap.layer(1).tile(1, 0) == 5);
        REQUIRE(gameMap.layer(1).tile(0, 1) == 6);
        REQUIRE(gameMap.layer(1).tile(1, 1) == 7);

        REQUIRE(gameMap.width() == 2);
        REQUIRE(gameMap.height() == 2);
    }

    SECTION("Tilemap rendering") {
        const std::string tilemap =
                "{"
                        "   \"width\":1,"
                        "   \"height\":1,"
                        "   \"layers\":["
                        "      {"
                        "          \"name\":\"Background\","
                        "          \"width\":1,"
                        "          \"height\":1,"
                        "          \"data\":["
                        "              1"
                        "          ]"
                        "      },"
                        "      {"
                        "          \"name\":\"Middleground\","
                        "          \"width\":1,"
                        "          \"height\":1,"
                        "          \"data\":["
                        "              12"
                        "          ]"
                        "      }"
                        "   ]"
                        "}";

        auto gameMap = engine.createMap(tilemap, "tilemap.xyz");

        gameMap.layer(0).draw(10, 10);

        REQUIRE(mockGfx.drawnImage == "tilemap.xyz,10,10,0,0,16,16,0");
        gameMap.layer(1).draw(20, 20);

        REQUIRE(mockGfx.drawnImage == "tilemap.xyz,20,20,16,16,16,16,0");
    }

    SECTION("Tiles of value 0 are not drawn") {
        const std::string tilemap =
                "{"
                        "   \"width\":1,"
                        "   \"height\":1,"
                        "   \"layers\":["
                        "      {"
                        "          \"name\":\"Background\","
                        "          \"width\":1,"
                        "          \"height\":1,"
                        "          \"data\":["
                        "              0"
                        "          ]"
                        "      },"
                        "      {"
                        "          \"name\":\"Middleground\","
                        "          \"width\":1,"
                        "          \"height\":1,"
                        "          \"data\":["
                        "              0"
                        "          ]"
                        "      }"
                        "   ]"
                        "}";

        auto gameMap = engine.createMap(tilemap, "tilemap.xyz");

        gameMap.layer(0).draw(10, 10);

        REQUIRE(mockGfx.drawnImage == "");
        gameMap.layer(1).draw(20, 20);

        REQUIRE(mockGfx.drawnImage == "");
    }
}