# FidelityFX FSR2 Injector Prototype

This is a fork of AMD FidelityFX Super Resolution 2 with an added Windows terminal injector prototype.

The goal is to explore upscaling for apps and games that do not natively support FSR. The current code is an early foundation, not a finished universal upscaler yet.

## What Works Now

- `fsr2injector` can list running processes.
- It can launch an app and inject a DLL.
- It can attach to a running process by PID and inject a DLL.
- It checks for basic 32-bit/64-bit architecture mismatches.
- `fsr2_reshade_bridge` adds a ReShade add-on bridge scaffold.
- `Fsr1SpatialUpscale.fx` adds a first spatial upscaling/sharpening effect for generic apps.

The injector code is here:

[tools/fsr2_injector](tools/fsr2_injector)

The ReShade bridge and upscaler are here:

[tools/reshade_bridge](tools/reshade_bridge)

## What Does Not Work Yet

The project now includes a ReShade bridge scaffold and a first spatial effect, but it is not full FSR2 for every app yet.

FSR2 normally needs data from the game engine, including motion vectors, depth, exposure, jitter, and frame timing. Apps that do not support FSR2 do not usually expose that data, so a universal injector needs a separate graphics hook layer.

The realistic path is:

1. Use ReShade add-on support as the graphics bridge.
2. Run `Fsr1SpatialUpscale.fx` for generic spatial upscale/sharpen.
3. Use the bridge add-on to experiment with forced swap-chain sizing.
4. Add fuller FSR2 support only when enough game data is available.

More detail is in:

[docs/injector-roadmap.md](docs/injector-roadmap.md)

## Build The Injector

You need Visual Studio with the C++ desktop workload and CMake tools installed.

This was tested with Visual Studio 2022 Build Tools and CMake 4.3.2.

From a Visual Studio Developer PowerShell, run:

```powershell
cmake -S tools\fsr2_injector -B build\fsr2_injector_standalone
cmake --build build\fsr2_injector_standalone --config Release
```

The standalone build writes the executable to:

```text
build\fsr2_injector_standalone\bin\fsr2injectord.exe
```

If `cmake` is not found after installing it, restart Windows or open a fresh Visual Studio Developer PowerShell.

## Build The ReShade Bridge

The bridge needs ReShade's add-on headers. Clone or download ReShade, then pass its `include` folder to CMake:

```powershell
cmake -S tools\reshade_bridge -B build\reshade_bridge -DRESHADE_INCLUDE_DIR=C:\path\to\reshade\include
cmake --build build\reshade_bridge --config Release
```

Install ReShade's add-on support build into the target app, copy the built `.addon64` file next to the app executable, and copy:

```text
tools\reshade_bridge\shaders\Fsr1SpatialUpscale.fx
```

into the app's `reshade-shaders\Shaders` folder.

## Usage

List processes:

```powershell
fsr2injector.exe --list
```

Inject into a running process:

```powershell
fsr2injector.exe --dll C:\path\to\fsr2_bridge.dll --pid 1234
```

Launch an app and inject:

```powershell
fsr2injector.exe --dll C:\path\to\fsr2_bridge.dll --exe C:\path\to\app.exe -- --app-args
```

Dry run without injecting:

```powershell
fsr2injector.exe --dry-run --dll C:\path\to\fsr2_bridge.dll --pid 1234
```

## Safety

Only inject into software you own or have permission to modify.

Do not use this with anti-cheat protected games, competitive multiplayer games, DRM-protected apps, or anything where DLL injection violates the app's rules.

## Original FSR2 Code

The original AMD FSR2 API and sample code are still included in this repository under `src`.

This fork keeps AMD's original license terms. See [LICENSE.txt](LICENSE.txt).
