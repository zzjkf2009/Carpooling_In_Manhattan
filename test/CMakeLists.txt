set(GTEST_SHUFFLE 1)

add_executable(
    cpp-test
    main.cpp
   carpooling_cloud_server_test.cpp
   ../src/carpooling_cloud_server.cpp
  ../src/car.cpp
)
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${gmock_SOURCE_DIR}/include
)


target_include_directories(cpp-test PUBLIC ../vendor/googletest/googletest/include
                                           ../vendor/googletest/googlemock/include
                                           ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(cpp-test PUBLIC gtest jsoncpp)
#target_link_libraries(cpp-test PUBLIC  gmock_main ${OpenCV_LIBS})
