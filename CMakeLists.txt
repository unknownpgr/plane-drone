cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
include(pico_sdk_import.cmake)
pico_sdk_init()

project(drone C CXX ASM)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${SRC_DIR})
file(GLOB SOURCE_ARDUINO ${SRC_DIR}/arduino/*.c)
file(GLOB SOURCE_PICO ${SRC_DIR}/pico/*.c)
set(SOURCES
    ${SRC_DIR}/main.c
    ${SOURCE_PICO}
)

add_compile_options(-Wall)
add_executable(drone ${SOURCES})
target_include_directories(drone PRIVATE ${INC_DIR})
target_link_libraries(drone pico_stdlib hardware_i2c)
pico_add_extra_outputs(drone)