# Xbox mod setup

Install the [Xbox app](https://github.com/999sian/dusklight/releases) first.
Back up your saves and close Dusklight before copying files.

## Find your data folder

In Device Portal, open Dusklight's app data:
`LocalState\TwilitRealm\Dusklight`.

If you already set a custom data folder, use that instead.
All paths below start inside this folder.

## Luau Support

Already included in Xbox release 2.0.3.54. Open **Mods** and enable
**Luau Support** if disabled. No separate download needed.

[Source](https://github.com/999sian/dusklight/tree/xbox-one-s-uwp-v2.0.3/mods/luau_runtime)

## Dark Link and other asset/Luau mods

1. Download the author's `.dusk` file. For **Play as Dark Link**, choose
   version **1.0.0 by LoadingError**.
2. Close Dusklight. Use Device Portal to copy the file into `mods/`.
   Create the folder if missing. Leave the `.dusk` file zipped.
3. Launch Dusklight → **Mods** → enable the mod. Dark Link also needs
   **Luau Support** enabled.
4. Load your game to try it.

Use this method if the browser gives a permission error; that error is still
unresolved. Dark Link activated in testing, but its in-game appearance is unverified.
Downloaded native DLL mods cannot use this method.

## Texture packs

1. Download and extract the author's pack. For
   [TPDE+](https://gamebanana.com/mods/714018), follow its archive assembly instructions.
2. Copy the selected DDS/PNG files into `texture_replacements/<pack-name>/`.
   Keep their filenames. The tested TPDE+ selection needs about **7.76 GB**.
3. Launch Dusklight → graphics settings → enable **Enable Texture Replacements**.

For large transfers, use the [internal-storage guide](../README.md#internal-storage-and-large-transfers).
TPDE+ loaded textures at startup; full gameplay and performance checks are pending.

## Randomizer

**Not available to install in the public Xbox release yet.**
Our [Xbox fork](https://github.com/999sian/dusklight-randomizer/tree/xbox-one-s-v2.0.3)
contains build fixes, but needs to be included in a signed Xbox app package.
Copying its `.dusk` or DLL into `mods/` will not work. Publishing is pending
bundled texture rights review; gameplay is unverified.

## Other mods

- **Cosmetics:** excluded pending redistribution terms.
- **Twilit Essentials / Dawnlight:** no working Xbox port here yet.
- **GitHub source ZIPs:** source code, not installable mods.

## If a mod crashes

Close Dusklight, move the new mod or texture folder out, then relaunch.
Keep a backup. **Do not uninstall the app or delete your saves.**

[Detailed test history](xbox-one-s-mod-test-history.md)
