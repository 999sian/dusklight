# Dusklight for Xbox One S — experimental UWP port

An unofficial Xbox Developer Mode port of
[Twilit Realm's Dusklight](https://github.com/TwilitRealm/dusklight), a
reverse-engineered reimplementation of Twilight Princess. This fork is not
affiliated with Nintendo, Microsoft or the upstream Dusklight team.

**Bring your own legally dumped game disc. No game disc, save, extracted game
data or texture pack is supplied with the Xbox release.**
See [Dolphin's disc-dumping guide](https://wiki.dolphin-emu.org/index.php?title=Ripping_Games).
Do not upload game files in issues or pull requests.

## Download and install

Download the experimental x64 package from
[GitHub Releases](https://github.com/999sian/dusklight/releases). This is a
Developer Mode sideload package, not a Microsoft Store or retail-mode app.
You can use a browser on another device; a Windows PC is not required for
Device Portal installation.

1. Enable Xbox Developer Mode and open **Dev Home**.
2. Enable remote access and open the Device Portal address shown by your Xbox
   in your browser. Authenticate with your own portal credentials.
3. Extract the release ZIP. In Device Portal's app deployment page, select the
   `.msix` and include its x64 VCLibs dependency from `Dependencies/x64` if
   requested. The public `.cer` is included for sideload trust where required;
   no private signing key is distributed.
4. Install, then launch Dusklight from Dev Home. When updating, install over the
   existing app identity. **Back up your data first; do not uninstall to update.**
5. Supply your own supported disc image, select it in Dusklight and assign
   your controller to Port 1 in Settings.

For large personal texture packs, allocate sufficient Developer Mode storage
before transferring files. Dusklight's **Change Data Folder** setting can use
an internal development-files folder such as
`D:\DevelopmentFiles\DusklightData`. Keep backups before changing folders or
resizing storage. Your usual retail-mode free space does not establish the
space available to each Developer Mode folder.

## What is included

- Dusklight 2.0.3 with Xbox UWP startup, rendering, audio and file-access fixes.
- Seven CPU work-reduction changes: FIFO batching, matrix/material storage
  reuse, shader-queue tracking, texture-request deduplication, texture-publication
  sorting and guarded HUD updates.
- An upstream initial pipeline list; shaders still compile for the console.
- A bundled UWP build of **Luau Support**.
- Third-party license notices in `res/licenses` inside the package.

The public package excludes the upstream randomizer's bundled texture assets
and the static gameplay-screenshot background pending asset-rights review.
Cosmetics is excluded because its separate repository has no license or
redistribution terms. These optional submodules are excluded from the release source.
Dawnlight, Twilit Essentials, Dark Link and TPDE+ are **not bundled**. The
built-in mod browser does not guarantee Xbox compatibility: desktop native
DLLs generally need a UWP rebuild and inclusion in a signed package. Native
DLLs downloaded into app data cannot simply be loaded as desktop mods.

## Testing and limits

On Xbox One S, the preceding CPU build (2.0.3.53) passed a 60-second remote
startup check with a user's separately supplied disc and mods. CI passed the
full Windows/UWP build, interpolation checks, HUD regressions and native-mod
metadata checks. See [CPU implementation and evidence](docs/xbox-one-s-cpu-implementation.md)
for exact results. That test is not a gameplay benchmark or validation of every mod.

**Sustained 1080p60 has not been demonstrated.** The game simulates at 30 Hz;
interpolation can present intermediate frames at 60 FPS. First-launch shader
compilation can take time. FIFO batching and animated materials still need
gameplay checks, as do controller input, audio and save/load. Back up saves
before trying an experimental build.

## Build and contribute

The active branch is `xbox-one-s-uwp-v2.0.3`. The
[Windows build workflow](.github/workflows/build.yml) pins UWP dependencies,
builds the application and supported bundled mods, signs a development package
and checks the package contents. GitHub-hosted Windows runners provide the
Windows toolchain; Windows containers require a suitable Windows host.

For general source builds, see [building.md](docs/building.md). For upstream
releases and support, use [twilitrealm.dev](https://twilitrealm.dev/). Report
Xbox-port issues to [this fork](https://github.com/999sian/dusklight/issues),
including the package version and error text, with credentials removed.

## Licenses and credits

Dusklight's source is offered under [CC0 1.0](LICENSE.md). Dependencies and fonts
retain their own licenses and copyright notices; CC0 does not override those
terms or grant rights to Nintendo game assets or trademarks. See
[third-party notices](THIRD_PARTY_NOTICES.md) and the
[release-content audit](docs/release-content-audit.md). This audit is a technical
inventory, not a guarantee of the legal status of every inherited file.

Thanks to [Twilit Realm](https://github.com/TwilitRealm/dusklight),
[zeldaret/tp](https://github.com/zeldaret/tp),
[Aurora](https://github.com/encounter/aurora),
[SternXD's UWP dependencies](https://github.com/SternXD/uwp-dep), and all
upstream contributors. Please retain their notices when redistributing.
