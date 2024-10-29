cmake_minimum_required(VERSION 3.28)
project(Vector)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wshadow -g -fsanitize=address,bounds,undefined")
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(VectorTests tests/tests.cpp)

target_link_libraries(VectorTests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(VectorTests)