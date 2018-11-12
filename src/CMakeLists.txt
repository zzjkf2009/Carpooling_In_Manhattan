# Bring the header files into the library build environment.
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${JSONCPP_INCLUDE_DIRS}
)
#pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

# Create a executable file which is a user interface that can let user type their own matrix and doing calculations
add_executable(carpooling main.cpp car.cpp carpooling_cloud_server.cpp)
# Link the created library to the executable
target_link_libraries (carpooling ${JSONCPP_LIBRARIES} jsoncpp)

