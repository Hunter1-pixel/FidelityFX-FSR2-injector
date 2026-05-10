# FidelityFX FSR2 Injector Prototype

This is a fork of AMD FidelityFX Super Resolution 2 with an added Windows terminal injector prototype.

The goal is to explore upscaling for apps and games that do not natively support FSR. The current code is an early foundation, not a finished universal upscaler yet.

## What Works Now

- `fsr2injector` can list running processes.
- It can launch an app and inject a DLL.
- It can attach to a running process by PID and inject a DLL.
- It checks for basic 32-bit/64-bit architecture mismatches.

The injector code is here:

[tools/fsr2_injector](tools/fsr2_injector)

## What Does Not Work Yet

The project does not yet include the graphics bridge DLL that performs upscaling.

FSR2 normally needs data from the game engine, including motion vectors, depth, exposure, jitter, and frame timing. Apps that do not support FSR2 do not usually expose that data, so a universal injector needs a separate graphics hook layer.

The realistic path is:

1. Use the injector to load a bridge DLL.
2. Hook DirectX/Vulkan presentation.
3. Capture the app frame.
4. Start with spatial upscaling/sharpening for generic apps.
5. Add fuller FSR2 support only when enough game data is available.

More detail is in:

[docs/injector-roadmap.md](docs/injector-roadmap.md)

## Build The Injector

You need Visual Studio with the C++ desktop workload and CMake tools installed.

From a Visual Studio Developer PowerShell:

```powershell
cmake -S tools\fsr2_injector -B build\fsr2_injector_standalone
cmake --build build\fsr2_injector_standalone --config Release
```

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
