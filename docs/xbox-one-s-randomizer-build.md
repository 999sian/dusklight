# Build Randomizer for Xbox

**Requires a Windows build environment** with Visual Studio 2022 C++/UWP tools,
Windows SDK and CMake. Use the dependency setup and UWP configuration from
[the Xbox build workflow](https://github.com/999sian/dusklight/blob/xbox-one-s-uwp-v2.0.3/.github/workflows/build.yml).
GitHub's Windows runner can provide these tools if you have no Windows PC.
These steps start from a configured Dusklight Xbox checkout with its normal
Luau package staged, ready to build/sign an MSIX.

1. In the **Dusklight repository folder**, get the tested Randomizer source:

   ```powershell
   git clone --branch xbox-one-s-v2.0.3 https://github.com/999sian/dusklight-randomizer.git mods/randomizer
   git -C mods/randomizer checkout 6bd800033cf39e37da620c5e4ce29ddd94df10d6
   ```

   If that folder already exists, use the existing checkout instead of cloning.

2. In Dusklight's `CMakeLists.txt`, add this directly after
   `add_subdirectory(mods/luau_runtime)`:

   ```cmake
   add_subdirectory(mods/randomizer)
   ```

3. In PowerShell, reconfigure and build the game first, then Randomizer.
   Stop if any command fails.

   ```powershell
   cmake -S . -B build-uwp
   cmake --build build-uwp --config RelWithDebInfo --target dusklight -- /p:GenerateAppxPackageOnBuild=false
   cmake --build build-uwp --config RelWithDebInfo --target randomizer_package
   ```

4. Extract the built mod into the app's extra-content folder:

   ```powershell
   Add-Type -AssemblyName System.IO.Compression.FileSystem
   $bundle = (Resolve-Path 'build-uwp/bundled_mods/randomizer.dusk').Path
   $destination = Join-Path (Get-Location) 'build/uwp-extra/mods/dev.twilitrealm.randomizer'
   [System.IO.Compression.ZipFile]::ExtractToDirectory($bundle, $destination)
   cmake -S . -B build-uwp
   ```

   Use a fresh destination; do not extract over an older mod build.

5. Repeat the workflow's **development certificate**, **signed sideload package**
   and **symbol-manifest embedding/re-signing** steps. Install the resulting
   MSIX through Device Portal, backing up data first. Keep the same app identity
   and use a newer package version; do not uninstall to update.

**The public workflow intentionally rejects Randomizer and its textures.**
It cannot publish this package unchanged. A separate private build needs
content checks that explicitly allow its expected Randomizer files while
retaining the other checks. Resolve bundled texture redistribution rights
before publishing a package containing them.

This documents the build integration; the current recipe has not been rerun
on Windows. Earlier Xbox testing confirmed initialization, not a playthrough.

