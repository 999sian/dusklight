cmake_minimum_required(VERSION 3.25)
project(interpolation_storage_tests LANGUAGES CXX)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "Run this isolated engine-storage test on Linux; the UWP job builds the full Windows engine.")
endif()
set(CMAKE_CXX_STANDARD 20)
include(../../cmake/GameABIConfig.cmake)
include(FetchContent)
# Exercise the Xbox dependency's allocation semantics, even on hosts with newer Abseil.
set(ABSL_PROPAGATE_CXX_STD ON)
FetchContent_Declare(abseil-cpp
    URL https://github.com/abseil/abseil-cpp/archive/refs/tags/20240722.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE)
FetchContent_MakeAvailable(abseil-cpp)
find_package(fmt CONFIG QUIET)
if(NOT TARGET fmt::fmt)
    FetchContent_Declare(fmt
        URL https://github.com/fmtlib/fmt/archive/refs/tags/12.1.0.tar.gz
        URL_HASH SHA256=ea7de4299689e12b6dddd392f9896f08fb0777ac7168897a244a6d6085043fea
        DOWNLOAD_EXTRACT_TIMESTAMP FALSE)
    FetchContent_MakeAvailable(fmt)
endif()
enable_testing()
add_executable(interpolation_storage_test interpolation_storage_test.cpp)
target_link_libraries(interpolation_storage_test PRIVATE dusklight_game_headers absl::flat_hash_map fmt::fmt)
# Eliminate unused engine paths; this test intentionally links no renderer/game runtime.
target_compile_options(interpolation_storage_test PRIVATE
    -O2 -UNDEBUG -Wno-multichar -Wno-deprecated-declarations -Wno-invalid-offsetof
    -ffunction-sections -fdata-sections)
target_link_options(interpolation_storage_test PRIVATE -Wl,--gc-sections)
add_test(NAME interpolation_storage COMMAND interpolation_storage_test)
