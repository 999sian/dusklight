# Xbox One S UWP: Dusklight v2.0.3 port

This branch starts at upstream v2.0.3. It is an experimental port. Package
2.0.3.33 built and installed in place on Xbox One S, preserved the ISO and save
folder, initialized D3D12, and rendered the prelaunch menu. Starting the game
still hangs on a black screen; gameplay is not verified. The tested v1.1.1 package is preserved by tag
`xbox-one-s-baseline-1.1.1.25`.

Build the root project using Visual Studio 2022, x64, WindowsStore 10.0,
`DUSK_UWP=ON`, the DLL CRT, and embedded debug information. The Xbox workflow
pins SternXD/uwp-dep at `810cdcb672383045a8b03d1d55bb535352d7d46f`.
The root executable links the game's transitive CMake dependencies directly.

The Aurora fork retains the v2.0.3 renderer and adds a CoreWindow surface,
D3D12 backend selection, and `disable_resource_suballocation`. The latter
avoids the Xbox One S driver crash in `CreatePlacedResource` observed with
the original UWP port.

Code mods, Discord, HTTP/WebSocket features and automatic online updates are
unavailable in this initial port. Crash dumps are collected through Xbox Device
Portal. Shader compilation performance has not been tuned.

Package identity and publisher match the tested baseline. Each CI run uses a
new `2.0.3.<run number>` package revision. Before deployment, back up settings and
saves from LocalState, verify the new package's exact installed version, and
check ISO loading and rendering remotely. Do not uninstall the baseline to
upgrade, as uninstalling removes app data.
