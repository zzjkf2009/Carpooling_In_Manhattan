cmake_minimum_required(VERSION 3.2.1)
project (carpooling)

add_compile_options(-std=c++11)
find_package(jsoncpp REQUIRED)
#find_package(json REQUIRED)
# Set the build type to be a release build
set(CMAKE_BUILD_TYPE Release)

# Add project cmake modules to path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

include(CMakeToolsHelpers OPTIONAL)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(vendor/googletest/googletest)
