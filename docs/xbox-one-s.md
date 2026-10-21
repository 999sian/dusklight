# Experimental Xbox One S port

The `Build Xbox UWP (experimental)` workflow uses a Windows 2022 cloud runner.
No local Windows PC is required. Builds currently use the discontinued UWP
1.1.1 source as a baseline; this is not a Dusklight 2.0.3 port yet.

The workflow compiles x64 game libraries with the DLL CRT, then builds and
signs the existing UWP launcher. Both stages use the same pinned UWP
dependency revision. The temporary signing key stays on the runner.

After a successful run, download `dusklight-uwp-experimental-x64` from Actions,
extract it, and use Xbox Device Portal to install the package and its x64
dependencies. Set the installed app's type to Game in Developer Mode.
Supply your own supported game dump. Back up existing saves before testing.

The development certificate changes between runs, so repeated installations
may need certificate handling in Device Portal. Do not uninstall an existing
installation without backing up its LocalState data.

Xbox One S support is unverified. A successful cloud build establishes only
that the package compiles; console startup, rendering, controller input,
save/load and performance require hardware tests.

Next: establish the baseline build, adapt the UWP integration and Aurora
dependencies to upstream v2.0.3, then test on an Xbox One S.
