# Xbox One S mod compatibility

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
