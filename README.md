SportsMixDecomp
[![Code Progress]][progress] [![Data Progress]][progress] [![Functions]][progress] [![Linked]][progress]
===============

[Code Progress]: https://api.decmp.dev/ethanseagull/SportsMixDecomp.svg?mode=shield&measure=code&label=Code
[Data Progress]: https://api.decmp.dev/ethanseagull/SportsMixDecomp.svg?mode=shield&measure=data&label=Data
[Functions]: https://api.decmp.dev/ethanseagull/SportsMixDecomp.svg?mode=shield&measure=functions&label=Functions
[Linked]: https://api.decmp.dev/ethanseagull/SportsMixDecomp.svg?mode=shield&measure=complete_code&label=Linked
[progress]: https://decmp.dev/ethanseagull/SportsMixDecomp

A work-in-progress decompilation of Mario Sports Mix (Wii), with the aim of readable, working C/C++ source that can be ported to other platforms.

This repository does **not** contain any game assets or assembly whatsoever. An existing copy of the game is required.

Supported versions:

- `RMKE01`: Rev 0 (USA), the default and the version all current work targets
- `RMKP01`: Rev 0 (EU), an older configuration for `main.dol` only

What is decompiled
==================

The game is `main.dol` plus nine code modules, loaded as RSO files: `SystemF`, `MenuF`, `TourF`, `BasketF`, `VolleyF`, `DodgeF`, `HockeyF`, `BossF` and `DebugF`. decomp-toolkit cannot read RSO, so `tools/rso2rel.py` converts each module to REL; the build relinks the RELs and checks them against the converted originals (code, data and relocations). Packing them back into RSO is not done yet.

Game code (the `msw::` and `bcs::` classes) comes first. Nintendo's SDK, NW4R and the C runtime in `main.dol` are last, as a port replaces them.

Each function ends up in one of three states:

- **matched**: objdiff reports it 100% identical to the original.
- **near**: 85% or more, and every remaining difference is one that cannot change behaviour (register choice, instruction order, stack slots). These are listed in `config/RMKE01/<Module>/equivalent.txt` with the reason.
- **unverified**: written and compiling, but further off; listed the same way and waiting on a behaviour test.

A source file (unit) is `Matching` in its module's `objects.txt` when it rebuilds byte for byte and passes the hash check, and `Equivalent` when all its functions are matched or near. The **Linked** badge counts `Matching` units only.

Dependencies
============

Windows
--------

On Windows, it's **highly recommended** to use native tooling. WSL or msys2 are **not** required.  
When running under WSL, [objdiff](#diffing) is unable to get filesystem notifications for automatic rebuilds.

- Install [Python](https://www.python.org/downloads/) and add it to `%PATH%`.
  - Also available from the [Windows Store](https://apps.microsoft.com/store/detail/python-311/9NRWMJP3717K).
- Download [ninja](https://github.com/ninja-build/ninja/releases) and add it to `%PATH%`.
  - Quick install via pip: `pip install ninja`

macOS
------

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages):

  ```sh
  brew install ninja
  ```

- Install [wine-crossover](https://github.com/Gcenx/homebrew-wine):

  ```sh
  brew install --cask --no-quarantine gcenx/wine/wine-crossover
  ```

After OS upgrades, if macOS complains about `Wine Crossover.app` being unverified, you can unquarantine it using:

```sh
sudo xattr -rd com.apple.quarantine '/Applications/Wine Crossover.app'
```

Linux
------

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).
- For non-x86(_64) platforms: Install wine from your package manager.
  - For x86(_64), [wibo](https://github.com/decompals/wibo), a minimal 32-bit Windows binary wrapper, will be automatically downloaded and used.

Building
========

- Clone the repository.

- Copy your own disc image to `orig/RMKE01/disc/` (USA). Supported formats: ISO (GCM), RVZ, WIA, WBFS, CISO, NFS, GCZ, TGC.

- Unpack the files the build needs:

  ```sh
  python tools/setup_orig.py
  ```

  This extracts `main.dol` and the symbol file, decompresses the nine RSO code modules, and converts them to REL so decomp-toolkit can split and relink them.

- Configure:

  ```sh
  python configure.py
  ```

  The default version is `RMKE01` (USA). `RMKP01` (EU) has an older config for `main.dol` only.

- Build:

  ```sh
  ninja
  ```

  The build ends by checking the hash of every binary built from `Matching` units.

  On Windows, use a native Python and ninja: the MSYS ones that come with devkitPro (if it is on `PATH`) break this toolchain. `py -m ninja` works after `pip install ninja`.

Diffing
=======

Once the initial build succeeds, an `objdiff.json` should exist in the project root.

Download the latest release from [encounter/objdiff](https://github.com/encounter/objdiff). Under project settings, set `Project directory`. The configuration should be loaded automatically.

Select an object from the left sidebar to begin diffing. Changes to the project will rebuild automatically: changes to source files, headers, `configure.py`, `splits.txt` or `symbols.txt`.

![](assets/objdiff.png)

Progress
========

Progress is tracked on [decmp.dev](https://decmp.dev/ethanseagull/SportsMixDecomp). The game files cannot be on CI, so CI does not build: after a verified local build, `python tools/decomp/publish_report.py` copies the objdiff report to `progress/RMKE01/report.json` (names, sizes and match percentages only), and the `progress` workflow uploads it on push.

Helper scripts
==============

`tools/decomp/` holds the scripts used day to day, among them:

- `status.py`: progress totals, the work queue, or one unit's functions and scores
- `next.py`: the next thing to work on
- `draft.py`, `ghidra_draft.py`: rough C drafts of a function (m2c, Ghidra) to start from
- `similar.py`, `copies.py`, `port_unit.py`: find matched copies of a function or unit (the sports modules share a lot of code)
- `layout.py`: a class's size, bases, vtable and fields, gathered from every function that uses it
- `equivalent.py`: record a function as near or unverified, with the reason
- `mark.py`: set a unit's status

License
=======

[CC0 1.0](LICENSE). This covers the source in this repository only, not the game.
