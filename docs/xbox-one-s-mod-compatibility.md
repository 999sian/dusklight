# Xbox One S mod compatibility

The target is the existing online browser, downloads and broad mod support.
Compatibility is still experimental until checked on the console.

| Mod type | Current approach | Verification still needed |
| --- | --- | --- |
| Asset packs | Existing `.dusk` installer and resource overlays | Download/install in Xbox browser; texture memory use |
| Luau scripts | UWP runtime starts and executes a script plus update callback on Xbox package 48 | Individual script APIs and mods; browser installation |
| Bundled native mods | Build Cosmetics, Randomizer and Luau with the game's UWP toolchain/import library | DLL loading, hooks and features on Xbox |
| Downloaded native mods | Rejected by the UWP loader until a supported LocalState loading route exists | Xbox API/CRT compatibility; packaging compatible DLLs with the app |
| Mods using HTTP | Windows.Web.Http transport through existing Borealis services | Xbox browser/download and mod requests |
| Mods requiring WebSocket | Backend remains unavailable | A separate UWP WebSocket port is required |

## Checks completed

Package 48 fixes MSVC metadata retention: all three bundled UWP DLLs pass
metadata validation on Xbox. Luau and Randomizer initialize; Cosmetics still
fails with unresolved named hooks because the package lacks its symbol manifest.
A temporary Luau script logs both startup and first-update success and remains
running for 30 seconds. The test package is removed and configuration restored.
This does not establish Randomizer hook correctness or full gameplay behavior.
Package 49 is testing symbol embedding in the signed package layout.


Package 46 isolation narrows the startup exit to directory-backed mod discovery:
a plain folder containing only an asset manifest also reproduces it. Archived
`.dusk` overrides stay running, including archives holding the original DLLs
(which are safely rejected by the LocalState loading policy). This supersedes
the earlier assumption that DLL loading itself had been isolated. Build 47
(`39660091944`, source `b5be9f8`) tests lexical bundle-relative paths instead of
physical filesystem resolution. Native loading and script activation remain
unverified until a normal launch succeeds.


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
mod alone reproduces a startup exit; the package-loader correction is pending
build/hardware verification. Temporary overrides were removed after testing.

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
resume/replacement test, which passed on Windows. Xbox installation remains pending.

## Recovery detail

The loader opens native DLLs during discovery to read their service metadata,
before applying the enabled setting. Disabling a mod therefore does not prevent
its DLL or static initializers from loading. For a startup failure caused by a
downloaded native mod, stop the app and move that mod out of its search folder
while retaining a backup. Bundled package files are read-only and would require
a corrected in-place package. Do not uninstall or clear LocalState for recovery.
