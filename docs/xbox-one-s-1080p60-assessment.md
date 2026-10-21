# Xbox One S: assessment of a 1080p / 60 FPS target

Assessed 21–22 October 2026 against branch `xbox-one-s-uwp-v2.0.3`, installed
package 2.0.3.49, game source `9331db1`. This assessment is a source/configuration
review; it introduces no performance code, build or gameplay measurement.
Separate texture-installation startup checks are recorded in the progress doc.

## Feasibility and what the target means

**1080p at 60 presented frames per second is a target to measure, not a result
established by current evidence.** The game deliberately simulates at **30 Hz**:
`src/dusk/game_clock.h:6`. Its existing interpolation path separates simulation
from presentation (`src/dusk/game_clock.cpp:88`,
`src/m_Do/m_Do_main.cpp:285`). At 60 FPS it draws intermediate frames while
retaining the original simulation timing. This is object/camera interpolation,
not a claim of 60 Hz simulation. `set_sim_rate(60)` changes the clock scale and
would speed gameplay up; it is not a safe 60 FPS optimization.

The display target and the internal rendering resolution must also be recorded
separately. `game.internalResolutionScale=0` (Auto) follows the window's pixel
size; positive integers scale the original configured framebuffer and adapt its
aspect (`extern/aurora/lib/window.cpp:446`, `:87`). An integer setting of 2 is
not inherently 1080p. A lower internal resolution upscaled to a 1080p display is
an alternative with reduced image detail, not native 1080p rendering. Likewise,
a 3840×2160 Device Portal screenshot does not establish the game's render size.

The GPU workload must fit a **16.67 ms** presentation interval; CPU simulation
and draw submission on simulation frames must fit the schedule as well. A
33.33 ms simulation period does not permit every other presentation to stall
for 33 ms. Doubling presentation frequency can increase draw traversal,
interpolation and driver work even though simulation remains at 30 Hz.

## Evidence available

The user reports slowdown during **normal gameplay**, not only during startup
shader compilation. No numeric gameplay FPS or reproducible scene was supplied;
this prioritizes warmed gameplay profiling without establishing a CPU bottleneck.

- `v49-after-tpde-rollback.log` identifies D3D12 on the console's SraKmd GPU,
  BC texture support, reported memory of 5,120 MiB and an initial 1920×1080
  framebuffer. Verify actual scene render dimensions after startup/settings
  application before using the initial framebuffer line as a gameplay claim.
- Bounded startup checks reached the menu with 4/4 mods active, including Dark
  Link. A menu screenshot around 59 FPS and a running process do not establish
  gameplay performance, input responsiveness, audio, or worst-case frame times.
- The confirmed current backup is
  `/home/sian/Downloads/dusklight-xbox-diagnostics/before-v2-20261021-232204/config.json`:

  | Setting | Saved value | Meaning |
  | --- | --- | --- |
  | `game.internalResolutionScale` | `0` | Auto |
  | `game.enableFrameInterpolation` | `1` | Capped interpolation |
  | `video.maxFrameRate` | `60` | Target presentation cap |
  | `video.enableVsync` | `false` | Vsync disabled |
  | `game.shadowResolutionMultiplier` | `1` | Lowest existing shadow multiplier |
  | `game.bloomMode` | `2` | Dusklight bloom |
  | `game.enableDepthOfField` | `false` | Legacy key; not registered by current code |
  | `game.depthOfFieldMode` | absent | Current code defaults to Dusklight (`2`) |

- The old `game.enableDepthOfField` key has no current source consumer. Current
  registration/default is `src/dusk/settings.cpp:79`, `:315`; actual drawing
  checks `game.depthOfFieldMode` in `src/m_Do/m_Do_graphic.cpp:1171`. This is a
  concrete mismatch, not proof of the size of any performance penalty.
- Buffered logging and persistent shader caches are already implemented. See
  `xbox-one-s-optimization.md`; neither cache persistence nor removing the
  debug-line flush has a measured FPS improvement yet.
