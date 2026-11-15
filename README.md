# Gedouble-Core

**The native C++ runtime and mod-loader foundation for Gedouble.**

Gedouble-Core is an independent Android native component intended to provide the runtime foundation for loading and managing Geometry Dash mods.

## Targets

- Android only.
- Initial target: Geometry Dash 2.2081.
- ABIs: `armeabi-v7a` and `arm64-v8a`.
- Primary language: C++20.
- Build system: CMake + Android NDK.

## Implemented foundation

- Runtime initialization and shutdown.
- Thread-safe runtime state.
- Host-provided logging callback.
- Native module registration and duplicate-ID rejection.
- Module load/unload callbacks.
- Reverse-order module unloading during runtime shutdown.
- Android NDK CI build for both target ABIs.

This is an initial runtime foundation, **not yet a Geometry Dash injector or a complete mod loader**. It does not currently discover mod packages, parse manifests, resolve dependencies, install game hooks, or integrate with the game's process.

## Public headers

- `native/include/gedouble/core.hpp` — runtime lifecycle and logging.
- `native/include/gedouble/module.hpp` — native module lifecycle API.

## Build

The GitHub Actions workflow builds both Android ABIs with the Android NDK. For a local build with an installed Android NDK:

```sh
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-23
cmake --build build --config Release
```

Use `armeabi-v7a` for the 32-bit build.

## Related repositories

- [Gedouble launcher](https://github.com/Prism-GD/Gedouble)
- [Gedouble-SDK](https://github.com/Prism-GD/Gedouble-SDK)

## Legal

Geometry Dash is proprietary software. This project does not include or distribute the game or its assets. Any compatibility work must respect applicable law and third-party licenses. The project is independent and is not affiliated with or endorsed by RobTop Games.

## License

A project license has not yet been selected. Until one is added, do not assume that the repository is licensed for reuse or redistribution.
