# Xbox One S CPU work reduction

Implemented 22 October 2026 after the user approved the candidates in
`xbox-one-s-1080p60-assessment.md`. These changes reduce repeated work; they do
not establish a gameplay frame-time improvement or sustained native 1080p60.

## Implemented

| Candidate | Change | Preserved behavior |
| --- | --- | --- |
| FIFO publication | Batch four completed draws rather than one | Explicit publish/drain, GX_AUTO patching, DrawDone, EFB and display-list boundaries |
| Matrix interpolation | Rotate two recording maps; full-range erase retains backing capacity, including replacement map capacity | Matrix values, missing/forgotten keys, presentation sync and history invalidation |
| Material capture | Reuse shared recording/binding objects and vector capacity; reset before reuse only with no outstanding callback owner | Callback ownership, original capture gating and material application/restoration |
| Shader promotion | Track background-queue membership in pending state, avoiding searches after promotion/dequeue | Priority insertion order, readiness, completion, synchronous take and shutdown |
| Texture request deduplication | Construct owned snapshots only for new jobs; start workers and enqueue under one job lock | Duplicate priority refresh, one load per pending ID and registry-before-job lock order |
| Texture publication | Snapshot priority once per completion; stable-sort outside job lock; reuse deferred capacity | Stable ties, stale-selection recheck, upload budget and first oversized upload progress |
| HUD updates | Guard repeated FPS open/corner/bottom and shader-progress value/open writes | Settings changes, progress transitions, FPS sampling and corner avoidance |

FIFO batching applies to all builds, so desktop behavior also needs coverage.
Do not assume fewer notifications always improves latency: batching can change
producer/consumer overlap. It is the first hardware A/B candidate to revert if
pacing worsens. Material storage retains its high-water allocation until history
invalidation; this exchanges repeated allocations for retained capacity.

The material duplicate searches remain linear for small binding lists. No new
membership cache, blanket mod throttling, changed simulation rate, collision/
animation simplification or native export/hook removal was introduced. Existing
intermediate-frame material evaluation remains to prevent lighting flicker.
Worker counts, cache budgets and limiter spin duration remain measurement-led
experiments, rather than unvalidated default changes.

## Local validation

- Full Aurora build and **295/295 CTest tests passed** in
  `/tmp/dusklight-cpu-tests`, including FIFO, queue, streaming, texture-cache,
  render-worker and recording tests. Expected malformed-input diagnostics in
  negative FIFO tests are not failures.
- Repeated pending-texture test observed **40 allocations before, zero after**
  over 20 requests; one pending load and one callback were retained. In-flight
  unregister and stale-failure tests passed too.
- Standalone interpolation test passed with Xbox's pinned **Abseil 20240722.0**.
  Twenty warm ticks recording 256 matrices each made no new allocations, with
  interpolation/stale-key/sync/epoch checks. Material checks cover ownership,
  reused objects/vectors, scalar restoration and invalidation. Animation vector
  reset is inspected in code; real animated-material replay needs game testing.
- Headless HUD test against pinned **RmlUi f00a0fee** passed. It checks 1,000
  unchanged updates without attribute/property callbacks and verifies changed
  corner, height, visibility and progress values. Removing the guards makes the
  regression fail. Rendered layout remains a hardware check.
- Independent source review found no actionable correctness issue. Both Git
  trees passed `git diff --check`. CI runs the standalone interpolation test on
  Linux and the HUD test on Windows, alongside the complete UWP application and
  bundled native-mod build. The standalone interpolation harness includes real
  engine implementation files but intentionally omits the renderer; it relies
  on Linux dead-section elimination. Attempting it with MSVC exposed full-engine
  link dependencies, so Windows correctness is checked through the full app build.

No game/settings/save/ISO files were modified for CPU testing. The full texture
pack remains enabled in internal D: storage as described in the progress doc.

## Windows build and Xbox startup

CI run [39694255721](https://github.com/999sian/dusklight/actions/runs/39694255721)
passed for source `4c48761` and Aurora `cc1c53d`. Both the Linux interpolation
job and Windows HUD/application/native-mod/package job succeeded. The signed
package identity is `Dusklight_2.0.3.53_x64__5pxj2cgsb63vw`.

Before deployment, archive CRC, exact package identity, embedded SYMGEN data,
three bundled native DLLs and pinned shader seed hash were verified. Fresh
backups of both original U: and active D: settings/save/mod files were taken
after stopping the app. Installation completed in place without uninstalling;
all backed-up hashes and the custom-folder descriptor were unchanged, and the
original ISO remained 1,459,978,240 bytes.

Remote launch survived checks at 5, 15, 30 and 60 seconds. The log confirms
config from D:, 4,730 texture registrations, 4/4 active mods and no startup
errors. The screenshot shows the menu, disc ready and shader compilation in
progress. Its 60 FPS counter is menu evidence only. It also reports no device
assigned to Port 1; controller assignment still needs checking before gameplay.
The existing 41×64 BC thumbnail warning remains. Sampled working set ranged
from about 565 to 1,009 MiB during startup; this is not peak gameplay memory or
a controlled memory comparison. Both data-folder hashes remained unchanged
after startup. The app was left running; no TV input/power commands were sent.

Local evidence under `/home/sian/Downloads/dusklight-xbox-diagnostics`:
`before-cpu-package53/`, `v53-cpu-startup.log`, `v53-cpu-startup.png` and
`v53-cpu-startup-verification.json`. The downloaded package is under
`/home/sian/Downloads/dusklight-uwp-39694255721`.

## Remaining acceptance

Use the same save/route/settings for baseline versus this package. Separate cold compilation/
streaming from warmed gameplay. Capture CPU execution, waits, GPU time where
available, p95/p99 frame times and peak memory; compare FIFO batch one versus
four if pacing regresses. Check animated materials, scene transitions, mod
reload, input/audio and save/load. Remote menu survival is insufficient.