- The four TPDE+ archives are verified locally. Selected content is 4,788
  textures / 7,764,942,214 bytes. The full transfer to D: passed remote filename/
  size checks and three sampled SHA256 checks; use the progress document for
  activation/startup status. This does not measure pack cost during gameplay.

## Prioritized work, smallest first

| Priority | Action | Evidence / code path | Benefit to test and tradeoff |
| --- | --- | --- | --- |
| 1 | Measure a repeatable gameplay baseline with the existing 60 cap; no new performance code first | Existing cap in `src/m_Do/m_Do_main.cpp:346`; frame preference in `src/dusk/presentation.cpp:17` | Establish CPU/GPU/streaming bottleneck and correctness; no expected percentage gain asserted |
| 2 | Test the current DoF key set to Off, then bloom Off, one change at a time | `game.depthOfFieldMode=0`, `game.bloomMode=0`; defaults in `src/dusk/settings.cpp:77` | Remove optional post-processing work; loses DoF/bloom visual effects. Both have existing UI controls, so no new settings code is needed |
| 3 | Compare Vsync on/off at the same capped 60 and display refresh | `src/dusk/settings.cpp:16`, `src/dusk/presentation.cpp:17` | Choose stable pacing/tearing/latency behavior from measurements; enabling Vsync does not create performance headroom |
| 4 | Keep shadows at 1 and verify actual 1920×1080 rendering; test lower internal rendering only as a named fallback | `src/dusk/settings.cpp:83`, `:84`; `extern/aurora/lib/window.cpp:446` | Avoid accidental oversampling. Lower resolution helps only GPU-bound work; CPU-bound frames may barely change |
| 5 | Measure pack cost separately from the base game and Dark Link | Streaming/cache paths below | Large textures can worsen upload spikes, RAM pressure and storage traffic. A console-sized derivative needs separate author/license review and byte/visual validation; originals must be retained |
| 6 | If memory/streaming measurements justify it, give UWP a smaller replacement-cache budget and/or cap texture resolution | `extern/aurora/lib/gfx/texture_replacement.cpp:45`, `:1036` | Current 4 GiB budget is large for this console. Start an A/B experiment at 512 MiB or 1 GiB only after measuring scene working set; smaller LRU may increase eviction/reload stutter. Not a validated preset |
| 7 | If CPU traces show contention, test fewer background texture workers or a lower-priority warmup worker | Texture workers `texture_replacement.cpp:842`; pipeline worker `pipeline_cache.cpp:958` | Current texture pool chooses up to 4 low-priority workers, while compilation and rendering also run. Fewer workers may reduce frame-time spikes but delay texture/shader readiness |
| 8 | Profile specific hot functions before selective CPU/compiler tuning | Existing Tracy zones in the main loop; optimized CI configuration | PGO or narrowly improving measured hot paths may help. Broad LTO/inlining/export changes risk native-mod hooks and cannot be justified by a generic FPS target |

The Dusk first-run preset is not a suitable Xbox performance shortcut:
`src/dusk/ui/preset.cpp:47` chooses Unlimited interpolation and `:52` chooses
4× shadows. Changing to that preset also changes gameplay preferences; avoid
using it as a blanket optimization. The current saved configuration already
has capped 60 and 1× shadows.

### Texture memory and upload risks

Aurora already supports asynchronous texture replacements, 64-pixel temporary
thumbnails and a 12 MiB per-frame publication budget
(`extern/aurora/lib/gx/texture.hpp:27`). A separate original-content cache is
128 MiB; the replacement LRU is **4 GiB**, not the pack's total on-disk size.
Neither number includes all render targets, decoded pending jobs, meshes,
shader/driver allocations, mods, game heaps or audio. Main startup config alone
requests 256 MiB MEM1 plus 24 MiB MEM2 (`src/m_Do/m_Do_main.cpp:705`). The log's
5,120 MiB reported memory is not a measured safe texture allowance or a query
of every current OS allocation limit.

