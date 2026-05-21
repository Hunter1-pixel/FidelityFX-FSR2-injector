# Universal Upscaling Injector Roadmap

The repository now has a terminal injector executable, but universal upscaling needs a second component: a graphics bridge DLL.

## Implemented

- `tools/fsr2_injector`: terminal process launcher/attacher.
- `LoadLibraryW` DLL injection for same-architecture Windows processes.
- Suspended launch mode so the bridge can load before the target starts rendering.
- Process listing, dry-run validation, and basic architecture checks.
- `tools/reshade_bridge`: ReShade add-on bridge scaffold.
- `tools/reshade_bridge/shaders/Fsr1SpatialUpscale.fx`: first generic spatial upscale/sharpen effect.

## Next bridge DLL milestones

1. Build the ReShade add-on against the current ReShade headers.
2. Test forced swap-chain sizing with safe windowed apps.
3. Improve the spatial effect toward a fuller FSR1 EASU + RCAS implementation.
4. Add native D3D11/D3D12 compute passes when the ReShade API gives enough resource access.
5. Add a Vulkan implicit layer path for Vulkan apps that need lower-level control.
6. Add optional per-game adapters that can provide real FSR2 inputs such as motion vectors and depth.
7. Add an allowlist/blocklist and refuse known protected multiplayer/anti-cheat processes.

## Why FSR2 cannot be blindly forced everywhere

FSR2 is not just a final-image scaler. Its quality comes from temporal reconstruction, and that reconstruction depends on data the application normally owns internally. Without depth and motion vectors, a bridge can still upscale the final frame, but it cannot reliably recover disocclusion, camera motion, transparency, particle reactivity, or jitter history.

That means the practical universal design is hybrid:

- spatial upscale for generic apps and unsupported games,
- full or near-full FSR2 only for titles where a bridge can obtain the required render data,
- explicit user control so protected or unstable targets are not touched.
