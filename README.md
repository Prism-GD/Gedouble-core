# Gedouble-Core

**The native C++ runtime and mod loader for Gedouble.**

Gedouble-Core is an independent Android native component intended to provide the runtime foundation for loading and managing Geometry Dash mods.

## Targets

- Android only.
- Initial target: Geometry Dash 2.2081.
- ABIs: `armeabi-v7a` and `arm64-v8a`.
- Primary language: C++.
- Assembly: optional and limited to cases where low-level integration genuinely requires it.
- Build system: CMake + Android NDK.

## Planned components

- Native library entry point and Android/JNI bridge.
- Runtime and game-version detection.
- Compatibility layer for supported game builds.
- Hooking and symbol-resolution abstractions.
- Mod discovery, dependency metadata, and lifecycle management.
- Logging, diagnostics, and crash-safe failure handling.
- Integration contract with Gedouble-SDK.

## Development status

This repository starts with a minimal native library and CI build. **No game hooks, injection, mod loading, or Geometry Dash integration are implemented yet.** Those features require careful reverse engineering, device testing, and version-specific validation.

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

Geometry Dash is proprietary software. This project does not include or distribute the game or its assets. Any compatibility work must respect applicable law and third-party licenses.

## License

A project license has not yet been selected. Until one is added, do not assume that the repository is licensed for reuse or redistribution.