The selected pack contains 8192×4096 BC7 DDS textures around **44.7 MB each**
including mip levels. BC support means they need not all be expanded to RGBA,
but large uploads still matter. Crucially,
`texture_replacement.cpp:949` intentionally permits the first texture in a
frame to exceed the 12 MiB publication budget so oversized textures can make
progress. A nominal 12 MiB budget therefore does **not** prevent a 44.7 MB
single-upload spike. Simply forbidding oversized publications would leave
those textures permanently unpublished. Consider a separately tested mip-level
cap or properly resized pack only if measured spikes show this path is costly.

More development storage helps installation capacity; it does not increase the
CPU, GPU bandwidth or usable runtime memory. Retaining full-resolution texture
support and meeting 1080p60 in all scenes may be conflicting goals on One S.

### Shader/CPU/build work to avoid duplicating

There is already a dedicated pipeline compilation thread for D3D12
(`extern/aurora/lib/gfx/pipeline_cache.cpp:1143`), prioritizing normal jobs over
background warmup (`:980`). The pinned seed describes pipeline configurations,
not portable Xbox driver binaries. Preserve compiled local caches, warm up the
same package/settings, and measure cold and warm runs separately. Do not add
more shader workers or bundle desktop compiled blobs based solely on the
compilation queue being long.

CI builds RelWithDebInfo (`.github/workflows/build.yml:118`), not an unoptimized
Debug package. Debug symbols alone do not mean slow code. `DUSK_SELECTED_OPT`
is documented for selected optimization in Debug (`CMakeLists.txt:105`), so
turning that switch on is not a demonstrated fix for the installed build.
Preserve `/OPT:NOICF`, patchable entry points, mod metadata retention and the
embedded symbol manifest; those protect existing native mod/hook support.
Do not disable checks or use `/fp:fast` globally without correctness evidence.

## CPU bottleneck: three profiling experiments

A CPU limit is plausible but **not demonstrated** by the available startup
checks. Low aggregate CPU usage can hide one saturated game/render thread:
averaging across cores dilutes its contribution, and waits are different from
useful execution. Device Portal previously reporting zero CPU while rendering
makes that particular sample unusable. Do not equate it with CPU headroom.

Existing instrumentation can locate the work before adding another profiler:

- `fapGm_Execute` has `ZoneScoped` (`src/f_ap/f_ap_game.cpp:812`), enclosing
  game/process management, mod ticks and achievements. Nested
  `fpcM_Management` (`src/f_pc/f_pc_manager.cpp:53`) includes actor execution,
  deletion/creation and draw work. These are candidates to inspect, not proven
  hot spots. `mDoAud_Execute` also has a zone (`src/m_Do/m_Do_audio.cpp:178`).
- Presentation invokes draw traversal and painting on each interpolated frame
  (`src/m_Do/m_Do_main.cpp:309`); `mDoGph_Painter` is already zoned
  (`src/m_Do/m_Do_graphic.cpp:2176`). Rendering therefore adds CPU work at
  60 presentations even with simulation still running at 30 Hz.
- Aurora has CPU frame elapsed-time/EMA tracking in
  `extern/aurora/lib/gfx/frame.cpp:662` and `:668`, plus `FrameMark` and the
  separate `Frame limiter` zone in the game loop. An elapsed CPU interval can
  include waits; neither an EMA nor limiter utilization is a per-core profile
  or a p99 frame-time result.
- Existing wait zones include `Wait for buffer map`, `Acquire frame slot` and
  `Acquire mapped staging buffer` (`gfx/frame.cpp:581`, `:596`, `:610`). The
  render worker records `QueueItem work` (`gfx/render_worker.cpp:48`) and the
  encoder has `render_pass`/`render` zones (`gfx/encoding.cpp:109`, `:189`).
  Long game-thread waits can reflect render-worker/GPU backpressure rather than
  expensive game simulation. Texture hashing/resolution also has zones
  (`gx/texture.cpp:282`, `:596`) and counters to correlate with streaming.

