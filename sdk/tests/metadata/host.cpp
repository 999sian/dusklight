#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <mods/svc/log.h>
#undef NDEBUG
#include <cassert>
#include <cstring>
#include <cstdio>

int wmain(int argc, wchar_t** argv) {
    assert(argc == 2);
    const auto module = LoadLibraryW(argv[1]);
    assert(module);
    const auto* meta = reinterpret_cast<const ModMeta*>(GetProcAddress(module, "mod_meta"));
    assert(meta && meta->struct_size == sizeof(ModMeta));
    auto* cursor = static_cast<const unsigned char*>(meta->records_begin);
    const auto* end = static_cast<const unsigned char*>(meta->records_end);
    assert(cursor && end && cursor < end);
    unsigned headers = 0, imports = 0, exports = 0;
    LogService sentinel{};
    while (cursor < end) {
        assert(end - cursor >= 8);
        uint64_t first = 0;
        std::memcpy(&first, cursor, sizeof(first));
        if (!first) { cursor += 8; continue; }
        const auto* rec = reinterpret_cast<const ModMetaRecord*>(cursor);
        assert(rec->size >= 8 && rec->size % 8 == 0 && rec->size <= end - cursor);
        if (rec->kind == MOD_META_HEADER) {
            assert(reinterpret_cast<const ModMetaHeader*>(rec)->abi_version == MOD_ABI_VERSION);
            ++headers;
        } else if (rec->kind == MOD_META_IMPORT) {
            const auto* import = reinterpret_cast<const ModMetaImport*>(rec);
            assert(std::strcmp(import->service_id.chars, LOG_SERVICE_ID) == 0);
            *static_cast<const LogService**>(import->slot) = &sentinel;
            ++imports;
        } else if (rec->kind == MOD_META_EXPORT) {
            const auto* record = reinterpret_cast<const ModMetaExport*>(rec);
            assert(std::strcmp(record->service_id.chars, "com.test.metadata") == 0);
            assert(record->rec.flags == SERVICE_EXPORT_DEFERRED);
            ++exports;
        }
        cursor += rec->size;
    }
    std::printf("Metadata: %u header, %u import, %u export\n", headers, imports, exports);
    assert(headers == 1 && imports == 1 && exports == 1);
    const auto imported = reinterpret_cast<const LogService*(*)()>(GetProcAddress(module, "imported_log"));
    assert(imported && imported() == &sentinel);
    FreeLibrary(module);
}
