# Gedouble-Core

**The Android integration core for the Gedouble launcher and Noahh.**

Gedouble is a companion launcher and management interface for the Noahh Geometry Dash mod loader. Noahh remains responsible for game modification, hooks, mod execution, and its own runtime. Gedouble does not reimplement or replace those systems.

## Targets

- Android companion application.
- Initial game target: Geometry Dash 2.2081 (Noahh compatibility must be confirmed separately).
- Android ABIs: `armeabi-v7a` and `arm64-v8a`.
- Native component: C++20, built with CMake and Android NDK.

## Responsibilities

- Inspect Noahh mod metadata using Noahh's `mod.json` format.
- Enumerate, install, and remove `.noahh` package files in a directory selected/provided by the Android app.
- Provide integration helpers for the Gedouble Android launcher.
- Leave game launching, hooks, mod loading, and runtime behavior to Noahh.

## Noahh compatibility

Noahh's Android documentation identifies this shared-storage mods directory:

```text
Android/media/com.noahh.launcher/game/noahh/mods/
```

The app must obtain filesystem access through Android's supported storage APIs and pass an accessible directory path to the native core. Do not assume that a hard-coded path is accessible under Android scoped-storage rules.

Noahh packages use the `.noahh` extension. This core can enumerate package files, copy a selected `.noahh` package into the mods directory without overwriting an existing file, and remove a package by its exact filename. It does not unpack or execute packages, and it does not change Noahh's internal enable/disable state. The in-game loader remains responsible for execution.

Noahh mod projects use `mod.json`, with metadata including `noahh`, `gd.android`, `id`, `name`, `version`, and `developer` or `developers`. The metadata reader supports these fields for a project or unpacked mod folder.

## Public headers

- `native/include/gedouble/core.hpp` — runtime lifecycle and logging.
- `native/include/gedouble/noahh.hpp` — Noahh metadata and package discovery.

## Build

GitHub Actions builds the native library for `armeabi-v7a` and `arm64-v8a` using the Android NDK.

## Related projects

- [Gedouble launcher](https://github.com/Prism-GD/Gedouble)
- [Gedouble-SDK](https://github.com/Prism-GD/Gedouble-SDK)
- [Noahh website](https://www.noahh-sdk.org/)
- [Noahh documentation](https://docs.noahh-sdk.org/)

## Legal and licensing

Noahh is an independent open-source project. Gedouble is a separate companion project and is not affiliated with or endorsed by the Noahh team or RobTop Games. Respect Noahh's license and third-party licenses when integrating or redistributing any components.

Geometry Dash is proprietary software. Gedouble does not include or distribute the game or its assets.

A license for Gedouble-Core has not yet been selected. Do not assume it is licensed for reuse or redistribution.