| Order | Experiment using the same save, route and warm caches | What would guide the next change |
| --- | --- | --- |
| 1 | Capture a capped-60 gameplay Tracy trace with game thread, render worker, waits and GPU time if available; then repeat with a lower internal resolution, keeping scene/mods/effects identical | If simulation/recording dominates with little change at lower resolution, inspect the largest traced CPU functions. If GPU time/backpressure falls, prioritize graphics. Resolution response is a clue, not proof: caps, upload work and synchronization can mask or mix both limits |
| 2 | Compare the existing interpolated path capped at 30 versus capped at 60, retaining 30 Hz simulation, resolution and content; inspect tick frames separately from intermediate presentations | Isolates the additional draw/encoding/interpolation load more closely than switching interpolation entirely off. Keep original timing. If that added work dominates, optimize the measured draw/recording hot path, not actor simulation or game speed; report visual/latency differences |
| 3 | Compare the first run/first scene encounter with a warmed repeat and same-package relaunch; correlate long frames with shader queues, texture uploads and worker activity | If stalls vanish when background work finishes, fix contention/streaming rather than assuming steady-state simulation is too slow. Only then A/B a lower-priority pipeline worker or 1–2 texture workers against the existing pool; longer warmup or visible texture delay is a real cost |

Use the existing optimized build and on-demand Tracy path; first establish
that its Xbox capture works and record instrumentation overhead. On a real CPU
hot path, the next change should remove measured redundant work or allocation,
retain caches/invalidation and preserve mod behavior. Do not parallelize actor
updates or shared game state wholesale, increase every worker count, remove
collision/animation correctness, or promise a speedup without before/after
captures. No CPU profile or CPU optimization has been performed here.

## Normal-gameplay CPU candidates: renderer and interpolation

Read-only scan on 22 October 2026. For the reported slowdown during normal
gameplay, prioritize these paths before the loading-only candidates below.
Ranking is an investigation order, not measured CPU cost.

| Order | Candidate | Repeated work visible in source | Smallest experiment |
| --- | --- | --- | --- |
| 1 | Batch FIFO publication | `extern/aurora/lib/gx/fifo.cpp:28` sets `kDrawBatchSize=1`; `finish_draw` at `:193` publishes every completed draw, updating atomics and notifying the worker (`:46`, `:172`) | Compare batches of 1, 4 and 8 with identical scenes; keep every explicit publication and drain boundary |
| 2 | Reuse matrix-recording storage | `src/dusk/interp/frame_interpolation.cpp:165` moves the current map into the previous recording, discards the old previous map and starts a fresh current map each recorded tick | Reuse two snapshot buffers with verified capacity retention, preserving exactly the same matrix keys and snapshots |
| 3 | Reuse material-capture storage | `src/dusk/interp/material.cpp:543` creates shared recording ownership and `:356` allocates binding state for each captured model; capture vectors are built again | Reuse recording/binding storage only after previous callback owners release it; measure allocation cost first |

### FIFO batching: synchronization overhead versus overlap

With threaded processing enabled (`fifo.cpp:27`), each publication calls
`wake_worker`, which increments an atomic and calls `notify_all`. A notification
is not proof of an actual OS wake/context switch. The consumer holds
`sBufferMutex` around command processing (`:56`); the producer takes that mutex
for buffer growth (`:161`), not every ordinary write. Count publications, wake
notifications, processed bytes, worker idle time and drain waits alongside
frame time. Larger batches may reduce notification overhead but also delay
consumer work and reduce producer/consumer overlap.

Preserve `drain` (`:234`), DrawDone callbacks, EFB copies, display lists, custom
draw/pass transitions and end-of-frame ordering. Test these boundaries and
compare visuals, transitions and hangs before retaining even a one-line batch
change. This is an experiment, not a justification to remove synchronization.
The command processor already merges eligible contiguous draws and caches
pipeline/bind-group/uniform state (`gx/command_processor.cpp:390`, `:491`);
measure merge hit rates before designing another draw cache.

### Matrix interpolation: allocation and hash work

`record_final_mtx` (`frame_interpolation.cpp:202`) inserts/copies each recorded
matrix. `interpolate_replacements` (`:69`) clears the replacement map, walks
the old recording, looks up matching current keys and inserts interpolated
results. These operations continue with warm shaders and resident textures.
Measure matrix counts, allocations, recording time and replacement-building
time separately on simulation and intermediate frames.

