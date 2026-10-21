# Xbox One S logging and shader-cache checks

Checked 21 October 2026. The working installed baseline is package 2.0.3.37;
new logging, bundled-cache and mod builds still need console verification.

## Buffered logging

Commit `458ffe7` removes the temporary UWP `flushOn = Debug` override in
`src/dusk/logging.cpp`. Debug messages remain enabled. Borealis defaults to
`flushOn = Warning`, so ordinary debug/info messages use the file buffer while
warnings and errors immediately flush it. Explicit logger flushes and file close
still flush buffered data. A forced termination can lose the final buffered
low-severity messages; Device Portal crash capture remains available.

This is a deletion of diagnostic behavior, so no extra logging option or new
logging implementation was added. The existing Borealis `Log.ExtraSink` test
covers flushing at the default threshold. That test was not run in this review; the new Xbox package still needs
verification. No CPU/FPS improvement is claimed yet.

## Persistent caches already supported

The game passes `dataPaths.cachePath` to Aurora. Borealis sets that path to the
application preference directory, independent of the package version and of a
custom save/data folder. On this Xbox it resolves to:

`U:\Users\UserMgr0\AppData\Local\Packages\Dusklight_5pxj2cgsb63vw\LocalState\TwilitRealm\Dusklight`

- `pipeline_cache.db` stores backend-neutral pipeline configurations. Startup
  reads compatible configuration versions and queues them for background
  warmup, prioritizing their recorded first-use order.
- `dawn_cache.db` stores Dawn's compiled cache entries. The renderer's load and
  store callbacks read/write this database, so compatible entries can be reused
  on later launches.
- Both use SQLite WAL mode. The `-wal` file can contain committed data absent
  from the main database. A consistent backup needs the stopped app and its
  database/WAL files; file size alone does not measure cache reuse.

No additional cache manager is needed. Keep the app identity/publisher and
upgrade in place; do not uninstall or delete the caches. Renderer/schema,
pipeline-configuration or driver changes can legitimately invalidate entries.
Existing code also prunes stale entries after warmup, so stable row counts or
identical file hashes are not a requirement for successful reuse.

## Read-only console evidence

With Dusklight stopped, downloaded the two databases plus WAL/SHM companions.
Local SQLite connections opened the snapshots in read-only mode and returned:

| Database | Schema | Rows | `PRAGMA quick_check` |
| --- | --- | ---: | --- |
| `dawn_cache.db` | 2 | 818 | `ok` |
| `pipeline_cache.db` | 1 | 306 | `ok` |

Pipeline records: 1 Clear configuration at version 5, 294 GX configurations at
version 13 and 11 RML configurations at version 3. This proves that valid cache
records have persisted on the console; it does not establish a cache hit rate
or a measured frame-time improvement.

Snapshot and SHA-256 manifest:
`/home/sian/Downloads/dusklight-xbox-diagnostics/cache-before-optimization-20261021-134109`.
No console files, settings, saves or ISO were changed for these checks.

## Bundled pipeline list

CI stages upstream v2.0.3's hash-pinned `initial_pipeline_cache.db`. A separate
read-only check of that exact release file verified its required SHA-256,
SQLite schema 1 and `quick_check = ok`. It contains 2,695 records: 2,678 GX
configurations at version 13 and 17 RML configurations at version 3, matching
the configuration versions in the console's current cache. Its size is
11,104,256 bytes. Aurora
validates its schema and row shape, then merges it into the local pipeline
cache. Upserts retain the earlier first-used frame for matching keys. The seed
is a list of configurations, not compiled Xbox driver shaders: first-run GPU
compilation is still required. The list is currently re-read and upserted on
each process start; changing that should wait for evidence of startup cost.

## Remaining hardware checks

1. Inspect the successful MSIX for the expected seed and exact package identity.
2. Back up settings/saves, install in place, and verify the exact new version.
3. Confirm the seed-merge log and absence of cache/database errors. Leave caches
   intact; allow the background queue to finish before judging startup reuse.
4. Stop and relaunch the same package with the same settings and scene. Compare
   shader-compilation behavior and frame times after warmup. Changes in the
   cache files alone are insufficient evidence of hits or improved CPU use.
5. Confirm warnings/errors still appear promptly in logs. A clean shutdown
   should retain the final buffered debug/info lines.

The earlier menu/intro FPS screenshots were isolated observations. Device
Portal returned zero CPU even while rendering, so those samples cannot be used
to quantify this optimization. Controller input, audio output, saves and
sustained gameplay also remain outside these read-only checks.
