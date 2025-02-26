cmake_minimum_required(VERSION 3.16)

if(CMAKE_CXX_COMPILER MATCHES "arm")
    message("CONFIG FOR RASPERRY PICO")
    set(PICO_BOARD "pico_w")
    include(pico_sdk_import.cmake)
    pico_sdk_init()

    #set(SOURCE_FILES  main_test.cpp)
    if (PICO_CYW43_SUPPORTED) 
        set(SOURCE_FILES  main_wifi.cpp mqtt_publisher.cpp)
        message("WIFI ACTIVATED - main_wifi.cpp ")
    else()
        set(SOURCE_FILES  main.cpp)
        message("WIFI DEACTIVATED - main.cpp")
    endif()

    set(SOURCE_FILES ${SOURCE_FILES} serial_pico.cpp gpio_pico.cpp mcp23017.cpp)
else()
    message("CONFIG FOR LINUX")
    set(SOURCE_FILES ${SOURCE_FILES} main_linux.cpp)
endif()

set(PROJECT_NAME picoHomeBoard)

project(${PROJECT_NAME} C CXX ASM)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(TARGET_LIBS jolib)


set(SOURCE_FILES ${SOURCE_FILES} zaluzie.cpp topeni.cpp homeBoard.cpp ${CMAKE_CURRENT_BINARY_DIR}/srcVersion.cpp )

add_executable ( ${PROJECT_NAME} ${SOURCE_FILES})


if(CMAKE_SYSTEM_PROCESSOR STREQUAL "cortex-m0plus")
    

    pico_enable_stdio_usb(${PROJECT_NAME} 1)
    pico_enable_stdio_uart(${PROJECT_NAME} 0)
    pico_add_extra_outputs(${PROJECT_NAME})
    #target_link_libraries(${PROJECT_NAME} pico_stdlib jolib)
    set(TARGET_LIBS ${TARGET_LIBS} jopicolib pico_stdlib hardware_i2c hardware_uart )

    add_custom_target(pouDeploy_${PROJECT_NAME}
                DEPENDS ${PROJECT_NAME}
                COMMAND ./flash.sh ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.uf2
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
else()
    #target_link_libraries ( ${PROJECT_NAME} jolib linuxSimulator)
    set(TARGET_LIBS ${TARGET_LIBS} linuxSimulator)
    add_subdirectory(tests)
endif()


add_subdirectory(jolib)

target_link_libraries ( ${PROJECT_NAME} ${TARGET_LIBS})





ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/srcVersion.cpp
           ${CMAKE_CURRENT_BINARY_DIR}/_srcVersion.cpp #dummy target - aby se CMD provedl vzdy
    COMMAND ${CMAKE_COMMAND} -P
            ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake)

add_custom_target(run ALL #aby se CUSTOM CMD zavolal poro vsechny targety
    DEPENDS srcVersion.cpp)
