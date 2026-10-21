include(${CMAKE_CURRENT_LIST_DIR}/cmake/AssetDeploy.cmake)

set_target_properties(dusklight PROPERTIES OUTPUT_NAME dusklight-uwp)
target_compile_definitions(dusklight PRIVATE AURORA_WINDOWS_STORE=1)
target_link_libraries(dusklight PRIVATE aurora::uwp_dep WindowsApp OneCore)
# nod.lib carries static CRT directives; UWP uses the DLL CRT throughout.
target_link_options(dusklight PRIVATE /NODEFAULTLIB:libcmt /NODEFAULTLIB:libcmtd)

set(_uwp_manifest "${CMAKE_CURRENT_LIST_DIR}/Package.appxmanifest")
file(GLOB _uwp_dlls "${AURORA_UWP_DEP_ROOT}/x64/bin/*.dll")
target_sources(dusklight PRIVATE ${_uwp_manifest} ${_uwp_dlls})
set_source_files_properties(${_uwp_dlls} PROPERTIES
    VS_DEPLOYMENT_CONTENT TRUE VS_DEPLOYMENT_LOCATION "." VS_COPY_TO_OUT_DIR Always)
uwp_pkg(dusklight "${CMAKE_CURRENT_LIST_DIR}/Assets" "Assets")
uwp_pkg(dusklight "${CMAKE_SOURCE_DIR}/res" "res")
