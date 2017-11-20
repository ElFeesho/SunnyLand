#include <engine.h>
#include <json.hpp>
#include "MockSleeper.h"
#include "MockGfx.h"
#include "MockInput.h"
#include "MockTime.h"
#include "MockScene.h"

TEST_CASE("[Engine]") {
    MockGfx mockGfx;
    MockInput mockInput;
    MockTime mockTime;
    MockScene mockScene;
    MockSleeper mockSleeper;

    mockGfx.simulateAvailableImage("test.xyz", 128, 32);
    mockGfx.simulateAvailableImage("layer.xyz", 200, 200);
    mockGfx.simulateAvailableImage("tilemap.xyz", 160, 160);
    mockGfx.simulateAvailableImage("bg.xyz", 10, 10);
    mockGfx.simulateAvailableImage("mg.xyz", 10, 10);

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
                R"({
   "width":2,
   "height":2,
   "layers":[
      {
          "name":"Background",
          "width":2,
          "height":2,
          "data":[
              0, 1,
              2, 3
          ]
      },
      {
          "name":"Middleground",
          "width":2,
          "height":2,
          "data":[
              4, 5,
              6, 7
          ]
      }
   ],
   "properties": {
        "background": "bg.xyz",
        "middleground": "mg.xyz"
   }
})";

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
                R"({
   "width":1,
   "height":1,
   "layers":[
      {
          "name":"Background",
          "width":1,
          "height":1,
          "data":[1]
      },
      {
          "name":"Middleground",
          "width":1,
          "height":1,
          "data":[12]
      }
   ],
   "properties": {
        "background": "bg.xyz",
        "middleground": "mg.xyz"
   }
})";

        auto gameMap = engine.createMap(tilemap, "tilemap.xyz");

        gameMap.layer(0).draw(10, 10);

        REQUIRE(mockGfx.drawnImage == "tilemap.xyz,10,10,0,0,16,16,0");
        gameMap.layer(1).draw(20, 20);

        REQUIRE(mockGfx.drawnImage == "tilemap.xyz,20,20,16,16,16,16,0");
    }

    SECTION("Tiles of value 0 are not drawn") {
        const std::string tilemap =
                R"({
   "width":1,
   "height":1,
   "layers":[
      {
          "name":"Background",
          "width":1,
          "height":1,
          "data":[0]
      },
      {
          "name":"Middleground",
          "width":1,
          "height":1,
          "data":[0]
      }
   ],
   "properties": {
        "background": "bg.xyz",
        "middleground": "mg.xyz"
   }
})";

        auto gameMap = engine.createMap(tilemap, "tilemap.xyz");

        gameMap.layer(0).draw(10, 10);

        REQUIRE(mockGfx.drawnImage == "");
        gameMap.layer(1).draw(20, 20);

        REQUIRE(mockGfx.drawnImage == "");
    }

    SECTION("Player spawn can be read") {
        const std::string tilemap =
                R"({
   "width":1,
   "height":1,
   "layers":[
      {
          "name":"Background",
          "width":1,
          "height":1,
          "data":[0]
      },
      {
          "name":"Middleground",
          "width":1,
          "height":1,
          "data":[0]
      },
      {
          "type":"objectgroup",
          "objects":[
               {
                   "type":"player_spawn",
                   "x":123,
                   "y":345
               }
           ]
       }
   ],
   "properties": {
        "background": "bg.xyz",
        "middleground": "mg.xyz"
   }
})";

        auto gameMap = engine.createMap(tilemap, "tilemap.xyz");

        REQUIRE(gameMap.playerSpawnX() == 123);
        REQUIRE(gameMap.playerSpawnY() == 345);
    }

    SECTION("Camera spawn can be read") {
        const std::string tilemap =
                R"({
   "width":1,
   "height":1,
   "layers":[
      {
          "name":"Background",
          "width":1,
          "height":1,
          "data":[0]
      },
      {
          "name":"Middleground",
          "width":1,
          "height":1,
          "data":[0]
      },
      {
          "type":"objectgroup",
          "objects":[
               {
                   "type":"camera_spawn",
                   "x":123,
                   "y":345
               }
           ]
       }
   ],
   "properties": {
        "background": "bg.xyz",
        "middleground": "mg.xyz"
   }
})";

        auto gameMap = engine.createMap(tilemap, "tilemap.xyz");

        REQUIRE(gameMap.cameraSpawnX() == 123);
        REQUIRE(gameMap.cameraSpawnY() == 345);
    }

    SECTION("Parallax backgrounds can be processed") {
        const std::string tilemap =
                R"({
   "width":1,
   "height":1,
   "layers":[
      {
          "name":"Background",
          "width":1,
          "height":1,
          "data":[0]
      },
      {
          "name":"Middleground",
          "width":1,
          "height":1,
          "data":[0]
      },
      {
          "type":"objectgroup",
          "objects":[
               {
                   "type":"camera_spawn",
                   "x":123,
                   "y":345
               }
           ]
       }
   ],
   "properties": {
        "background": "bg.xyz",
        "middleground": "mg.xyz"
   }
})";

        auto gameMap = engine.createMap(tilemap, "tilemap.xyz");

        REQUIRE(gameMap.bgImage().filename() == "bg.xyz");
        REQUIRE(gameMap.mgImage().filename() == "mg.xyz");
    }

    SECTION("JSON Sprite factory can parse multiple sprites") {
        const std::string sprites =
                R"({
    "sprites":[
        {
            "name":"idle",
            "cw":32,
            "ch":32,
            "img":"idle.xyz"
        },
        {
            "name":"walk",
            "cw":32,
            "ch":32,
            "img":"walk.xyz"
        },
        {
            "name":"jump",
            "cw":32,
            "ch":32,
            "img":"jump.xyz"
        }
    ]
})";
        mockGfx.simulateAvailableImage("idle.xyz", 32, 32);
        mockGfx.simulateAvailableImage("walk.xyz", 64, 32);
        mockGfx.simulateAvailableImage("jump.xyz", 96, 32);

        SL::JSONSpriteFactory jsonSpriteFactory{engine};

        std::map<std::string, SL::Sprite> spriteMap = jsonSpriteFactory.parse(sprites);

        REQUIRE(spriteMap.at("idle").frameCount() == 1);
        REQUIRE(spriteMap.at("walk").frameCount() == 2);
        REQUIRE(spriteMap.at("jump").frameCount() == 3);


    }
}
