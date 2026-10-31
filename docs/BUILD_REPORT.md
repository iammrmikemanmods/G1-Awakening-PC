# G1 Awakening PC 1.0.1 — build and verification report

Built on 31 October 2026. **This is a new complete game executable**, with a freshly compiled runtime, launcher and cinematic helper. It is not an older EXE renamed and not a cinematic-only download.

## Exact identity

- File: `G1Awakening.exe`
- Size: 115,109,888 bytes (109.78 MiB)
- SHA-256: `3c54f46c8967d927977ba1a0d99abdd91a078b59c2fd5d5fcaed8cda99316694`
- Target: Windows x64 / AMD64 PE32+ GUI launcher.
- Compiler: Rust 1.99.1; MinGW GCC 13.2 POSIX; CMake 3.31.6.

The exact component hashes and imports are in `BUILD_VALIDATION.json`. The self-extracting package holds the game copies, runtime, supplied 3840×2160 / 60 FPS movie, opening helper and dependency notices. Existing saves remain at `%LOCALAPPDATA%\G1Awakening`.

## Included work

The menu-tested Pass09 candidate06 runtime was reconciled against its 433-file frozen manifest before building. Two existing runtime files were changed for startup readiness and input isolation; one new guard module was added. Original ARM game logic is still run by the game-specific touchHLE runtime; this is not a claim of fully native x64 game recompilation.

This update includes keyboard menu highlighting/navigation, the Help02 desktop game copy, widescreen profiles and source-based texture enhancements already present in that baseline. The new launcher integrates the supplied modern opening, a loading still, skip forwarding, exact readiness markers, timeout/error fallback, project credits and original application icon. Its extraction logic preserves the existing save/settings locations. The original-layout compatibility profile bypasses the new desktop intro handshake.

The user-reported end-to-end playable V1 remains the established game baseline. The exact updated binary above has its own checks; missing native Windows tests for this changed binary do not mean the original V1 is a prototype.

## Completed checks

**48 component checks passed:** 30 opening-state/marker/aspect-ratio checks, 7 input-guard checks and 11 launcher/extraction/options checks. The previous GNU/OpenAL TLS regression was checked against the rebuilt runtime's actual instructions, control-block pointers and import table. It passed; no external libgcc, libstdc++ or libwinpthread DLL is required.

The new EXE was executed in an isolated **Wine 9.0 x64** prefix with software graphics. It reached the title and menu, accepted ordinary keyboard navigation, loaded the saved campaign, rendered Mission 1, advanced dialogue with Enter and exited with code 0. The screenshots are real captures of that environment, not concept art or original-mobile references.

A separate executable-driven verification test deliberately changed one byte of the isolated installed runtime, then ran `--verify-only`. The runtime was repaired, all six saved-data/canary files remained byte-identical, and the verifier exited with code 0. No real user's Windows files were accessed.

The embedded package's 88 members passed CRC and exact byte/hash checks. Original-layout game files and runtime resources remained identical to the preserved payload. The original IPA and recovery archives were not overwritten. The source-package asset recovery/repacking procedure was replayed and produced the identical embedded payload.

## Boundaries and remaining Windows checks

**Actual video playback was not verified:** Wine rejected the Media Foundation player with exit code 3. The new launcher correctly released input and let the game continue. The movie and player are embedded and compiled; a real Windows check is still needed for their playback, sound and smooth handoff. `--no-intro` is available as a per-launch fallback. The original Glu/Hasbro startup graphics remain visible on the no-intro/fallback path; do not interpret this report as proof that every original startup graphic was removed.

This was not a new complete-campaign playthrough, an audible audio test, or a native Windows GPU/performance test. The 60 FPS media specification does not claim that the original game's simulation is unlocked to 60/144 FPS. A full texture remaster and later unlocated keybinding/settings changes were not invented or silently declared included.

The EXE is unsigned. Preserve the older working EXE and back up saves before replacing a build. Build trust is recorded per artifact: compilation/package evidence passed, while native Windows/full-update acceptance is not yet asserted.

## Matching source

`G1_Awakening_PC_1.0.1_Source.zip` contains the matching source, pinned dependency identities, included vendor source, tests and build scripts. Its asset-recovery script reads the exact paired EXE and verifies its SHA-256 without executing it. The source archive deliberately excludes compiler caches, user saves and standalone font binaries.
