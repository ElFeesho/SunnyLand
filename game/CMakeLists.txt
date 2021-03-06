set(CMAKE_CXX_STANDARD 11)

#
# Dependency resolution
#

find_package(SFML 2 COMPONENTS system window graphics audio REQUIRED)

#
# Game Engine
#

add_library(engine STATIC engine/engine.cpp engine/json.hpp engine/Parallax.cpp engine/Tilemap.cpp engine/Image.cpp engine/Sprite.cpp engine/JSONSpriteFactory.cpp)
target_link_libraries(engine INTERFACE ${SFML_LIBRARIES})
target_include_directories(engine PUBLIC engine)

#
# Game
#

set(RESOURCES resources/spritesheets/player/climb.png
        resources/spritesheets/player/duck.png
        resources/spritesheets/player/fall.png
        resources/spritesheets/player/hurt.png
        resources/spritesheets/player/idle.png
        resources/spritesheets/player/jump.png
        resources/spritesheets/player/skip.png
        resources/spritesheets/player/fox-player-climb.png
        resources/spritesheets/player/fox-player-duck.png
        resources/spritesheets/player/fox-player-fall.png
        resources/spritesheets/player/fox-player-hurt.png
        resources/spritesheets/player/fox-player-idle.png
        resources/spritesheets/player/fox-player-jump.png
        resources/spritesheets/player/fox-player-run.png
        resources/spritesheets/enemies/bee.png
        resources/spritesheets/enemies/slug.png
        resources/spritesheets/enemies/piranha-plant.png
        resources/spritesheets/enemies/piranha-plant-attack.png
        resources/spritesheets/misc/carrot.png
        resources/spritesheets/misc/star.png
        resources/spritesheets/misc/chest.png
        resources/spritesheets/misc/enemy-death.png
        resources/spritesheets/misc/hud.png
        resources/environment/layers/forest-background.png
        resources/environment/layers/forest-middleground.png
        resources/environment/layers/island-background.png
        resources/environment/layers/island-middleground.png
        resources/environment/layers/forest-tileset.png
        resources/environment/layers/island-tileset.png
        resources/maps/first.json
        )

file(GLOB_RECURSE RES_SOURCES "resources/*")

add_executable(sunnyland
        MACOSX_BUNDLE
        app/main.cpp
        app/Camera.cpp
        app/LevelBackground.cpp
        app/fileio.cpp
        app/fileio.h
        app/sfml/SFMLGfx.cpp app/sfml/SFMLGfx.h
        app/sfml/SFMLInput.cpp app/sfml/SFMLInput.h
        app/sfml/SFMLTime.cpp app/sfml/SFMLTime.h
        app/sfml/SFMLSleeper.cpp app/sfml/SFMLSleeper.h
        ${RES_SOURCES})

set_source_files_properties(${RES_SOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

target_link_libraries(sunnyland PUBLIC engine ${SFML_LIBRARIES})

#
# Unit tests
#

enable_testing()

add_library(mocks STATIC test/MockGfx.cpp test/MockInput.cpp test/MockScene.cpp test/MockSleeper.cpp test/MockTime.cpp)
target_link_libraries(mocks PUBLIC engine)

function(create_test_target target_name)
    set(OTHER_FILES "")
    set(OTHER_LIBS "")

    set(INLIBS OFF)
    foreach (f ${ARGN})
        if (${f} STREQUAL LINK)
            set(INLIBS ON)
        else ()
            if (${INLIBS})
                set(OTHER_LIBS ${OTHER_LIBS} ${f})
            else ()
                set(OTHER_FILES ${OTHER_FILES} ${f})
            endif ()
        endif ()
    endforeach ()
    add_test(${target_name} ${target_name})
    add_executable(${target_name} test/${target_name}.t.cpp ${OTHER_FILES})
    add_dependencies(${target_name} engine sunnyland)
    target_link_libraries(${target_name} PRIVATE ${OTHER_LIBS} mocks)
    set_source_files_properties(test/${target_name}.t.cpp PROPERTIES COMPILE_FLAGS "-DCATCH_CONFIG_MAIN -include ${CMAKE_CURRENT_SOURCE_DIR}/test/catch.h")
endfunction()

create_test_target(engine_test LINK engine)