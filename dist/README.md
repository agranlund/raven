# dist

This is the tooling I use to build and publish the Raven release packages.

It is **not** a distribution itself. If you're looking for ready-made files, use the [Releases](https://github.com/agranlund/raven/releases) page on Github.

`make release` builds ROMs, software and firmware from the rest of this repository, assembles the release folder, zips it and uploads it to GitHub as `Raven.<date>`.

`make release-test` does everything except the upload.

It is written for my own development machine and isn't expected to run anywhere else without adjustments.


## What the build does
- Builds the ROM images and baremetal srec programs
- Builds rvbios, rvnova, rvsnd, the ISA drivers and TOS tools
- Picks up pre-built hardware outputs (gerbers, BOMs) and CPLD/keyboard-controller firmware
- Unpacks the parts needed from `3rdparty/` and patches some of them for 68060 compatibility (`tools/`)
- Zips the result and creates the GitHub release

## Contents
|||
|------|----------|
|`Makefile`|Release build and upload|
|`3rdparty/`|Third-party software bundled into the release, kept as the original archives|
|`tools/`|Small host tools that patch bundled third-party binaries for 060|
|`readme_*.txt`|Readmes that end up inside the release package|
|`release_notes.txt`|Text shown on the GitHub release page|
|`release_history.txt`|Changelog shipped with the package (`history.txt`)|
|`license.gpl`|License text shipped with the package|

Bundled third-party software keeps their own readme and license.
