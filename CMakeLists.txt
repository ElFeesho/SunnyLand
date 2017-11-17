cmake_minimum_required(VERSION 3.8)
project(SunnyLand)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(game)

add_dependencies(sunnyland engine)