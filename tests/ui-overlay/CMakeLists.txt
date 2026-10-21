cmake_minimum_required(VERSION 3.25)
project(DusklightOverlayTest LANGUAGES CXX)

include(FetchContent)
set(RMLUI_FONT_ENGINE none CACHE STRING "" FORCE)
set(RMLUI_SAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(rmlui
    URL https://github.com/encounter/RmlUi/archive/f00a0fee38391b2f927114e11bea18dc0a7dba1e.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE
    EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(rmlui)
add_executable(overlay_test overlay_test.cpp)
target_compile_features(overlay_test PRIVATE cxx_std_20)
target_link_libraries(overlay_test PRIVATE RmlUi::Core)
enable_testing()
add_test(NAME overlay_unchanged_updates COMMAND overlay_test)