Do not assume `swap` plus `clear` retains an Abseil flat hash map's allocation;
verify the pinned container's semantics and allocation counts before choosing
the reset operation. Preserve presentation-epoch resets, sync frames,
`forget_mtx` (`:215`), object deletion and address reuse. Test scene changes,
interpolation toggles and mod-driven model replacement so recycled storage
cannot interpolate stale objects. Capacity reuse must not mean stale entries.

### Material interpolation: allocation before algorithm changes

Model recording is already gated by `needsInterpCallBack`
(`libs/JSystem/src/J3DGraphAnimator/J3DModel.cpp:542`); it does not capture every
model unconditionally. `Values<T>::capture` (`material.cpp:280`) linearly scans
existing entries for duplicates before insertion, and resource capture (`:487`)
also scans. Repeated insertions can become quadratic as a capture grows, but
short vectors can be faster than an additional hash container. Count entries,
duplicate hits, capture duration and allocations before changing membership
tracking. Keep shared ownership and restore all captured material/resource
state after callbacks; pooling must respect model deletion and mod reload.

Do not simply delete intermediate-frame material evaluation:
`src/m_Do/m_Do_ext.cpp:380` explicitly recalculates materials to prevent light
flicker (issue 355). Improving callback eligibility/invalidation might reduce
that work, but requires visual tests for lights and animated materials.

### Lower priority: frame-limiter CPU usage

`src/dusk/time.h:75` sleeps for most of the Windows wait, then polls the
performance counter with `_mm_pause` until the deadline (`:97`–`:107`). A
shorter spin window could save idle CPU/power when frames already have spare
time. It cannot make over-budget gameplay finish sooner, and oversleeping can
worsen pacing. Measure spin duration and deadline jitter before touching it.
`VIWaitForRetrace` in this port (`src/dusk/stubs.cpp:310`) dispatches callbacks;
it is not an additional blocking retrace sleep.

## Focused CPU code scan: loading and UI candidates

Read-only follow-up on 22 October 2026, covering background shader/texture
work, caches, mod callbacks and overlays. The source proves the repeated work
below exists; it does **not** prove that work dominates the console's frame
time. These candidates complement the main-loop/renderer investigation rather
than replace the gameplay evidence above. No changes or speedups are claimed.

### 1. Avoid repeated linear searches when promoting a pending shader

**Proven source behavior:** `find_pipeline_impl`
(`extern/aurora/lib/gfx/pipeline_cache.cpp:468`) calls
`promote_pending_pipeline` for a requested pipeline that is still pending.
That function (`:392`) linearly searches the background deque using
`find_pending_pipeline` (`:382`) while holding `g_pipelineMutex`. After a
pipeline was promoted into the normal queue, repeated requests before its
compilation finishes still scan the background queue and fail to find it.
This can repeat for draw requests during a large shader warmup; already-built
pipelines take the existing map-hit path and do not do this scan.

**Smallest candidate:** track whether a pending ID is still in the background
queue, using the existing pending-state bookkeeping, and skip its promotion
search once it has moved. Avoid a full queue/container rewrite. Every enqueue,
promotion, dequeue and shutdown must update that state together. Preserve
blocking readiness waits, priority, deduplication and native-mod behavior.

**Check before keeping:** count unsuccessful promotion scans, scanned elements
and mutex time on a first-encounter route versus the same warmed route. Add a
focused queue test for repeated requests after promotion and dequeue/shutdown.
No steady-state benefit should be claimed if the compilation queue is empty.

### 2. Deduplicate texture requests before copying their load snapshot

**Proven source behavior:** a cached thumbnail touch calls
`queue_full_load(snapshot_entry(key, *entry))`
(`extern/aurora/lib/gfx/texture_replacement.cpp:1157`; another call at `:1177`).
`snapshot_entry` (`:732`) copies strings/path/source/shared ownership state.
Only after that copy, `queue_full_load` (`:885`) discovers the job is already
pending and merely updates its request-priority counter. Also,
`start_worker_pool` (`:834`) acquires `s_jobMutex` to discover workers already
exist, then the queue function takes that same mutex again. Repeated thumbnail
hits while loading can therefore do avoidable snapshot construction and two
sequential mutex acquisitions without creating a job.

