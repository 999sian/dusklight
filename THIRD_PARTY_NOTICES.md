# Third-party notices for the Xbox package

The application's CC0 dedication does not remove third-party copyrights.
Full license texts and embedded font notices are distributed in `res/licenses`
in both this source tree and the MSIX. `res/licenses/sources.json` records the
upstream locations used for the notices. Those URLs are provenance, not a
substitute for the included texts.

| Component | License / notice |
| --- | --- |
| Dusklight | CC0 1.0; `Dusklight-CC0.txt` |
| Aurora, Borealis | MIT; `Aurora-MIT.txt`, `Borealis-MIT.txt` |
| SDL3, libuwp | zlib; `SDL.txt`, `LIBUWP.txt` (libuwp refers to SDL's terms) |
| Dawn / Tint | BSD; `DAWN.txt` |
| DirectX Shader Compiler / DXIL | Microsoft redistribution terms; `DXIL.txt` |
| nod | MIT / Apache 2.0; `NOD.txt`, `NOD-APACHE.txt` |
| Abseil | Apache 2.0; `Abseil-20240722.0.txt` |
| fmt, ImGui, RmlUi, RmlUi containers, PicoSHA2, miniz | Their included MIT/BSD-style notices |
| xxHash, zstd | Included BSD notices |
| libpng, zlib | `libpng-1.6.58.txt`, `zlib-1.3.2.txt` |
| FreeType | FreeType License option; `FreeType-FTL.txt`, `FreeType-LICENSE.txt`, BDF/PCF notices |
| Tracy | BSD; `Tracy-0.14.1.txt` |
| SQLite | Public domain; `SQLite-public-domain.txt` |
| funchook | GPL 2 or later with independent-module linking exception; `funchook-1.1.3.txt` |
| diStorm (funchook dependency) | BSD; `diStorm-COPYING.txt` |
| Luau, LuaBridge3 | MIT; `Luau-0.734.txt`, `LuaBridge3.txt` |
| Alegreya SC, Fira Sans/Condensed, Inter, Noto Mono | SIL Open Font License; family texts and `Font-notices.txt` |
| Material Symbols | Apache 2.0; `MaterialSymbols-Apache.txt`, `Font-notices.txt` |

FreeType credit: Portions of this software are copyright © The FreeType
Project (https://freetype.org). All rights reserved.

The release includes a corresponding-source archive for funchook 1.1.3 and its
pinned diStorm submodule, together with the host's build integration and patch
scripts. The independent-module linking exception is reproduced in full;
do not describe the entire distribution as copyright-free or exclusively CC0.

VCLibs, when supplied alongside the package, is a Microsoft redistributable
runtime governed by Microsoft's terms. The development signing certificate
contains only the public certificate; it is not a production trust endorsement.

Upstream project branding is inherited project artwork. Nintendo retains rights
to its game content, characters and trademarks. No assertion of ownership of
those rights is made. The audit does not establish legal clearance for every
upstream dependency or piece of artwork; see `docs/release-content-audit.md`.
