#include <mods/svc/log.h>
DEFINE_MOD();
IMPORT_SERVICE(LogService, svc_log);
EXPORT_DEFERRED_SERVICE(probe, "com.test.metadata", 1, 0);
extern "C" MOD_EXPORT const LogService* imported_log() { return svc_log; }