**Smallest candidate:** pass the key/entry into queueing and create the owned
snapshot only on actual insertion; perform lazy-worker initialization and
queue bookkeeping under one appropriate synchronization path. Retain priority
refreshes for already-pending jobs; those favor currently used textures. Do
not replace the queued snapshot with borrowed references or remove lifecycle
locking: mod reload/unregister must not leave workers with dangling paths or
callback userdata. Respect the existing registry-before-job lock ordering.

**Check before keeping:** count requested versus inserted jobs, snapshot
constructions, allocation bytes and job-lock wait during the same streaming
route. Test repeated requests, priority refresh, mod disable/unregister while
loading, and final publication. This primarily addresses texture-streaming
periods, not frames whose replacements are fully resident.

### 3. Reduce publication sorting and scratch allocations during a backlog

**Proven source behavior:** `process_streaming`
(`extern/aurora/lib/gfx/texture_replacement.cpp:1751`) stable-sorts pending
publications while holding `s_jobMutex`. Its comparator looks up both operands'
priorities in `s_pendingFullLoads`, repeating those hash lookups throughout the
sort. A fresh local `deferred` vector (`:1762`) grows as over-budget textures
are skipped and replaces the old vector (`:1805`), discarding its previous
capacity. When a backlog spans frames this repeats while workers also need the
job mutex. With no backlog there is little work here; sorting an empty vector
is not itself a substantive bottleneck.

**Smallest candidate:** snapshot each completion's priority once under the
mutex, then sort by the captured values outside it, preserving stable order on
ties. Reuse publication scratch capacity rather than allocate a fresh deferred
buffer each call. Do not skip sorting solely because no new completion arrived:
visible-texture touches can update existing priorities. Keep the publish-byte
budget, stale-entry checks and correct texture/source lifetime unchanged.

**Check before keeping:** time sorting and mutex hold/wait, count backlog size
and allocation activity, and compare publication order plus upload-byte spikes.
Test changing priorities, equal-priority stability, deferred oversized textures
and unregister/re-register while completions are pending. A smaller LRU can
increase this backlog; evaluate both together rather than assuming it helps CPU.

### 4. Avoid submitting unchanged HUD attributes every presentation

**Proven source behavior:** `Overlay::update`
(`src/dusk/ui/overlay.cpp:276`) sets FPS visibility/corner and removes/replaces
its bottom property every update while the overlay is enabled, despite updating
the text only twice per second. `update_pipeline_progress` (`:473`) repeatedly
removes the progress widget's `open` attribute when the queue stays zero and
sets the progress value each update while active.

**Smallest candidate:** apply visibility/corner/style/value changes only when
the corresponding observable state changes; keep text sampling, compilation
progress, corner avoidance and transitions correct. RmlUi may internally ignore
some identical values, so source-level repeated calls do not establish repeated
full layout work. Do not hide HUD information or force users to disable it.

**Check before keeping:** temporarily compare the same warmed gameplay with
FPS overlay on/off, then count attribute/property calls and UI update/layout
time. If these calls are negligible, leave the implementation alone. This is a
lower-priority candidate than measured shader/streaming or game-thread costs.

### Mod and logging work that this scan does not justify removing

The installed Dark Link 1.0.0 package was inspected locally at
`/tmp/com.loadingerror.dink-1.0.0.dusk` (`res/main.luau`). Its 43 texture
registrations occur on initialization/config subscriptions; it registers **no
per-frame update callback**. `runtime_update` immediately returns when
`vm.updateRefs` is empty (`mods/luau_runtime/src/runtime.cpp:409`–`:417`). It is
therefore incorrect to describe Dark Link as looping through all 43 textures
in Lua every frame. Keep its textures/models and subscription behavior.

