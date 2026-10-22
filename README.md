# Dusklight for Xbox One S — experimental UWP port

An unofficial fork of
[XboxEmulationHub's dusklight-uwp](https://github.com/XboxEmulationHub/dusklight-uwp),
building on its Xbox UWP port with updates and Xbox One S compatibility and
optimization work. The original game project is
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

### First launch: disc image and controller

1. Launch once to create the app's data folders, then stop Dusklight before
   transferring files or editing settings.
2. In Device Portal's file explorer, select Dusklight's app data. The default
   data folder is `LocalState\TwilitRealm\Dusklight`. Create a `Games` folder
   there and upload your own supported disc image. Do not put it inside the
   installed app package, which is read-only.
3. Launch Dusklight and choose **Select Disc Image**. If the Xbox file picker
   cannot reach the image, stop the app, back up `config.json` in the active
   data folder, then edit its existing flat `backend.isoPath` key to the full
   console path reported by Device Portal. For example, an image transferred
   to the development-files share could use:

   ```json
   "backend.isoPath": "D:\\DevelopmentFiles\\DusklightData\\Games\\My Disc.iso"
   ```

   This is one entry to merge into the existing JSON object; do not replace
   the entire configuration with this example. Keep its other keys and valid
   JSON punctuation. Restart after changing the disc path.
4. In Settings, assign your Xbox controller to **Port 1**, then start the game.
   Allow time for initial shader compilation. Keep the app's cache for later
   launches; the bundled pipeline list does not eliminate compilation.

### Internal storage and large transfers

Developer Mode has its own storage allocation. Retail-mode free space does
not establish the space available to each Developer Mode folder. Back up app
settings and saves before resizing that allocation in Dev Home.

For large packs, use the Xbox's development-files SMB share where available:
`smb://<xbox-ip>/DevelopmentFiles` from a compatible file manager, or
`\\<xbox-ip>\DevelopmentFiles` on Windows. Use the credentials configured for
remote access on your own Xbox. Create a folder such as
`D:\DevelopmentFiles\DusklightData` and leave room for extraction and caches.
This share has worked for a 7.76 GB texture transfer; large browser uploads
previously failed when the destination ran out of space.

Dusklight's **Settings → Data Folder → Change Data Folder** can select a custom
folder when the platform's folder picker is available. Files placed on D: do
not automatically become the active data folder. If the Xbox picker is
unavailable, retain the working default folder; manual relocation requires a
backup and copying the existing settings/saves/mods before changing the data
location. Keep the original files until the new location is verified. Logs
and compiled shader caches can remain in the original preference folder.

## Mods: installation and compatibility

The built-in online browser can list desktop mods that cannot run on Xbox.
A `.dusk` file is a package format, not an Xbox compatibility guarantee.
**The browser's reported installation-permission error is still unresolved.**
Manual placement has activated asset/Luau packages, but does not fix that error.

For an asset or Luau mod obtained from its author:

1. Back up saves and settings, then stop Dusklight.
2. Put the intact `.dusk` file in `mods/` inside the **active data folder**
   (by default `LocalState\TwilitRealm\Dusklight\mods`). Do not unpack it into
   the application or replace the bundled DLLs.
3. Restart, check the Mods screen and enable the mod if needed. Follow the
   author's dependency/version instructions and install one mod at a time.
4. If startup fails, stop the app and move the new package out of `mods/`,
   keeping a backup. Do not uninstall the app or delete saves to recover.

For loose texture packs, follow the author's assembly instructions, copy the
selected textures beneath `texture_replacements/<pack-name>/` in the active
folder and enable **Enable Texture Replacements** in graphics settings. This
is separate from installing a `.dusk` mod. Desktop ReShade files do not provide
an Xbox shader port. Large packs may exceed available storage or memory.

| Mod / type | Xbox status | Included in public release? |
| --- | --- | --- |
| **[Luau Support 1.0](https://github.com/999sian/dusklight/tree/xbox-one-s-uwp-v2.0.3/mods/luau_runtime)** | Bundled UWP runtime executed a script and update callback in a previous Xbox test; individual mods still need testing. | Yes |
| **Play as Dark Link 1.0.0**, by LoadingError | Original asset/Luau package activated after manual transfer. In-game appearance unverified; browser permission error unresolved. This is the appearance mod, not the native Dark Link boss mod. | No |
| **TP Definitive Edition+ 2.1 texture archives** | Original DDS/PNG data transferred to internal storage; startup registered 4,730 textures. Full visual, memory and FPS checks pending. | No |
| **Cosmetics** | Initialized in a previous private build; features not fully tested. Excluded because its repository has no license or redistribution terms. | No |
| **[Randomizer — Xbox fork](https://github.com/999sian/dusklight-randomizer/tree/xbox-one-s-v2.0.3)** | Xbox source fixes exist and initialization passed in a previous private build. Bundled texture redistribution rights remain unresolved; gameplay unverified. | No |
| **Twilit Essentials 2.8.4** | Current source was not available in our investigation; released desktop DLL has unsupported dependencies/imports. No working Xbox port. | No |
| **Dawnlight 5.1.0** | Public source exists, but needs a UWP port and matching game exports. Not installed or ported. | No |
| **Other native DLL mods** | Downloaded DLLs in app data cannot load through the current UWP loader. They need compatible source, a UWP rebuild and signed bundling. Windows x64 alone is insufficient. | No |

HTTP transport is implemented; WebSocket support remains unavailable. Neither
that transport nor a successful startup proves every mod feature works. See
[mod compatibility and test history](docs/xbox-one-s-mod-compatibility.md) for
the evidence and remaining limits.

### Which mods did we change?

| Project | Our changes and source | Distribution status |
| --- | --- | --- |
| **Randomizer** | [Existing Xbox fork](https://github.com/999sian/dusklight-randomizer/tree/xbox-one-s-v2.0.3), commit [`6bd8000`](https://github.com/999sian/dusklight-randomizer/commit/6bd800033cf39e37da620c5e4ce29ddd94df10d6): preserve floating-point bits during endian conversion, an MSVC codecvt build fix and an endian regression test. | Excluded here pending rights review. The fork link is source provenance, not a cleared downloadable Xbox mod. |
| **Luau Support** | Its mod source is unchanged; it lives in this Dusklight tree under [`mods/luau_runtime`](mods/luau_runtime). Xbox compatibility changes are in the host, SDK and build/packaging. | Bundled; no separate mod fork needed. |
| **Cosmetics** | Upstream mod source unchanged; previous private build used host/SDK compatibility changes. | Excluded; no new fork published without redistribution terms. |
| **Dark Link / TPDE+** | Author packages/content unchanged. We transferred and configured them for personal testing. | Not republished or forked; obtain from their authors. |
| **Twilit Essentials / Dawnlight** | No Xbox mod-source changes or completed port. | No Xbox fork/release claimed. |

The supporting [Aurora](https://github.com/999sian/aurora/tree/xbox-one-s-v2.0.3)
and [Borealis](https://github.com/999sian/borealis/tree/xbox-one-s-v2.0.3)
forks contain Xbox engine/platform work. They are libraries, not game mods.

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

Thanks to [XboxEmulationHub for the original UWP port](https://github.com/XboxEmulationHub/dusklight-uwp),
[Twilit Realm](https://github.com/TwilitRealm/dusklight),
[zeldaret/tp](https://github.com/zeldaret/tp),
[Aurora](https://github.com/encounter/aurora),
[SternXD's UWP dependencies](https://github.com/SternXD/uwp-dep), and all
upstream contributors. Please retain their notices when redistributing.
