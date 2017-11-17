# SunnyLand
Game & simple game engine created in C++ using TDD.

Graphics sourced from [Pixel Game Art](http://pixelgameart.org/web/portfolio/sunnyland-forest/).

## Dependencies
 - CMake 3.8
 - SFML 2.4.2
 
## Tools used
 - Level editing: [Tiled](http://www.mapeditor.org/)
 - IDE: [CLion](https://www.jetbrains.com/clion/)
 
## Building
```bash
git clone git@github.com:ElFeesho/SunnyLand.git SunnyLand
cd SunnyLand
mkdir build
cd build
cmake ..
make -j4
ctest .
```
