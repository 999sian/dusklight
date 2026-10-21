include(${CMAKE_CURRENT_LIST_DIR}/cmake/AssetDeploy.cmake)

target_compile_definitions(dusklight PRIVATE AURORA_WINDOWS_STORE=1)
target_link_libraries(dusklight PRIVATE aurora::uwp_dep WindowsApp OneCore)
# nod.lib carries static CRT directives; UWP uses the DLL CRT throughout.
target_link_options(dusklight PRIVATE /NODEFAULTLIB:libcmt /NODEFAULTLIB:libcmtd)

set(_uwp_manifest "${CMAKE_CURRENT_LIST_DIR}/Package.appxmanifest")
file(GLOB _uwp_dlls "${AURORA_UWP_DEP_ROOT}/x64/bin/*.dll")
target_sources(dusklight PRIVATE ${_uwp_manifest} ${_uwp_dlls})
set_source_files_properties(${_uwp_dlls} PROPERTIES
    VS_DEPLOYMENT_CONTENT TRUE VS_DEPLOYMENT_LOCATION "." VS_COPY_TO_OUT_DIR Always)
file(GLOB _uwp_assets "${CMAKE_CURRENT_LIST_DIR}/Assets/*.png")
target_sources(dusklight PRIVATE ${_uwp_assets})
# Image items must enter the PRI index so unqualified manifest paths resolve.
set_source_files_properties(${_uwp_assets} PROPERTIES
    VS_TOOL_OVERRIDE Image VS_DEPLOYMENT_CONTENT TRUE VS_DEPLOYMENT_LOCATION "Assets")
uwp_pkg(dusklight "${CMAKE_SOURCE_DIR}/res" "res")

# CI-staged files added to the package root with their relative paths kept:
# initial_pipeline_cache.db and pre-extracted bundled mods (mods/<id>/...).
set(DUSK_UWP_EXTRA_CONTENT_DIR "" CACHE PATH "Directory whose tree is added to the package root")
if (DUSK_UWP_EXTRA_CONTENT_DIR)
    file(GLOB_RECURSE _uwp_extra RELATIVE "${DUSK_UWP_EXTRA_CONTENT_DIR}" "${DUSK_UWP_EXTRA_CONTENT_DIR}/*")
    foreach (_file IN LISTS _uwp_extra)
        get_filename_component(_dir "${_file}" DIRECTORY)
        if (_dir STREQUAL "")
            set(_dir ".")
        endif ()
        set(_src "${DUSK_UWP_EXTRA_CONTENT_DIR}/${_file}")
        set_source_files_properties("${_src}" PROPERTIES
            VS_DEPLOYMENT_CONTENT TRUE VS_DEPLOYMENT_LOCATION "${_dir}" VS_COPY_TO_OUT_DIR Always)
        target_sources(dusklight PRIVATE "${_src}")
    endforeach ()
endif ()
