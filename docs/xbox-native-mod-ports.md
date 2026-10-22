# Experimental Xbox mod ports

This work is on `xbox-mod-ports`. It is separate from public release 2.0.3.54.

| Mod | Source | Current check |
| --- | --- | --- |
| Dawnlight 5.1.0 | [Xbox fork](https://github.com/999sian/dawnlight/tree/xbox-one-s-v2.0.3) | Windows/UWP build and separate Xbox startup test passed. |
| Roll and Sprint 1.1.0 | [Xbox fork](https://github.com/999sian/Roll-and-Sprint/tree/xbox-one-s-v2.0.3) | Windows/UWP build and separate Xbox startup test passed. |
| TP Classic Modern Controller UI 1.4.0 | [Xbox fork](https://github.com/999sian/TP-CM-Controller-UI/tree/xbox-one-s-v2.0.3) | Windows/UWP build and separate Xbox startup test passed. |

All three require bundling inside a signed MSIX. Desktop `.dusk` downloads
cannot add their native DLLs to an installed Xbox app.

The experimental workflow builds them against the same host import library,
includes their resources and licenses, and checks for four bundled native
libraries (the three ports plus Luau). It permits the pinned mods' BTI/BMD
artwork only in their resource folders. It still rejects disc images, saves,
private keys, Cosmetics and Randomizer. No new public release is published.
Dawnlight's standalone updater remains unavailable, as in upstream 5.1.0;
Xbox mod updates need a new signed app package.

Test Dawnlight and Roll and Sprint separately first: both change sprinting.
A successful build or menu launch does not establish gameplay compatibility.

[Successful Windows/UWP build](https://github.com/999sian/dusklight/actions/runs/39714675293)

Experimental package **2.0.3.1006** passed DLL import/export and archive checks.
It is installed on the test Xbox One S. Saves, settings, user mods and the data-folder
setting were hash-checked unchanged during the update; the ISO size was also checked.

## Use on Xbox

1. Install the experimental signed MSIX as an update. Do not uninstall Dusklight.
2. Open **Mods** before starting the game.
3. Enable **Dawnlight** or **Roll and Sprint**. Leave the other disabled.
4. For **TP Classic Modern Controller UI**, enable it and choose **Xbox** before starting the game. Restart Dusklight to change controller style later.
5. Start the game.

All three initialized successfully in separate 30-second Xbox One S startup tests
on package 2.0.3.1006, with no logged errors. These were menu tests; gameplay,
controller prompts in-game and combinations remain unverified. Original settings
were restored and the app left stopped. New mods may default to enabled, so check
step 3 before playing. Dawnlight reports its optional boss-rush music file
`mods/temp.ast` is absent; that music is disabled.
