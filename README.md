# enwik8
An attempt to compress the first 100MB of Wikipedia.

## Build
- The executable is built using [CMake](https://cmake.org/) and your build system of choice.
- An example using [Ninja](https://ninja-build.org/) would be
    `cmake -G Ninja -B bld && cmake --build bld`

## Usage
`enwik8 encode|decode /path/to/in /path/to/out`
