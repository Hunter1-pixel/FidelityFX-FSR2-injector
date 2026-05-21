# ReShade Bridge

This folder adds the first practical upscaler path for the project.

It uses ReShade as the graphics bridge and starts with an FSR1-style spatial upscale/sharpen path. That is the best first step for unsupported apps because it only needs the presented color frame. Full FSR2 still needs motion vectors, depth, jitter, exposure, and frame timing from the game engine.

## Pieces

- `fsr2_reshade_bridge.cpp`  
  ReShade add-on scaffold. It can force a larger swap-chain size from config and logs frame/present events.

- `shaders/Fsr1SpatialUpscale.fx`  
  ReShade effect shader that applies a lightweight spatial sample and RCAS-style sharpening pass.

## Build The Add-On

Install or clone ReShade with add-on headers, then pass the ReShade `include` folder to CMake:

```powershell
cmake -S tools\reshade_bridge -B build\reshade_bridge -DRESHADE_INCLUDE_DIR=C:\path\to\reshade\include
cmake --build build\reshade_bridge --config Release
```

The output file is:

```text
build\reshade_bridge\bin\fsr2_reshade_bridge.addon64
```

## Install Into An App

1. Install the **ReShade add-on support** build for the target app.
2. Copy `fsr2_reshade_bridge.addon64` next to the app executable, beside the ReShade DLL.
3. Copy `shaders/Fsr1SpatialUpscale.fx` into the app's `reshade-shaders\Shaders` folder.
4. Start the app and enable `FSR1SpatialUpscale` in the ReShade overlay.

## Config

The add-on reads these values from `ReShade.ini`:

```ini
[FSR2Injector]
Enabled=1
Scale=1.5
TargetWidth=0
TargetHeight=0
```

`TargetWidth` and `TargetHeight` take priority. If both are `0`, `Scale` is applied to the app's requested swap-chain size.

## Current Limits

This is not full FSR2 yet. It is a generic spatial upscaler/sharpener path that can run on unsupported apps through ReShade. Some apps may ignore or dislike forced swap-chain sizing, so start with windowed/borderless test apps before trying games.
