file(READ "${SOURCE_DIR}/cmake/capstone.cmake.in" _content)

# Insert PATCH_COMMAND before CONFIGURE_COMMAND in the ExternalProject_Add.
# Bracket args prevent cmake from substituting ${...} while writing this file.
if (NOT _content MATCHES "CAPSTONE_FIX_SCRIPT")
    string(REPLACE
        "    CONFIGURE_COMMAND \"\""
        [=[    PATCH_COMMAND "${CMAKE_COMMAND}" -DDIR=${CMAKE_CURRENT_BINARY_DIR}/capstone-src -P "${CAPSTONE_FIX_SCRIPT}"
    CONFIGURE_COMMAND ""]=]
        _content "${_content}")
    file(WRITE "${SOURCE_DIR}/cmake/capstone.cmake.in" "${_content}")
endif ()

file(READ "${SOURCE_DIR}/src/funchook.c" _content)
if (NOT _content MATCHES "commit_code_patch")
    string(REPLACE "#include \"funchook_internal.h\""
        "#include \"funchook_internal.h\"\n#ifdef __APPLE__\n#include \"code_patch_macos.hpp\"\n#endif"
        _content "${_content}")

    foreach(_operation install uninstall)
        if (_operation STREQUAL "install")
            set(_expected old_code)
            set(_replacement new_code)
        else ()
            set(_expected new_code)
            set(_replacement old_code)
        endif ()
        set(_original "            mem_state_t mstate;
            int rv = funchook_unprotect_begin(funchook, &mstate, entry->target_func, JUMP32_BYTE_SIZE);

            if (rv != 0) {
                return rv;
            }
            memcpy(entry->target_func, entry->${_replacement}, JUMP32_BYTE_SIZE);
            rv = funchook_unprotect_end(funchook, &mstate);
            if (rv != 0) {
                return rv;
            }
            flush_instruction_cache(entry->target_func, JUMP32_BYTE_SIZE);")
        string(FIND "${_content}" "${_original}" _position)
        if (_position EQUAL -1)
            message(FATAL_ERROR "Funchook ${_operation} patch site changed")
        endif ()
        string(REPLACE "${_original}" "#ifdef __APPLE__
            int rv = commit_code_patch(entry->target_func, entry->${_expected},
                                            entry->${_replacement}, JUMP32_BYTE_SIZE);
            if (rv != 0) {
                funchook_set_error_message(funchook, \"Code patch commit failed (Mach error %d)\", rv);
                return FUNCHOOK_ERROR_MEMORY_FUNCTION;
            }
#else
${_original}
#endif" _content "${_content}")
    endforeach ()
    file(WRITE "${SOURCE_DIR}/src/funchook.c" "${_content}")
endif ()

# UWP: only the *FromApp memory APIs are allowed and they reject RWX pages (the codeGeneration
# capability permits RW <-> RX flips). GetModuleHandleEx and psapi are desktop-only.
file(READ "${SOURCE_DIR}/src/funchook_windows.c" _content)
if (NOT _content MATCHES "FUNCHOOK_UWP")
    set(_uwp_prelude [=[#include "funchook_internal.h"
#if defined(WINAPI_FAMILY) && WINAPI_FAMILY == WINAPI_FAMILY_APP
#define FUNCHOOK_UWP 1
#define VirtualAlloc(addr, size, type, prot) VirtualAllocFromApp(addr, size, type, prot)
#define VirtualProtect(addr, size, prot, old) VirtualProtectFromApp(addr, size, prot, old)
#define GetMappedFileNameA(process, addr, buf, size) 0
/* ponytail: target pages go RW (not RWX) while the jump is written, so a thread executing
   that page at that instant faults. Hooks install during mod load; an RX alias via
   section mapping would close the window if it ever bites. */
#define FUNCHOOK_PATCH_PROTECTION PAGE_READWRITE
#else
#define FUNCHOOK_PATCH_PROTECTION PAGE_EXECUTE_READWRITE
#endif]=])
    set(_module_original "    ok = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, func, &hMod);")
    set(_module_replacement [=[#ifdef FUNCHOOK_UWP
    {
        MEMORY_BASIC_INFORMATION mbi;
        ok = VirtualQuery(func, &mbi, sizeof(mbi)) != 0 && mbi.Type == MEM_IMAGE;
        hMod = (HMODULE)mbi.AllocationBase;
    }
#else
    ok = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, func, &hMod);
#endif]=])
    foreach (_site "#include \"funchook_internal.h\"" "PAGE_EXECUTE_READWRITE, &mstate->protect" "${_module_original}")
        string(FIND "${_content}" "${_site}" _position)
        if (_position EQUAL -1)
            message(FATAL_ERROR "Funchook UWP patch site changed: ${_site}")
        endif ()
    endforeach ()
    string(REPLACE "#include \"funchook_internal.h\"" "${_uwp_prelude}" _content "${_content}")
    string(REPLACE "PAGE_EXECUTE_READWRITE, &mstate->protect" "FUNCHOOK_PATCH_PROTECTION, &mstate->protect"
        _content "${_content}")
    string(REPLACE "${_module_original}" "${_module_replacement}" _content "${_content}")
    file(WRITE "${SOURCE_DIR}/src/funchook_windows.c" "${_content}")
endif ()
