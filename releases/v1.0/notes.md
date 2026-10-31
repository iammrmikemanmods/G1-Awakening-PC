# G1 Awakening PC — Public Release 1.0

A faithful desktop adaptation by **iammrmikeman**, preserving the original tactical campaign with keyboard/mouse controls and high-resolution presentation.

**Public version:** 1.0. **Included executable build:** 1.0.1. This release publishes the newly compiled complete game EXE, not an older EXE renamed and not the standalone cinematic helper.

## Downloads

- **G1Awakening.exe** — standalone complete Windows x64 game package, with the runtime and supplied opening embedded.
- **G1_Awakening_PC_1.0_Windows.zip** — the same EXE, player guide, distinct real screenshots and build/verification records.
- **G1_Awakening_PC_1.0_Source.zip** — matching runtime, launcher, cinematic helper, vendor source and rebuild tools.
- **SHA256SUMS.txt** and **BUILD_REPORT.md** — exact download identities and testing details.

Extract the Windows package, run G1Awakening.exe, choose 1080p or 4K and select Play. Saves remain at `%LOCALAPPDATA%\G1Awakening`. Back up existing saves before changing versions.

## Included

Original campaign and tactics; desktop keyboard navigation and action controls; fullscreen/resizable-window modes; 1080p/4K widescreen profiles and supersampling; Classic/Enhanced texture selection; modern 4K/60 opening integration; iammrmikeman launcher credits; save-preserving runtime verification and repair.

## Testing and compatibility notes

The creator reports end-to-end playability of the established V1. This updated binary passed compilation, 48 component checks, archive integrity and isolated Wine gameplay/save-preservation checks. The screenshots are direct captures of the delivered EXE, not concept art.

Native Windows playback/audio/hardware acceptance remains separate: Wine rejected Media Foundation playback but the game fallback continued. Use `--no-intro` for a per-launch fallback; original startup graphics remain on that path. Video being 60 FPS does not mean the original simulation is unlocked to 60/144 FPS. The build is unsigned. These limits are recorded in BUILD_REPORT.md, not hidden behind a blanket claim of testing on every PC.

## Next

Faithful graphics/texture improvements and further PC polish. The established V1 is the playable baseline, not a prototype restart.

Unofficial fan adaptation. Original game/Transformers rights remain with their respective owners; touchHLE and dependency notices are retained.
