cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 14)

project(enwik8)
add_executable(enwik8
  src/main.cpp
  src/LZW/LZW.hpp
  src/LZW/LZW.cpp
  src/IO/IO.hpp
  src/IO/IO.cpp
  src/AC/AC.hpp
  src/AC/AC.cpp
  src/Model/ModelInterface.hpp
  src/Model/StaticModel.hpp
  src/Model/StaticModel.cpp
)

target_include_directories(enwik8 PRIVATE src)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT enwik8)
target_compile_definitions(enwik8 PRIVATE UNICODE _UNICODE _CRT_SECURE_NO_WARNINGS)

if (MSVC)
    add_compile_options(/W0 /Ox)
else()
    add_compile_options(-w -O3)
endif()
