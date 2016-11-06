cmake_minimum_required(VERSION 2.8.0)
project(Streams)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /W4")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++14 -g -O0 --coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic") 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++ -Woverloaded-virtual -Wctor-dtor-privacy")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wold-style-cast -Werror -Wconversion")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion -Winit-self -Wunreachable-code")
endif()

add_executable(example_mult_of_17 examples/mult_of_17.cpp)

add_executable(general_tests tests/general.cpp)
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

target_link_libraries(general_tests ${GTEST_BOTH_LIBRARIES})

add_test(AllTests general_tests)
