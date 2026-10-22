# Xbox release content audit — 22 October 2026

This is a technical inventory for the experimental public Xbox release. It
does not establish legal clearance for every inherited source file, brand,
artwork or dependency. Copyrighted software can be redistributed under its
license; describing the distribution as entirely copyright-free would be wrong.

## Scope and findings

- The parent repository's tracked files and locally available history across
  all refs were checked for disc images, saves, extracted game archives and
  private-key filenames. No such added paths were found. The local disc under
  `local-game/` is ignored and has not been added to Git or release assets.
- The prior CI-built 2.0.3.53 MSIX was inspected directly: 78 entries,
  91,717,460 uncompressed bytes. It contained no disc image or personal save,
  but did include eight randomizer `.bti` textures and a static gameplay
  screenshot background. Their redistribution rights were not established.
- This release removes the screenshot from the current source tree and uses
  a CSS gradient. The randomizer submodule reference/build integration is
  removed from this release's source checkout, and the randomizer is no longer
  staged into the MSIX. Its existing local checkout is retained outside Git.
- Standard project branding remains inherited upstream artwork. This audit
  does not certify ownership of character imagery or trademarks. Dependency
  license texts do not grant rights to Nintendo game content.
- Open-source dependency and font notices are included in `res/licenses`.
  Original copyright notices are retained. See `THIRD_PARTY_NOTICES.md`.
- The public package is produced by the Windows CI workflow, using named
  build outputs. Local ISO files, Xbox data folders, saves, downloaded mods,
  texture packs, diagnostic screenshots and credentials are not release inputs.
- CI rejects game-disc/save/resource extensions, private signing keys, the
  excluded randomizer directory and the removed screenshot in the final MSIX.
  Filename checks cannot recognize every possible renamed or embedded asset.
- The release ZIP will contain the MSIX, public development certificate,
  x64 runtime dependency and documentation/notices. Private keys, symbol
  archives and personal diagnostic logs are excluded.

## History and external repositories

Removing a file from HEAD does not remove it from earlier commits or existing
Actions artifacts. This work does not rewrite public history, delete upstream
repositories or certify the assets in optional external mods. The removed
screenshot and older randomizer references remain in historical commits.
The new release does not attach those older build artifacts.

The source release includes funchook's corresponding source and pinned diStorm
dependency with the host build/patch scripts, alongside the independent-module
linking exception. License provenance URLs and the exact embedded font notices
are supplied in `res/licenses`.

## Verification record

The previous CPU package (2.0.3.53) passed Windows CI and a bounded Xbox startup
check. The public package has additional resource/notice changes; its exact CI,
content scan, hashes and hardware-test status are recorded in its release notes.
No gameplay-performance or comprehensive copyright-clearance claim is made.
