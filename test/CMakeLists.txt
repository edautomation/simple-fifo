cmake_minimum_required(VERSION 3.14)

project(tests VERSION 1.0)


add_executable(tests test_fifo.cpp)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
include_directories(${PARENT_DIR})
target_sources(tests PRIVATE ${PARENT_DIR}/byte_fifo.c)



# Google Test
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
target_link_libraries(tests gtest_main)
gtest_add_tests(TARGET tests)


