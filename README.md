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

**[Instructions for each mod, including Randomizer](docs/xbox-one-s-mod-compatibility.md)**

1. Close Dusklight and back up your saves.
2. In Device Portal, open `LocalState\TwilitRealm\Dusklight` in Dusklight's
   app data. Use your custom data folder instead if you set one.
3. Copy asset/Luau `.dusk` files into `mods/`. Do not unzip them.
4. Launch Dusklight → **Mods** → enable the mod and its dependencies.

For loose texture packs, extract the selected textures into
`texture_replacements/<pack-name>/`, then enable **Enable Texture Replacements**
in graphics settings.

| Mod | Xbox status |
| --- | --- |
| [Luau Support](mods/luau_runtime) | Included. Enable in **Mods**; no download needed. |
| Play as Dark Link 1.0.0 — LoadingError | Needs Luau. Manual installation activated it; appearance unverified. |
| [TPDE+](https://gamebanana.com/mods/714018) | Startup loaded textures; gameplay/performance checks pending. Tested selection: 7.76 GB. |
| [Randomizer — our Xbox fork](https://github.com/999sian/dusklight-randomizer/tree/xbox-one-s-v2.0.3) | Not installable in this public release. Needs signed app bundling; texture rights review pending. |
| Cosmetics | Excluded pending redistribution terms. |
| [Dawnlight 5.1.0 — our Xbox fork](https://github.com/999sian/dawnlight/tree/xbox-one-s-v2.0.3) | Experimental package 2.0.3.1006; separate Xbox startup test passed. Gameplay unverified. |
| [Roll and Sprint 1.1.0 — our Xbox fork](https://github.com/999sian/Roll-and-Sprint/tree/xbox-one-s-v2.0.3) | Experimental package 2.0.3.1006; separate Xbox startup test passed. Gameplay unverified. |
| [TP Classic Modern Controller UI 1.4.0 — our Xbox fork](https://github.com/999sian/TP-CM-Controller-UI/tree/xbox-one-s-v2.0.3) | Experimental package 2.0.3.1006; separate Xbox startup test passed. In-game prompts unverified. |
| Twilit Essentials | Current version has no compatible Xbox build here. |

### Try the three new Xbox ports

These mods are bundled in **experimental package 2.0.3.1006**, separate from
public release 2.0.3.54. Desktop mod downloads will not work on Xbox.

1. Sign in to GitHub and open the [successful build](https://github.com/999sian/dusklight/actions/runs/39714675293).
   Under **Artifacts**, download `dusklight-native-mod-ports-experimental-x64`.
2. Extract the ZIP and install its `.msix` through Device Portal using the
   steps above. Back up first; **do not uninstall**.
3. Open **Mods**. Enable **Dawnlight** or **Roll and Sprint**, and disable the
   other. Both change sprinting; using them together is untested.
4. Enable **TP Classic Modern Controller UI** and choose **Xbox** before
   starting the game. Restart Dusklight if you change controller style later.
5. Assign your controller to **Port 1**, then start the game.

All three passed separate 30-second startup tests on Xbox One S. Saves and
user mods were checked unchanged. Gameplay and mod combinations still need
testing. Dawnlight's optional boss-rush music is unavailable without its
`mods/temp.ast` file. Updates require a new signed app package.

[Package details and test results](https://github.com/999sian/dusklight/blob/xbox-mod-ports/docs/xbox-native-mod-ports.md).

The browser's permission error is unresolved. Use manual transfer for asset/Luau
mods. Downloaded native DLL mods cannot load from app data.
If a mod crashes, close the app and move the new mod out. **Keep your saves;
do not uninstall.**

### Which mods did we change?

[Randomizer Xbox build instructions](docs/xbox-one-s-randomizer-build.md).

Randomizer's changes cover float byte conversion, an MSVC build fix and a
regression test. Dawnlight, Roll and Sprint and TP Classic Modern Controller UI
also have Xbox forks, linked above: their builds use the Xbox host SDK and
signed bundling; Dawnlight additionally excludes desktop-only touch/updater
code and handles an MSVC build warning. Luau's source,
Cosmetics, Dark Link and TPDE+ content were unchanged; compatibility work is in
the host/SDK and our [Aurora](https://github.com/999sian/aurora/tree/xbox-one-s-v2.0.3)
and [Borealis](https://github.com/999sian/borealis/tree/xbox-one-s-v2.0.3) library forks.

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
Public release 2.0.3.54 does **not bundle** Dawnlight, Roll and Sprint,
TP Classic Modern Controller UI, Twilit Essentials, Dark Link or TPDE+.
The separate experimental 2.0.3.1006 package bundles Luau and the three new
ports; it excludes Randomizer and Cosmetics. The
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
