# Xbox One S mod compatibility

The target is the existing online browser, downloads and broad mod support.
Compatibility is still experimental until checked on the console.

| Mod type | Current approach | Verification still needed |
| --- | --- | --- |
| Asset packs | Existing `.dusk` installer and resource overlays | Download/install in Xbox browser; texture memory use |
| Luau scripts | Bundle the Luau runtime compiled for UWP | Script activation on Xbox |
| Bundled native mods | Build Cosmetics, Randomizer and Luau with the game's UWP toolchain/import library | DLL loading, hooks and features on Xbox |
| Downloaded native mods | Existing Windows x64 platform selection and native loader | Xbox API/CRT dependencies and loading from LocalState |
| Mods using HTTP | Windows.Web.Http transport through existing Borealis services | Xbox browser/download and mod requests |
| Mods requiring WebSocket | Backend remains unavailable | A separate UWP WebSocket port is required |

## Checks completed

Windows HTTP workflow run `39604721297` passed 21 tests, including live HTTPS
GET and HEAD requests to the official mod catalog, request cancellation and
expired deadlines, plaintext rejection, download validation and file handling.
The optional live range-resume integration test was not enabled. The Randomizer
endian self-check passed under MSVC with deprecations treated as errors, and
under GCC locally. This does not prove Xbox runtime behavior.

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
