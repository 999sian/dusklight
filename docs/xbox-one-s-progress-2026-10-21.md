# Dusklight on Xbox One S — progress and handoff

Saved 21 October 2026 (Europe/London). This is a snapshot; check GitHub and
Device Portal again before continuing.

## Optimization and mod support work

Package **2.0.3.46** passed run `39617057857` and in-place deployment; all
five save/settings hashes still matched backup `before-v2-20261021-200913`.
Normal launch still exits before the first native-runtime checkpoint.
Hardware isolation now narrows the failure to **directory-backed mod scanning**:
all three asset-only ZIP overrides remain stable; ZIP overrides containing the
original native DLLs also remain stable and log the expected unsupported
LocalState DLL rejection; adding one directory-backed asset-only mod containing
only `mod.json` reproduces the exit. No DLL is needed to trigger this failure.
All temporary manifests/packages were removed and the original config restored.
An empty `mods/temporary-disk-isolation` folder remains ignored by discovery.
Evidence: `v46-startup.log`, `v46-native-zip-isolation.log` and
`v46-asset-directory-isolation.log` in the diagnostics directory.

Next candidate replaces physical `fs::relative` resolution in directory bundle
enumeration with `path.lexically_relative(root_path)`. The iterator already
provides paths prefixed with the root. A local assert test against the actual
bundle implementation fails before and passes after the change for linked
resource names, listing/readability and file sizes. Hardware confirmation is
still required; a flushed pre-enumeration checkpoint was also added.


Previous retry: package **2.0.3.45** passed build `39612531442`, package inspection
and in-place installation, retaining all five backed-up files unchanged. Normal
startup still exits. The package-loader change alone did not fix it. Added a
few flushed startup checkpoints around native runtime preparation, DLL opening,
metadata validation and activation to locate the next failure; steady-state
debug messages remain buffered. Device Portal again retained no usable dump.
Newest backup: `before-v2-20261021-151540` under the diagnostics directory.

Earlier hardware isolation: package **2.0.3.42** built successfully in run
`39605326169` and was installed in place. All five backed-up settings/save
files matched their previous SHA-256 after installation; the ISO and both
caches remained present. The normal launch exits during native-mod startup.
Temporary asset-only packages shadowing all three bundled native mods let the
same package reach its menu and remain running for 30 seconds, with the shader
warmup queue visible. Luau, Cosmetics and Randomizer each reproduce the exit
when allowed individually. Temporary overrides were removed and configuration
restored after each test. This isolates a shared native-loading/init problem;
Device Portal did not retain a usable crash dump. A change to use the supported
package-relative `LoadPackagedLibrary` route for bundled UWP DLLs is awaiting
a new Windows build and hardware verification. Downloaded native DLLs are now
rejected until a supported Xbox loading route is established. Runs `39611566818`
and `39611025870` were canceled and superseded by 45.
Do not describe 42 as a working replacement for 37.

Latest backup before installing 42:
`/home/sian/Downloads/dusklight-xbox-diagnostics/before-v2-20261021-141733`.
Evidence: `v42-first-launch.png`, `v42-without-native-mods.png` and
`v42-startup.log` in the same diagnostics parent directory.

The startup freeze is fixed. The current focus is Xbox One S optimization,
bundled pipeline-cache seeding and broad mod compatibility, including the
built-in online mod browser. Native Windows x64 mods can still require APIs
that Xbox UWP does not provide; compatibility must be checked on hardware.

- Removed the temporary UWP `flushOn = Debug` override. Borealis retains debug
  messages but buffers file output, flushing immediately at Warning and above.
- Preserve the console's existing `dawn_cache.db` and `pipeline_cache.db`
  during in-place updates. CI stages upstream v2.0.3's hash-pinned
  `initial_pipeline_cache.db`; Aurora merges its pipeline records into the
  local database. The seed is not compiled Xbox GPU code. First-run GPU
  compilation remains necessary.
- CI now builds and packages Luau runtime, Cosmetics and Randomizer. The UWP
  architecture helper and `.dusk` ZIP extraction were fixed after the first
  build failure. Hardware verification of native loading/hooks is pending.
