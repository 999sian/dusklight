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

Xbox One S support is unverified. A successful cloud build establishes only
that the package compiles; console startup, rendering, controller input,
save/load and performance require hardware tests.

Next: establish the baseline build, adapt the UWP integration and Aurora
dependencies to upstream v2.0.3, then test on an Xbox One S.

## Xbox startup investigation

The first hardware test reaches Aurora, then crashes with access violation
`0xc0000005` in the console D3D12 driver (`umd12ddi.dll`). The captured stack
contains Dawn’s `ID3D12Device::CreatePlacedResource` path. The pinned Aurora fork
enables Dawn’s `disable_resource_suballocation` toggle for UWP to test committed
resource allocation instead. This workaround still needs a console retest; it
may increase GPU memory use. Original logs and crash dump are retained locally.
