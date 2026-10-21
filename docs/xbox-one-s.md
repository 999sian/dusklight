# Experimental Xbox One S port

The `Build Xbox UWP (experimental)` workflow uses a Windows 2022 cloud runner.
No local Windows PC is required. Builds currently use the discontinued UWP
1.1.1 source as a baseline; this is not a Dusklight 2.0.3 port yet.

Run the workflow on the `xbox-one-s-uwp` branch. If GitHub disables runs on
the fork, sign in as the repository owner and re-enable workflows from
the Actions page.

The workflow compiles x64 game libraries with the DLL CRT, then builds and
signs the existing UWP launcher. Both stages use the same pinned UWP
dependency revision. The temporary signing key stays on the runner.

After a successful run, download `dusklight-uwp-experimental-x64` from Actions,
extract it, and use Xbox Device Portal to install the package and its x64
dependencies. Set the installed app's type to Game in Developer Mode.
Microsoft documents DirectX 12 support on Xbox One for games, plus a
5 GB foreground memory limit, in its
[resource allocation guide](https://learn.microsoft.com/en-us/windows/uwp/xbox-apps/system-resource-allocation).
Supply your own supported game dump. Back up existing saves before testing.

The development certificate changes between runs, so repeated installations
may need certificate handling in Device Portal. Do not uninstall an existing
installation without backing up its LocalState data.

Xbox One S startup, the settings UI, ISO loading and opening-cutscene rendering
were verified remotely on 2026-10-21 with package 1.1.1.25 (run 39568219529,
source fb65c91). Native internal resolution and frame interpolation disabled
were used for this test. Controller input, audio, save/load, sustained performance
and a full playthrough remain unverified.

Next: recheck the latest stable upstream version, adapt the UWP integration
and Aurora dependencies to that release, then repeat the Xbox One S tests.

## Xbox startup investigation

The first hardware test reaches Aurora, then crashes with access violation
`0xc0000005` in the console D3D12 driver (`umd12ddi.dll`). The captured stack
contains Dawn’s `ID3D12Device::CreatePlacedResource` path. The pinned Aurora fork
enables Dawn’s `disable_resource_suballocation` toggle for UWP to test committed
resource allocation instead. The console retest passed graphics initialization and rendered the opening
cutscene without a new crash dump. Committed allocations may increase GPU
memory use; the process used about 519 MiB during the initial test. Original
logs, screenshots and crash dump are retained locally.

Each workflow run assigns its run number as the fourth package version component.
This avoids replacing different development binaries under the same package
identity/version, which produced a broken registration during console testing.
