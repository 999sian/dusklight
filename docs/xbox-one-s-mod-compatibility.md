# Xbox One S mod compatibility

## Public release 2.0.3.54

Only Luau Support is bundled. Cosmetics and Randomizer initialization below
refers to earlier private test builds: both are excluded from this public
package pending redistribution clearance. Randomizer's source changes already
exist in [our Xbox fork](https://github.com/999sian/dusklight-randomizer/tree/xbox-one-s-v2.0.3)
at `6bd8000`; the other tested mods' source/content was not modified.
See the [README](../README.md#mods-installation-and-compatibility) for installation,
mod status and fork provenance. Browser installation permissions remain unresolved.

## Xbox mod source links

| Mod | Source used for Xbox | Verified status / availability |
| --- | --- | --- |
| Randomizer | [Our Xbox fork — `xbox-one-s-v2.0.3`](https://github.com/999sian/dusklight-randomizer/tree/xbox-one-s-v2.0.3), commit `6bd8000` | Endian/MSVC fixes; initialized in private package 49. Gameplay unverified. Excluded from the public package pending bundled texture rights review. |
| Luau Support | [Runtime in our Dusklight fork](https://github.com/999sian/dusklight/tree/xbox-one-s-uwp-v2.0.3/mods/luau_runtime) | Bundled UWP build; script and update callback executed on Xbox. Mod source unchanged; host/SDK compatibility work is in the main fork. Individual mods need testing. |

Cosmetics initialized in an earlier private build, but its source was unchanged
and no separate fork was published; it is excluded pending redistribution terms.
Dark Link and TPDE+ used unchanged author content, so we have no modified forks
to link. These source links do not make downloaded desktop DLLs Xbox-compatible.

## How to use mods on Xbox

### 1. Install the Xbox app and find the active data folder

Install the [Xbox release](https://github.com/999sian/dusklight/releases/tag/v2.0.3-xbox-one-s.54)
through Device Portal as described in the [setup guide](../README.md#download-and-install).
Launch once, then stop the app before copying files. Back up settings and saves.

In Device Portal's file explorer, select Dusklight's app data and open
`LocalState\TwilitRealm\Dusklight`. This is the default data folder. If you
previously configured a custom data folder, use that folder instead; for
example `D:\DevelopmentFiles\DusklightData`. Do not copy mods into both places
or into the installed application directory.

### 2. Luau Support and Play as Dark Link

**Luau Support is already inside the public Xbox MSIX.** You do not need to
clone its source, build it, or download a desktop runtime DLL.

1. Launch Dusklight, open **Mods** and check that **Luau Support** is listed.
   Enable it if disabled. If missing, check the installed package version;
   the public 2.0.3.54 package includes it.
2. Obtain **Play as Dark Link 1.0.0** by **LoadingError** from the author's
   listing in the built-in browser/catalog. Its ID is `com.loadingerror.dink`
   and its runtime requirement is `dev.twilitrealm.luau@1.0`. The separate
   native Dark Link boss mod is not interchangeable with it.
3. If the browser installs it successfully, skip manual copying. If it reports
   the known permission error, obtain the author's `.dusk` download on your
   other device, stop Dusklight and upload that intact file through Device
   Portal to `<active data folder>\mods\com.loadingerror.dink.dusk`.
   Create `mods` if missing. Do not unzip the package or replace Luau's DLL.
4. Relaunch, open **Mods**, enable **Play as Dark Link** if necessary and check
   for dependency/error messages. Restart if requested by the UI, then load
   your game to check the appearance. Our previous test confirmed activation,
   not correct appearance throughout gameplay.

Use the same process for other asset/Luau `.dusk` packages, checking each
mod's dependencies. It does not work for downloaded native DLL packages.
A source-code ZIP from GitHub's **Code → Download ZIP** is not an installable
`.dusk` or MSIX.

### 3. TPDE+ and other loose texture packs

1. Obtain the pack from its author. For the tested TPDE+ release, use
   <https://gamebanana.com/mods/714018> and follow the author's multi-archive
   assembly order and selection instructions. Do not copy compressed archives
   or desktop ReShade into the texture folder.
2. With Dusklight stopped, copy the selected extracted DDS/PNG files beneath
   `<active data folder>\texture_replacements\TPDE-plus-2.1\`, preserving
   the texture filenames. For other packs, use a separate pack-name folder.
   The tested TPDE+ selection occupies about 7.76 GB; allow additional space.
3. For large transfers to a configured D: data folder, connect a compatible
   file manager to `smb://<xbox-ip>/DevelopmentFiles` using your Xbox remote
   access credentials. A Windows file manager can use
   `\\<xbox-ip>\DevelopmentFiles`. Copy into the matching active data folder.
4. Launch, enable **Enable Texture Replacements** in graphics settings, then
   start the game. If troubleshooting memory use or performance, stop the app
   and move the pack folder out of `texture_replacements`, retaining a backup.

Texture registration at startup is confirmed for the tested selection;
complete visual coverage and sustained gameplay performance are not.

### 4. Randomizer: source fork, no public Xbox installer yet

The [Randomizer Xbox branch](https://github.com/999sian/dusklight-randomizer/tree/xbox-one-s-v2.0.3)
contains source fixes. **It cannot currently be added to public package
2.0.3.54 by copying a `.dusk` or DLL into `mods`.** The earlier successful
initialization used a UWP DLL bundled inside a signed private app package.
Randomizer is excluded from the public build and its contents checks.

For developers preparing a separately reviewed compatible package:

1. Use this Dusklight Xbox branch and Randomizer source commit `6bd8000` from
   the linked fork, rather than the upstream desktop build instructions.
2. Integrate Randomizer into the host CMake build and build it against the
   same game's import library, SDK, UWP toolchain and runtime. The earlier
   integration is recorded in host commit
   [`27dde3b`](https://github.com/999sian/dusklight/commit/27dde3be76eed6451916a441632a57812b23e577).
3. A compatible package must stage the extracted bundle at
   `mods/dev.twilitrealm.randomizer/` **inside the MSIX**, including its
   `mod.json`, `lib/windows-amd64/mod.dll` and required resources, before
   signing. It also needs the game's embedded symbol manifest. Copying only
   the DLL into app data cannot substitute for this step.
4. Resolve redistribution permission for the bundled textures before
   publishing such a package. The current public workflow deliberately
   rejects Randomizer; it is not a ready-made Randomizer build workflow.
5. Back up app data, install a compatible signed package in place, then check
   **Mods** for initialization/errors. Seed generation, starting a new
   randomized game and save/load still need console testing; menu activation
   alone does not establish a working playthrough.

This is the required integration path, not a currently validated end-user
build recipe. There is no supported Randomizer install step for the public
release until a compatible package is available.

### 5. If a mod fails

Stop Dusklight and move the newly added `.dusk` or texture folder out of its
search folder while retaining a backup, then relaunch. Do not uninstall or
clear app data. Record the app version, mod version and full error message
when reporting a problem; remove credentials from logs. An error from a
bundled native mod requires a corrected app package rather than deleting
files from the read-only installation.

Cosmetics has no public Xbox package here pending redistribution terms.
Current Twilit Essentials and Dawnlight are not ported; their desktop build
instructions will not make them run on this UWP app.

## Earlier private-build test history

The target is the existing online browser, downloads and broad mod support.
Compatibility is still experimental until checked on the console.

| Mod type | Current approach | Verification still needed |
| --- | --- | --- |
| Asset packs and texture/model mods | Dark Link 1.0.0 activates via Luau after direct Device Portal installation | Browser installation permission failure; in-game appearance and texture memory use |
| Luau scripts | UWP runtime starts and executes a script plus update callback on Xbox package 48 | Individual script APIs and mods; browser installation |
| Bundled native mods | Cosmetics, Randomizer and Luau all initialize on Xbox package 49 | Individual mod features and sustained gameplay |
| Downloaded native mods | Rejected by the UWP loader until a supported LocalState loading route exists | Xbox API/CRT compatibility; packaging compatible DLLs with the app |
| Mods using HTTP | Windows.Web.Http transport through existing Borealis services | Xbox browser/download and mod requests |
| Mods requiring WebSocket | Backend remains unavailable | A separate UWP WebSocket port is required |

## Dark Link texture/model package

`Play as Dark Link` 1.0.0 (`com.loadingerror.dink`, LoadingError) is now installed
on Xbox package 2.0.3.49 through Device Portal. This is the appearance mod, not
`hopex.dark_link`, the separate native-code boss mod. The official package has
no native DLL and uses `dev.twilitrealm.luau@1.0` for texture/model controls.

The browser attempt left an 8,293,385-byte staged package. Its SHA-256 matches
the official catalog: `ea6817e57facc84230c4e1fa970effb0df58ecb5e296ac4f27124331c18c4bf9`.
That verifies download/staging integrity; the user reports a permission error
at installation but declined a retry/full-text capture. The exact failing API
is not established. Review found physical path resolution and atomic file
replacement in the remaining install path; neither is proven responsible.
Do not weaken staging containment or file replacement guarantees on that basis.

After a fresh backup (`before-v2-20261021-230955`), the exact verified package
was copied to `mods/com.loadingerror.dink.dusk` without overwriting another mod.
Xbox logs report `runtime activate succeeded` for the mod and **4/4 mods active**.
The process remained running through the 30-second startup check and rendered
the menu. Existing config values and achievements/UI/save files matched the
backup after launch. The mod remains installed; the original staged file is
retained as evidence. No reinstall/uninstall or cache clearing was performed.

Evidence: `v49-dark-link-installed.log` and `v49-dark-link-installed.png` in the
local Xbox diagnostics folder. Initialization does not establish correct texture
appearance, model swaps, HD-texture toggles or sustained gameplay. The browser
installation permission failure remains unresolved; direct installation bypasses it.

## TP Definitive Edition+: internal-storage installation

**Update, 22 October:** after expanding Developer Mode storage to 49 GB, the
full 4,788-file / 7,764,942,214-byte selection transferred via SMB to
`D:\DevelopmentFiles\DusklightData\texture_replacements\TPDE-plus-2.1`.
All remote filenames/sizes and three sampled SHA256 hashes matched. The existing
custom data-folder feature now uses D: for config/saves/mods/textures; the ISO,
logs and compiled caches remain at their original U: paths. Original U: data
was retained, and both original/copied saved-file hashes passed before/after
the bounded startup check. See the progress document for backup and rollback.

Startup stayed running through 60 seconds with **4,730 texture registrations**
and **4/4 mods active**, reaching the menu while shaders compiled. One warning
reports an unaligned 41×64 BC texture thumbnail. All transferred files becoming
usable textures, appearance during gameplay, browser installation permissions,
peak memory and sustained FPS remain unverified. Evidence:
`v49-tpde-internal.log`, `.png` and `-verification.json` in local diagnostics.

### Earlier U: transfer failure (superseded)

The four official archives for <https://gamebanana.com/mods/714018> downloaded
and passed their published MD5 checksums. The page advertises 2.1.2; the actual
published archives are named 2.1. All four extracted successfully.

Following the author's assembly order, the default selection contains **4,788
textures, 7.76 GB**. Ten duplicate basenames resolve to the first file in author
folder order. The prepared layout excludes inactive alternates, optional water
layer removal and desktop ReShade, and preserves original DDS/PNG bytes.
The local SHA-256/source manifest is at
`/home/sian/Downloads/TPDE-plus-2.1/xbox-install-manifest.json`.

Transfer into the new isolated `texture_replacements/TPDE-plus-2.1` folder
failed after the first batches. A single-file diagnostic upload returned HTTP
500, code **-2147024784 (0x80070070)**, with `There is not enough space on the
disk` / `File move failed`. App-data free space could not be queried beforehand;
the deployment share's capacity is a different volume and is not applicable.

All **143 partial-transfer files** were removed, with a post-cleanup listing
confirming only `installation-info.txt` remains in that U: folder. No TPDE+
textures were active at that point; the later D: installation supersedes this
state. All verified originals remain on the host.
The existing ISO, saves, settings, Dark Link and shader caches were preserved.
Do not clear unrelated app data to make room. The full pack requires additional
available console storage, or a separately chosen smaller texture selection.
No pack runtime, in-game appearance or sustained memory-use result is claimed.
After rollback, the normal menu rendered and the process remained running at
30 seconds with **4/4 mods active**, including Dark Link. All five settings/save
file hashes matched backup `before-v2-20261021-232204`. Evidence:
`v49-after-tpde-rollback.log`, `v49-after-tpde-rollback.png` and
`v49-after-tpde-file-check.json` in the local diagnostics folder.

## Twilit Essentials 2.8.4: current-version port requirements

The user wants the current release only. The official catalog publishes 2.8.4,
and its 18,219,979-byte package was verified against SHA-256
`721f63c64719d57c29479adff37b98801ce945f8127dcb0788bc24c64fe08e1f`.
Its source link, <https://github.com/F1mmel/dusklight-twilit-essentials>,
returns not found. No current public source was found in this investigation.

The newest public source found is **1.2.1**, on
<https://github.com/kamilink7/twilit-essentials-patch/tree/no-zitems>, commit
`e60a3e463e6bdba6436e03c1a9a2ab931606d1f0` (18 September 2026).
This branch disables Z-button item support; it is not the requested 2.8.4.
No older version has been installed as a substitute.

Static PE comparison of the released Windows x64 DLL against package 49 finds
**1,095 imports from dusklight.exe, of which 889 are absent from the executable's
PE export table**. This does not mean those functions are absent from the game:
the embedded hook symbol manifest is separate from Windows DLL import resolution.
Adding exports alone would also leave desktop USER32/SHELL32/process-enumeration
and CRT dependencies to resolve, along with ABI and mod-service compatibility.
The existing UWP loader accepts bundled native DLLs through LoadPackagedLibrary;
it rejects downloaded native DLLs in LocalState.

The practical route is a UWP rebuild of **current 2.8.4**, supplied by the author
or built from its current source: port unsupported desktop calls, link against
the Xbox game/mod interfaces, then bundle it in the signed MSIX and test its
features on console. An Xbox-compatible binary supplied by the author is also
worth evaluating without public source. Adapting the desktop binary through
reverse engineering would be substantially larger, with uncertain correctness;
no working adapter, author contact, or current-version Xbox compatibility is
claimed. The author also warns of incomplete Randomizer compatibility.

## Checks completed

Package 49 now contains the embedded game symbol manifest. Xbox logs load
239,095 symbols and report all three native mods initialized, **3/3 active**.
No unresolved-hook warning appears in the captured startup log; the menu stays
running at 30 seconds. This supersedes the package 48 missing-manifest limitation
below. It does not prove all Cosmetic/Randomizer features or browser downloads.


Package 48 fixes MSVC metadata retention: all three bundled UWP DLLs pass
metadata validation on Xbox. Luau and Randomizer initialize; Cosmetics still
fails with unresolved named hooks because the package lacks its symbol manifest.
A temporary Luau script logs both startup and first-update success and remains
running for 30 seconds. The test package is removed and configuration restored.
This does not establish Randomizer hook correctness or full gameplay behavior.
Package 49 subsequently passed signed-layout symbol embedding and Xbox
initialization checks as recorded above. A further launch remained running
through 86.52 seconds at the menu; Play/controller behavior remains untested.


Package 46 isolation narrows the startup exit to directory-backed mod discovery:
a plain folder containing only an asset manifest also reproduces it. Archived
`.dusk` overrides stay running, including archives holding the original DLLs
(which are safely rejected by the LocalState loading policy). This supersedes
the earlier assumption that DLL loading itself had been isolated. Build 47
(`39660091944`, source `b5be9f8`) tests lexical bundle-relative paths instead of
physical filesystem resolution. Package 47 later passed the exact folder-only
reproducer; packages 48/49 verified script/native initialization as recorded above.


Windows HTTP workflow run `39606283522` passed all 22 tests, including live HTTPS
GET and HEAD requests to the official mod catalog, request cancellation and
expired deadlines, plaintext rejection, download validation and file handling.
The live download test also passed byte-range resume, stale-validator replacement
and completion handling against Hylian Shield 1.0.0. The Randomizer
endian self-check passed under MSVC with deprecations treated as errors, and
under GCC locally. This does not prove Xbox runtime behavior.

On Xbox package 42, with bundled native DLLs temporarily bypassed for isolation,
the update service logged `Checked 3 installed mods for updates` on two launches.
This confirms its HTTP request path on the console. Browser UI navigation and
an in-app mod download/install have not yet been exercised. Each bundled native
mod alone reproduced a startup exit in that earlier build; later isolation
identified directory enumeration as the cause, fixed and verified in package 47.
Temporary overrides were removed after testing.

Hylian Shield 1.0.0 was also uploaded temporarily through Device Portal during
the native-isolation test. The Xbox loader logged both discovery and activation,
and the menu stayed running for 30 seconds. This checks the actual `.dusk`
asset-package loading path, not the shield's appearance in gameplay or the
browser's installer UI. Its test file and all overrides were removed, and the
original configuration restored afterward.

The catalog's Windows x64 query returned 68 entries on 21 October 2026:
22 without native code and 46 with native code. These are published platform
claims, not Xbox compatibility results.

As a static example, the most-downloaded native package, Twilit Essentials
2.8.4, was downloaded locally and its advertised size and SHA-256 verified.
Its Windows x64 DLL imports desktop APIs including `EnumWindows`,
`GetWindowTextW`, `ShellExecuteA` and toolhelp process/module enumeration, plus
desktop CRT DLLs. This is evidence that selecting `windows-amd64` alone does
not establish UWP compatibility. It was not executed or installed on Xbox.
Some such mods will need an Xbox-compatible build rather than a loader change.

Keep the game's ABI/service validation and download integrity checks. Do not
replace unsupported APIs with silent success stubs to claim compatibility.
Preserve settings, saves, ISO and caches during in-place app updates.

For browser download testing, Hylian Shield 1.0.0 is a small asset-only example
(499,508 bytes). Its published size/SHA-256 and archive manifest were checked
locally; it contains no native libraries. The server supports byte ranges with
a strong ETag, so the Windows HTTP workflow now uses it for the existing live
resume/replacement test, which passed on Windows. Xbox asset loading passed
through Device Portal as recorded above; browser installation remains pending.

## Recovery detail

The loader opens native DLLs during discovery to read their service metadata,
before applying the enabled setting. Disabling a mod therefore does not prevent
its DLL or static initializers from loading. For a startup failure caused by a
downloaded native mod, stop the app and move that mod out of its search folder
while retaining a backup. Bundled package files are read-only and would require
a corrected in-place package. Do not uninstall or clear LocalState for recovery.
