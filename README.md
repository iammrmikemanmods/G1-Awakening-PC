# Transformers G1 Awakening — PC

**A faithful desktop adaptation by iammrmikeman.**

G1 Awakening brings its original tactical campaign to PC, with desktop controls, high-resolution presentation, and numerous other improvements.

## Project status

**V1 is playable from beginning to end and the project is in its final polish stage.** The project creator reports full-game playthroughs, working remade credits and the modern opening. The remaining work is polishing the PC experience, publishing the updated Windows build and matching source, and exploring future graphical improvements—not building the game from scratch.

The playable V1 and the files currently uploaded to this repository are different things. **The updated complete game executable has not yet been published here.** Until the full-game release is uploaded, the opening-helper Actions artifact is only a component and must not be downloaded as though it were the game.

## The PC experience

- The original tactical game and campaign, preserved for desktop play.
- Keyboard and mouse controls, with a PC-focused presentation.
- High-resolution widescreen rendering.
- A modern opening cinematic and refreshed iammrmikeman credits.
- Further visual improvements planned after the core release.

Specific controls, requirements and installation instructions will accompany the exact downloadable game build. Future texture or graphical upgrades are not prerequisites for the existing playable V1.

## Downloads and source

The next publication will include a newly compiled **G1Awakening.exe**, its required runtime files, matching source, checksums and screenshots of the actual PC game. Older executable snapshots remain recovery references and will not be renamed as the new build.

At present this repository contains the cinematic helper and its tests. `G1OpeningPlayer.exe` is **not** the full game executable. The full PC source and release package are being reconciled with the playable baseline before upload.

## Screenshots

The public gallery will use genuine captures of the PC port. Concept art and original-mobile reference images are not substitutes for release screenshots.

## Building the cinematic component

This component is separate from the game. On Windows with CMake, the Visual Studio C++ desktop workload and Windows SDK:

```powershell
cmake -S desktop/opening -B build/opening -A x64
cmake --build build/opening --config Release
ctest --test-dir build/opening -C Release --output-on-failure
```

The helper compiled successfully in Windows CI. Its tests cover startup-state handling, the menu-ready marker and aspect-ratio fitting; these are component checks, not a new full-campaign playthrough. The helper supports skipping without bypassing required game initialization.

## Technical background

The recovered PC source uses a game-specific **touchHLE compatibility runtime** to run the original ARM game, alongside desktop rendering, input and launcher changes. This is a PC adaptation, not a claim that all original game logic has been recompiled into native x64 source. The separate iOS port is a different project.

## Credits

PC adaptation, project direction and presentation: **iammrmikeman**. Original game and Transformers property belong to their respective creators and rights holders. Compatibility runtime: **touchHLE and its contributors**. This is an unofficial fan project, not an official Glu or Hasbro release. Third-party license notices remain attached to their code and binaries.