- Added a Windows.Web.Http transport to the Borealis fork and selected it for
  UWP so the existing browser/download and mod HTTP services can use HTTPS.
  It preserves certificate validation, streaming, cancellation, timeouts and
  HTTPS-only redirects. Offline rejection tests and a Windows WinRT CI matrix
  entry were added. Browser downloads on Xbox are not yet verified.
- WebSocket services remain unavailable in this port. Mods requiring them
  are not yet supported.

The working installed baseline remains **2.0.3.37**. Its startup menu displayed
60 FPS; a later intro/title screenshot at the saved 2x-resolution/interpolation
settings displayed 47 FPS. These isolated readings are not benchmarks or proof
of a CPU bottleneck. Device Portal reported zero CPU in samples even while
rendering, so those samples are not usable measurements. Original settings
were restored after this test.

Newest settings/save backup:
`/home/sian/Downloads/dusklight-xbox-diagnostics/before-v2-20261021-132511`.
Shader/GPU caches and the ISO were not removed. Pending builds must pass package
inspection and an in-place console test before recording an improvement.

The focused Windows check now passes: run
[39606283522](https://github.com/999sian/dusklight/actions/runs/39606283522)
passed all 22 HTTP tests, including real catalog GET/HEAD requests, a real asset
mod's download resume/stale-partial replacement/completion handling, and the
Randomizer endian check. The HTTP backend keeps its COM apartment alive for
the worker lifetime and registers each completion handler only once. The full
Xbox package build is
[39605326169](https://github.com/999sian/dusklight/actions/runs/39605326169).
It must pass before deployment. Read [optimization verification](xbox-one-s-optimization.md)
and [mod compatibility checks](xbox-one-s-mod-compatibility.md) for measured
facts and remaining limits.

## Current result

Dusklight based on upstream **v2.0.3 builds successfully and installs on the
Xbox One S**. Installed package **2.0.3.37** initializes D3D12, loads the
European Twilight Princess ISO and renders the startup menu and game intro.
**Package 2.0.3.33 froze on a black screen at 0 FPS when starting the game;
that freeze is now fixed (2.0.3.37, see below). Full gameplay is not yet verified.**

**Root cause found (afternoon session).** The freeze is a deadlock in
uwp-dep's SDL3 3.4.16, not in the game. Build 2.0.3.36 added main-loop probes
and a 30-second watchdog that fail-fasts so WER writes a dump; the dump
(`/tmp/dusklight-36-hang.dmp`, thread report `/tmp/dusklight-36-threads.txt`)
shows the game thread in `mDoAud_Execute → dusk::audio::Initialize →
SDL_OpenAudioDeviceStream`, waiting on SDL's WASAPI management thread. That
thread is waiting on itself: `WASAPI_ActivateDevice → WASAPI_PrepDevice →
WASAPI_ProxyToManagementThread` queued a task to its own queue because
`ManagementThread->threadid` reads **0** in the dump. SDL 3.4.10 moved thread-ID
assignment from `SDL_RunThread` into each backend's `SDL_SYS_CreateThread`;
SternXD/SDL3-uwp builds the stdcpp backend, which upstream deleted with WinRT,
so it was never updated (still unfixed on `uwp-3.4.18`). The 1.1.1 baseline
shipped an older SDL3, which is why it worked.

Fix: `platforms/uwp/patches/sdl3-stdcpp-thread-id.patch`, applied in CI to the
exact shipped source (SternXD/SDL3-uwp `ddd5d4f`) and copied over uwp-dep's
`SDL3.dll`/`SDL3.lib` before configuration. The probe was removed in the
same commit (`af9cb1b`, run 39592628007). **Verified on hardware:** package
2.0.3.37 booted straight into the game (skip-menu path), ran past game frame
2700 at ~30% CPU with no crash dump, and rendered the intro (Hyrule Castle and
field flyover) at 88–119 FPS while compiling shaders
(`xbox-v2-37-skip-*.png` in the diagnostics folder). On that path a
"No device assigned: Configure Port 1" toast showed; controller input, audio
output, saves and sustained play are still unverified. Config was restored
afterwards (startup menu enabled); saves were backed up first to
`before-v2-20261021-115742`.

The original ISO and European save-card folder survived the in-place upgrade.
Settings and saves were backed up locally before installation. Do not uninstall
the app: uninstalling can remove its data.

## Goal and priorities

- Update the Xbox UWP port to the latest stable version selected for this work,
  upstream v2.0.3.
- Make startup and gameplay work on Xbox One S in Developer Mode.
- Use Windows GitHub Actions because there is no Windows PC available.
- Finish the version upgrade before tuning slow shader compilation.
- Remote deployment, launch and testing are authorized; the TV may be in use.

## Repositories and source state

| Item | Location or revision |
| --- | --- |
| Original UWP repository | https://github.com/XboxEmulationHub/dusklight-uwp |
| Personal fork | https://github.com/999sian/dusklight |
| Upstream release | https://github.com/TwilitRealm/dusklight/releases/tag/v2.0.3 |
| Local checkout | `/home/sian/Documents/dusklight-uwp` |
| Active branch | `xbox-one-s-uwp-v2.0.3` |
| Upstream v2.0.3 base | `40457c6adb381928e4b5fef6ed459ed291edd5e2` |
| Working baseline source | `af9cb1b6afb513899fef8ecc35f528a25a353974` |
| Current optimization/mod build source | `b5be9f8` (run `39660091944`, CI pending) |
| Aurora fork | https://github.com/999sian/aurora — branch `xbox-one-s-v2.0.3`, pin `13baa5a` |
| Borealis fork | https://github.com/999sian/borealis — branch `xbox-one-s-v2.0.3`, pin `2b0b5b6` |
| Pinned binary dependencies | SternXD/uwp-dep `810cdcb672383045a8b03d1d55bb535352d7d46f` |

The upstream `v2.0.3` tag was also pushed to the personal fork. The installed
build logs identify themselves as `v2.0.0-52-dirty` because that tag was missing
when they were configured. Their source revision above is based on v2.0.3;
the package version is 2.0.3.33. Later builds can use the restored tag. CI edits
the manifest revision before configuration, so a dirty suffix is expected.

## Working baseline preserved

The earlier **1.1.1.25** build was tested on this Xbox: startup, D3D12, settings,
ISO loading and the opening Link/Rusl river cutscene worked. Input, audio,
saving and sustained performance were not fully checked.

- Baseline branch: `xbox-one-s-uwp`.
- Baseline tag: `xbox-one-s-baseline-1.1.1.25`, source `fb65c91`.
- Successful baseline run: https://github.com/999sian/dusklight/actions/runs/39568219529
- Artifact: `/home/sian/Downloads/dusklight-xbox-build-39568219529`.
- Screenshot: `/home/sian/Downloads/dusklight-xbox-diagnostics/xbox-one-s-opening-cutscene.png`.

The latest package listing included both old 1.1.1.25 and new 2.0.3.33 entries.
The running process was explicitly confirmed as 2.0.3.33. Do not infer the
active version by taking the first package named Dusklight. A rollback from
2.x may require repackaging the baseline with a higher version; do not
uninstall to force a downgrade.

## Port and build changes completed

The build now uses the root Visual Studio CMake project with WindowsStore,
x64 and `DUSK_UWP=ON`. It links the current game's transitive dependencies
directly, replacing the older two-stage manual library list. It uses the DLL
CRT and embedded debug information, then signs a sideloadable MSIX with a
unique `2.0.3.<GitHub run number>` revision.

Aurora retains the current renderer with UWP-specific CoreWindow/SDL support,
D3D12 selection and the `disable_resource_suballocation` Dawn toggle. That
toggle avoids the Xbox One S `CreatePlacedResource` driver crash observed in
the earlier port. UWP guards replace unsupported desktop APIs, SQLite uses
its WinRT configuration, and PNG/zlib are taken from the pinned depot.
Borealis file access uses UWP-compatible calls such as CreateFile2.

Compiler blockers fixed include legacy CRT/POSIX-name deprecations,
`std::iterator` deprecation, unsigned arithmetic checks, and uninitialized
JSystem/game pointers. `/Zc:__cplusplus` was restored in Aurora to prevent
legacy headers from breaking modern C++ types. The linker excludes static
CRT defaults carried by prebuilt `nod.lib`, matching the tested baseline.

The first linked v2 package failed Xbox installation with **0x80070002**.
Its images existed but its resource index was empty, so the manifest's splash
screen could not resolve. Assets now use Visual Studio Image items. CI dumps
the packaged PRI and requires splash-screen and tile-logo resources before
uploading an artifact. The corrected package installed successfully.

Code mods, Discord, HTTP/WebSocket features and automatic online updates are
disabled for this initial port. Shader performance remains untuned.

## Build history and current diagnostic run

| Run | Result |
| --- | --- |
| [39575531424](https://github.com/999sian/dusklight/actions/runs/39575531424) | Failed on two overlay `strdup` deprecation errors; fixed. |
| [39577001519](https://github.com/999sian/dusklight/actions/runs/39577001519) | Passed overlay compilation; failed on 28 game pointer warnings promoted to errors; initialized reported declarations. |
| [39579334661](https://github.com/999sian/dusklight/actions/runs/39579334661) | All source compiled; link failed on `LIBCMT.lib`; fixed runtime defaults. |
| [39581609255](https://github.com/999sian/dusklight/actions/runs/39581609255) | Build passed, package 2.0.3.32; Xbox rejected its missing indexed splash-screen resource. |
| [39584162275](https://github.com/999sian/dusklight/actions/runs/39584162275) | Build and PRI check passed; package 2.0.3.33 installed and tested as described above. |
| [39587179941](https://github.com/999sian/dusklight/actions/runs/39587179941) | **In progress at this snapshot.** Source `1acb720` flushes UWP logs at Debug level to locate the freeze. Not yet downloaded or installed. |

Recent full builds take about 15–17 minutes. Completed logs can be fetched
with `gh run view <run> --repo 999sian/dusklight --log`. In-progress logs were
not available through the API used here. GitHub CLI authentication is already
configured; do not print credentials.

## Hardware tests and the unresolved freeze

1. Device Portal accepted 2.0.3.33. Its process metadata confirmed the new
   package was running, using roughly 500 MiB of working set.
2. Logs confirmed WindowsStore, Xbox One CPU, a 5 GiB app memory budget,
   D3D12 on the SraKmd GPU and a 1920×1080 framebuffer.
3. The European ISO was verified and opened as disc ID **GZ2P01**.
4. With `backend.skipPreLaunchUI=true`, startup stayed black at 0 FPS.
5. Setting `backend.skipPreLaunchUI=false` rendered the Dusklight menu at
   about 38 FPS in one screenshot and detected the Xbox One controller.
6. Continuing into the game froze again. The latest screenshot was black at
   0 FPS. This does not yet establish whether the fault is graphics,
   game-thread initialization, audio or synchronization.
7. A separate blank raw memory-card test also stayed black. It did not isolate
   the existing GCI save folder as the cause. The original card configuration
   was restored. A generated raw test card may remain alongside the original
   files; no original save was deleted.
8. The live process-dump endpoint returned HTTP 500 and produced no dump.
   Crash capture was enabled for the new package, but there is no verified
   crash dump explaining this hang.
9. Logs were buffered and ended partway through messages. Their final text
   cannot establish the exact blocking function. The diagnostic build makes
   Debug-level messages flush immediately to address that evidence gap.

**Console state at snapshot:** app stopped for the next upgrade; Developer
Mode and Device Portal remain available. Original card setting restored;
startup menu enabled. Sampled CPUUsage was 0, but this alone does not prove a
deadlock or rule out shader work. No root cause is confirmed yet.

## Device, game data and backups

- Device Portal: `https://192.168.1.150:11443` (self-signed certificate).
- Xbox OS: `26100.9445.amd64fre.xb_flt_2608ge.260927-0830`.
- Current installed v2 package: `Dusklight_2.0.3.33_x64__5pxj2cgsb63vw`.
- App ID: `Dusklight_5pxj2cgsb63vw!App`.
- Shared LocalState folder:
  `U:\Users\UserMgr0\AppData\Local\Packages\Dusklight_5pxj2cgsb63vw\LocalState\TwilitRealm\Dusklight`.
- Local ISO: `/home/sian/Documents/dusklight-uwp/local-game/Twilight Princess (Europe).iso`.
- Xbox ISO: `Twilight Princess (Europe).iso` in that LocalState folder.
- ISO size: **1,459,978,240 bytes**.
- Previously verified ISO SHA1: `2601822a488eeb86fb89db16ca8f29c2c953e1ca`.
- Save card: `EUR/Card A/01-GZ2P-gczelda2.gci`.
- The ISO is ignored by Git and must never be committed or uploaded to GitHub.

Pre-upgrade backups, each with a SHA256 manifest:

- `/home/sian/Downloads/dusklight-xbox-diagnostics/before-v2-20261021-103904`.
- `/home/sian/Downloads/dusklight-xbox-diagnostics/before-v2-20261021-112108`.

Backups include config, achievements, ImGui settings and the 32,832-byte
European GCI save. They intentionally exclude the large ISO, logs and shader
cache databases. The ISO remained present after upgrading.

Artifacts and evidence:

- Corrected installed build: `/home/sian/Downloads/dusklight-xbox-build-39584162275`.
- Diagnostics: `/home/sian/Downloads/dusklight-xbox-diagnostics`.
- Menu proof: `xbox-v2-menu-test.png` in that diagnostics folder.
- Freeze proof: `xbox-v2-frozen.png` in that diagnostics folder.
- Raw-card test: `xbox-v2-raw-test.png` in that diagnostics folder.
- Config before raw-card test: `v2-config-before-raw-test.json` in that folder.
- Temporary logs: `/tmp/dusklight-v2-startup.log`,
  `/tmp/dusklight-v2-menu.log`, `/tmp/dusklight-v2-frozen.log`,
  `/tmp/dusklight-v2-raw.log`.

Useful temporary scripts/state (copy or recreate if `/tmp` is cleared):

- `/tmp/dusk-backup-before-upgrade.py`: back up settings/save files with hashes.
- `/tmp/dusklight-installed-package.json`: metadata for 2.0.3.33.
- `/tmp/dusk-verify-launch.py`: older baseline test script; do not run blindly,
  because it changes config and reads the whole ISO.

## Resume checklist

- [ ] Check diagnostic run 39587179941 and inspect failures if any.
- [ ] If it passes, download `dusklight-uwp-experimental-x64` and verify its
  actual manifest version, executable, DLLs and indexed image resources.
- [ ] Refresh the save backup if the user has changed saves or settings.
- [ ] Install in place with its certificate and x64 VCLibs dependency.
  Device Portal can report completion before the refreshed package list is
  visible: poll and select the exact new package version.
- [ ] Reproduce the hang and read the newly flushed log. Compare skip-menu
  startup with Play from the menu. Restore diagnostic configuration afterward.
- [ ] Fix the demonstrated root cause, rebuild and verify actual game rendering.
- [ ] Return log flushing to a normal threshold after diagnosis if the extra
  writes affect performance.
- [ ] Verify controller input, audio, saving/loading and sustained gameplay.
- [ ] Only then tune shader compilation or broader performance.

For REST deployment, establish a session with Device Portal, obtain its CSRF
cookie and send that token in the CSRF header without logging it. Package
upload uses `/api/app/packagemanager/package`; completion uses
`/api/app/packagemanager/state`. Verify package identity separately via
`/api/app/packagemanager/packages`. Remote launch uses the base64 app ID at
`/api/taskmanager/app`; screenshots use `/ext/screenshot`. App files are
available through `/api/filesystem/apps/files` and `/api/filesystem/apps/file`
with `knownfolderid=LocalAppData`, the exact package full name and the
LocalState-relative path.
