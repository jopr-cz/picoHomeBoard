cmake_minimum_required(VERSION 3.16)

if(CMAKE_CXX_COMPILER MATCHES "arm")
    message("CONFIG FOR RASPERRY PICO")
    include(pico_sdk_import.cmake)
    set(SOURCE_FILES  main.cpp)
else()
    message("CONFIG FOR LINUX")
    set(SOURCE_FILES ${SOURCE_FILES} main_linux.cpp)
endif()

set(PROJECT_NAME pocoHomeBoard)

project(${PROJECT_NAME} C CXX ASM)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(TARGET_LIBS jolib)



add_subdirectory(jolib)


set(SOURCE_FILES ${SOURCE_FILES} zaluzie.cpp topeni.cpp homeBoard.cpp )

add_executable ( ${PROJECT_NAME} ${SOURCE_FILES})

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "cortex-m0plus")
    pico_sdk_init()

    pico_enable_stdio_usb(${PROJECT_NAME} 1)
    pico_enable_stdio_uart(${PROJECT_NAME} 1)
    pico_add_extra_outputs(${PROJECT_NAME})
    #target_link_libraries(${PROJECT_NAME} pico_stdlib jolib)
    set(TARGET_LIBS ${TARGET_LIBS} pico_stdlib)
else()
    #target_link_libraries ( ${PROJECT_NAME} jolib linuxSimulator)
    set(TARGET_LIBS ${TARGET_LIBS} linuxSimulator)
    add_subdirectory(tests)
endif()



target_link_libraries ( ${PROJECT_NAME} ${TARGET_LIBS})