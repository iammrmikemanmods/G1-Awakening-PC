# Transformers G1 Awakening — PC

**A fan-made PC preservation project by iammrmikeman.**

Bring the original tactical game to a desktop with keyboard navigation, widescreen rendering and a replacement opening cinematic, while retaining its original identity.

> **Development snapshot — not a finished public release.** No final game installer or current all-in-one executable has been published here. The recovered Windows rendering candidate predates the latest menu work; the new opening integration is not yet Windows-playback verified. Do not mistake the opening-player build artifact for the game.

## What this project actually is

The recovered PC implementation runs the original ARM game through a game-specific **touchHLE compatibility runtime**. It is not a native x64 recompilation of all game logic. The port includes desktop-specific rendering, controls, interface work and a launcher. The separate iOS project is not part of this repository.

## Verified progress

On 31 October 2026, the recovered Pass09 Linux runtime was launched in an isolated test environment. Its title screen, main menu and Help menu were reached using ordinary keyboard input; the process exited cleanly. Both recovered runtime-source snapshots matched all **433 recorded file hashes** each. These results do **not** establish Windows execution, working audio or a complete campaign playthrough.

The opening helper now validates the exact menu-ready marker and preserves loading-art aspect ratio. Its cross-platform contract tests cover malformed markers, early skipping, readiness, decoder/startup deadlines and viewport fitting. See `desktop/opening/` and `tests/` for the source. Its Windows build is deliberately separate from the full game.

## Build the opening helper

On Windows, install CMake and the Visual Studio C++ desktop workload/Windows SDK, then run:

```powershell
cmake -S desktop/opening -B build/opening -A x64
cmake --build build/opening --config Release
ctest --test-dir build/opening -C Release --output-on-failure
```

Output: `build/opening/Release/G1OpeningPlayer.exe`. This is **only the cinematic helper**, not `G1Awakening.exe`. It takes three arguments: the MP4 path, a loading BMP path and the per-launch menu-ready file path. Escape, Enter, Space or a left click skips the movie, but never skips required game initialization. A missing/failed decoder or startup deadline causes the helper to close, leaving the separate game process alone.

The marker must contain exactly `G1_NATIVE_MENU_VISIBLE_V1` followed by LF. A fresh unpredictable marker path is required for each launch; never reuse a stale readiness file. The launcher/runtime handshake remains an integration gate.

## Release checklist

- Recover and reconcile the later keyboard/settings source with the preserved baseline.
- Rebuild the complete Windows runtime and launcher from the exact published source.
- Verify cinematic playback, skipping, error fallback and transition to the game menu.
- Verify Windows graphics, audio, save/load, resizing, controls and campaign progression.
- Publish a matched executable/source pair, checksums, dependency notices and genuine screenshots.
- Explicitly promote a tested candidate; do not relabel an older executable as a new stable release.

## Source and downloads

The source committed here currently covers the opening helper, its tests and release documentation. The recovered full runtime source and existing Windows candidate are **not yet uploaded to this repository**. There is no complete game release to download here yet. GitHub Actions artifacts named `G1OpeningPlayer-*` contain only the helper.

## Credits

PC adaptation and project direction: **iammrmikeman**. Original game and Transformers property belong to their respective creators and rights holders. Compatibility runtime: **touchHLE and its contributors**. This is an unofficial fan project, not an official Glu or Hasbro release. Third-party licenses must remain attached to their code and binaries; project credits do not replace them.
