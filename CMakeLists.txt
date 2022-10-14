cmake_minimum_required(VERSION 3.21)
project(dtl)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
INCLUDE_DIRECTORIES(lib)
add_executable(dtl main.cpp lib/request_handler.cpp lib/reply.cpp lib/request_parser.cpp lib/mine_types.cpp lib/util.cpp)

add_executable(
        hello_test
        tests/thread_test.cpp
        lib/request_handler.cpp lib/reply.cpp lib/request_parser.cpp lib/mine_types.cpp lib/util.cpp
)
target_link_libraries(
        hello_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)
target_link_libraries(
        hello_test
        GTest::gtest_main
)

