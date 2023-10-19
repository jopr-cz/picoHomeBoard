cmake_minimum_required(VERSION 3.16)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "cortex-m0plus")
    include(pico_sdk_import.cmake)
endif()

set(PROJECT_NAME pocoHomeBoard)

project(${PROJECT_NAME} C CXX ASM)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(TARGET_LIBS jolib)



add_subdirectory(jolib)
add_executable ( ${PROJECT_NAME} main.cpp zaluzie.cpp topeni.cpp)



if(CMAKE_SYSTEM_PROCESSOR STREQUAL "cortex-m0plus")
    message("CONFIG FOR RASPERRY PICO")
    pico_sdk_init()

    pico_enable_stdio_usb(${PROJECT_NAME} 1)
    pico_enable_stdio_uart(${PROJECT_NAME} 1)
    pico_add_extra_outputs(${PROJECT_NAME})
    #target_link_libraries(${PROJECT_NAME} pico_stdlib jolib)
    set(TARGET_LIBS ${TARGET_LIBS} pico_stdlib)
else()
    #target_link_libraries ( ${PROJECT_NAME} jolib linuxSimulator)
    set(TARGET_LIBS ${TARGET_LIBS} linuxSimulator)
endif()

message("Linked libsXX: ${TARGET_LIBS}")

target_link_libraries ( ${PROJECT_NAME} ${TARGET_LIBS})