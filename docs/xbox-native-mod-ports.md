# Experimental Xbox mod ports

This work is on `xbox-mod-ports`. It is separate from public release 2.0.3.54.

| Mod | Source | Current check |
| --- | --- | --- |
| Dawnlight 5.1.0 | [Xbox fork](https://github.com/999sian/dawnlight/tree/xbox-one-s-v2.0.3) | Linux SDK build passed; UWP build pending. |
| Roll and Sprint 1.1.0 | [Xbox fork](https://github.com/999sian/Roll-and-Sprint/tree/xbox-one-s-v2.0.3) | Linux SDK build passed; UWP build pending. |
| TP Classic Modern Controller UI 1.4.0 | [Xbox fork](https://github.com/999sian/TP-CM-Controller-UI/tree/xbox-one-s-v2.0.3) | Linux SDK build passed; UWP build pending. |

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

[Initial Windows/UWP build](https://github.com/999sian/dusklight/actions/runs/39711549153)
