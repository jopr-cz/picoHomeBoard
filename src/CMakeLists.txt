cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME pocoHomeBoard)

project(${PROJECT_NAME})




add_subdirectory(nanoMODBUS)

#find_package(SimpleITK)
add_executable ( ${PROJECT_NAME} main.cpp )
#target_link_libraries ( sitk_example ${SimpleITK_LIBRARIES} )