General mod updates are dispatched by `ModLoader::tick`
(`src/dusk/mods/loader/loader.cpp:1241`), so profile individual active native
updates if they appear in the trace. Arbitrarily skipping/throttling all mod
callbacks changes behavior. Disc-overlay rebuilds already have a dirty check
(`src/dusk/mods/svc/overlay.cpp:339`); do not add another per-frame rebuild
cache. Bundle-backed disc overlays do read their contents again on file open
(`:143`), but repeated opens and decompression cost have not been established
for the slow gameplay route, so a new memory cache is not yet warranted.

Pipeline cache writing already batches queued records into a transaction
(`gfx/pipeline_cache.cpp:914`), and only changed known configurations enqueue
writes (`:438`). Missing-texture debug dumps are disabled by default
(`gfx/texture_replacement.cpp:1814`). These are not unconditional per-frame
logging/database floods. Debug-line flushing was already removed. Preserve
error handling, cache invalidation, mod ownership and callback safety.

## GDK versus the current retail Developer Mode setup

Microsoft's public GDK repository states that Xbox console development requires
**GDK with Xbox Extensions (GDKX)** and licensed partner access through managed
programs such as ID@Xbox: <https://github.com/microsoft/GDK/blob/Main/README.MD>.
The public PC GDK is not a replacement for the console extensions. Retail Xbox
Developer Mode's UWP deployment is a different route; installing the public GDK
or building on a Windows runner does not convert it to native GDK deployment.
A partner-approved native console port would be a separate platform effort,
not an immediate optimization or a guarantee of 1080p60 on this One S.
No GDK migration or change to the console's mode was performed.

## Minimal measurement and acceptance plan

1. Back up settings/saves and leave the ISO, app identity and persistent caches
   intact. Record exact package, driver, app/game resource mode, display refresh,
   render size, mod list, and config. Do not silently disable the user's mods.
2. Establish a base-game diagnostic comparison, then Dark Link, then TPDE+
   if installed. Only change optional mod/pack state temporarily and restore it.
   Run the same save/camera route: an indoor room, an open field with moving
   camera, a populated/combat scene, water, and a scene transition. If the saved
   progression does not expose these locations, record that coverage gap rather
   than creating or overwriting a save.
3. Let shader warmup finish. Keep the first encounter/streaming run as a separate
   result; then repeat the warmed route three times for at least 60–120 seconds
   per scene, plus a 10-minute memory/thermal stability pass. Include the exact
   same route after relaunch to test cache reuse.
4. Reuse existing `ZoneScoped`/`FrameMark` and texture counters where practical:
   `extern/aurora/lib/aurora.cpp:454`, `extern/aurora/lib/gx/texture.cpp:710`.
   Tracy defaults off (`extern/aurora/extern/CMakeLists.txt:383`). A diagnostic
   build can enable it without redesigning telemetry. GPU timestamp support is
   requested only with Tracy and only if exposed by the adapter
   (`extern/aurora/lib/webgpu/gpu.cpp:962`); if absent, state that GPU time is
   unavailable and use resolution-scaling comparisons as a clue, not proof.
5. Record frame-time median, p95, p99, worst frame, percentage over 16.67 ms,
   long stalls over 33.33/50 ms, simulation-tick time, CPU recording/submission
   time, GPU time where available, pipeline queues, texture uploads/pending
   loads/evictions, and peak memory. Device Portal's previously returned CPU=0
   and a one-second FPS overlay are insufficient for CPU profiling or percentiles.
6. For the uncapped diagnostic workload, CPU/GPU work should leave margin below
   16.67 ms (provisionally aim p95 below 15 ms); for the capped run inspect
   presented pacing near 16.67 ms, report p99 and missed refreshes explicitly.
   Do not demand measured frame intervals below the refresh interval when Vsync
   is pacing correctly. Menu results do not satisfy gameplay acceptance.
7. Apply one candidate, repeat all comparable measurements, retain only changes
   that improve the intended metric without broken visuals/input/audio/saves or
   new memory pressure. Run existing relevant cache/texture self-checks if their
   implementation changes; config-only experiments need hardware A/B checks,
   not tests that merely echo the config values.

No optimization, FPS gain, sustained 1080p60 result or TPDE gameplay validation
is claimed by this assessment. Actual bottlenecks and the ability to sustain
native 1080p60 remain unmeasured.
