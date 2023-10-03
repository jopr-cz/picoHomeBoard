cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME pocoHomeBoard)

project(${PROJECT_NAME})



include_directories(nanoMODBUS)
add_subdirectory(nanoMODBUS)

#find_package(SimpleITK)
add_executable ( ${PROJECT_NAME} main.cpp nanoMODBUS/nanomodbus.h )
#target_link_libraries ( sitk_example ${SimpleITK_LIBRARIES} )