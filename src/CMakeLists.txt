cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME pocoHomeBoard)

project(${PROJECT_NAME})



#include_directories(nanoMODBUS)
#add_subdirectory(nanoMODBUS)
add_subdirectory(jolib)

#find_package(SimpleITK)
add_executable ( ${PROJECT_NAME} main.cpp )
target_link_libraries ( ${PROJECT_NAME} jolib )