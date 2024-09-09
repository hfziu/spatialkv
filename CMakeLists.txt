# Some dependencies don't have a CMake config file, so we need to specify the paths manually
# Modify these paths to match your system setup
set(CUSTOM_INLCUDE_DIR "/opt/homebrew/include")
set(CUSTOM_LIB_DIR "/opt/homebrew/lib")

cmake_minimum_required(VERSION 3.12)
project(SpatialKV)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find LevelDB
# find_package(LevelDB REQUIRED)
link_directories(${CUSTOM_LIB_DIR})

add_library(spatialkv
  "util/format.cc"
)

target_include_directories(spatialkv
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CUSTOM_INCLUDE_DIR}
)

target_link_libraries(spatialkv leveldb s2)

# > Testing
# =========
enable_testing()
# Find GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Define the test function
function(spatialkv_test test_file)
  get_filename_component(test_target ${test_file} NAME_WE)
  add_executable(${test_target} ${test_file})
  target_link_libraries(${test_target} spatialkv GTest::Main)
  add_test(NAME ${test_target} COMMAND ${test_target})
endfunction()

# Add tests
spatialkv_test("util/leveldb_test.cc")
spatialkv_test("util/s2_test.cc")

