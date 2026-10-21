# Xbox One S UWP: Dusklight v2.0.3 port

This branch starts at upstream v2.0.3. It is an experimental port. Package
2.0.3.37 installs in place on Xbox One S, preserves the ISO and save folder,
initializes D3D12 and renders the game intro. Controller input, audio, saving
and sustained gameplay are not yet verified. The tested v1.1.1 package is preserved by tag
`xbox-one-s-baseline-1.1.1.25`.

Build the root project using Visual Studio 2022, x64, WindowsStore 10.0,
`DUSK_UWP=ON`, the DLL CRT, and embedded debug information. The Xbox workflow
pins SternXD/uwp-dep at `810cdcb672383045a8b03d1d55bb535352d7d46f`, but replaces
its SDL3 with SternXD/SDL3-uwp `ddd5d4f` built with
`platforms/uwp/patches/sdl3-stdcpp-thread-id.patch`. Without it, SDL's stdcpp
thread backend leaves every SDL thread ID at 0 and WASAPI deadlocks on the
first audio open, freezing the game on a black screen after the menu.
The root executable links the game's transitive CMake dependencies directly.

The Aurora fork retains the v2.0.3 renderer and adds a CoreWindow surface,
D3D12 backend selection, and `disable_resource_suballocation`. The latter
avoids the Xbox One S driver crash in `CreatePlacedResource` observed with
the original UWP port.

The next build enables native Windows x64 mods, bundles Luau, Cosmetics and
Randomizer, and stages upstream's initial pipeline cache. It also selects a
Windows.Web.Http transport for the built-in mod browser and mod HTTP services.
These changes still require CI and Xbox verification. Discord and WebSocket
services remain unavailable. Crash dumps are collected through Device Portal.

The temporary per-debug-line file flush has been removed, restoring buffered
logging and immediate flushing for warnings/errors. Keep `dawn_cache.db` and
`pipeline_cache.db` across upgrades. The bundled pipeline list merges into the
local cache; it is not compiled Xbox driver code and cannot eliminate initial
shader compilation. No controlled CPU/FPS improvement has been measured yet.

Package identity and publisher match the tested baseline. Each CI run uses a
new `2.0.3.<run number>` package revision. Before deployment, back up settings and
saves from LocalState, verify the new package's exact installed version, and
check ISO loading and rendering remotely. Do not uninstall the baseline to
upgrade, as uninstalling removes app data.
