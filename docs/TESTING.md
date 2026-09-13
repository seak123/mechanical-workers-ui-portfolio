# Verification and limits

[Overview](../README.md) · [Source-grounded code tour](CODE_TOUR.md) · [Reference appendix](REFERENCE_APPENDIX.md)

## Historical implementation

The main case was checked against the actual production-line and head-UI interfaces and their call relationships. The tour omits implementation bodies. It does not provide a complete native module, run the original editor or establish historical test results.

## Portable reference checks

The independently written reference has **45 C++ checks** and **25 Lua checks**, plus a synthetic work trace and an authoring dry run. They exercise the reference contracts, not the original game executable.

On Windows, from an x64 Native Tools Command Prompt for Visual Studio:

```bat
build-msvc.cmd
```

With Lua 5.3+, from the repository root:

```sh
lua tests/lua_tests.lua
lua tools/author_profile.lua Content/WorkerProfiles/gatherer.lua
```

A CMake alternative is provided, but is not part of the recorded local verification:

```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Reference checks cover transitions, payload changes, priority, stale snapshots and callback lifetimes, as well as profile validation and fake-service panel behaviour. Some are additional safeguards introduced by the reference, not historical mechanisms.

## Not established by these checks

- Unreal object lifetime, real delegate teardown and asset loading.
- Navigation failures or the full ability/animation pipeline.
- Actual roster request validation, replication or production accounting.
- World-space visibility, localisation layout or complete widget bindings.
- Device FPS, CPU/GPU cost, memory reduction or production-load benchmarks.

No synthetic counter or passing reference test is presented as a measured result from the game.
