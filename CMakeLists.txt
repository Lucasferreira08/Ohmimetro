
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(Ohmimetro C CXX ASM)

pico_sdk_init()

add_executable(${PROJECT_NAME} 
        Ohmimetro.c
        libs/display/ssd1306.c
        libs/cod_cores/cod_cores.c
        libs/init_config/init_config.c)

pico_set_program_name(${PROJECT_NAME} "Ohmimetro")
pico_set_program_version(${PROJECT_NAME} "0.1")

pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        hardware_i2c
        hardware_adc)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include/display
        ${CMAKE_CURRENT_LIST_DIR}/include/cod_cores
        ${CMAKE_CURRENT_LIST_DIR}/include/init_config)

pico_add_extra_outputs(${PROJECT_NAME})

