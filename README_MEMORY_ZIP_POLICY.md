# Recovery and release archive policy

## Preserve the playable baseline

The project creator reports that V1 plays the complete game and includes remade credits and the modern intro. The project is in final polish. Preserve that owner-tested baseline; missing repository uploads or an unfinished replacement build do not downgrade the existing game to a prototype.

## Produce a genuine updated game build

Every new game release must contain a newly compiled full G1Awakening.exe and its exact matching source, required runtime components, dependency notices and checksums. G1OpeningPlayer.exe is a separate cinematic component and must never be presented as the full game. Do not rename or repackage an older executable and describe it as newly compiled.

Recover and reconcile later keyboard, settings, menu, credit and intro changes before replacing the established baseline. Keep older source snapshots and executables as clearly identified recovery references. Do not overwrite owner saves, settings or backups.

## Verification and packaging

Record the exact source revision, input identities, compiler version, build command, executable hash and tests for each release. Distinguish owner-reported full-game testing of V1 from independent tests of a newly compiled update. A successful component build is not a full-game release test.

Before publishing any archive, verify safe relative paths, unique case-insensitive names, extraction limits, ZIP CRCs and per-file SHA-256 hashes. Include README_MEMORY_ZIP_POLICY.md. Exclude credentials, user saves, caches and unrelated project material.

## Trust records

Track trusted, trustedbuild and trustedupdate per identified artifact, not as blanket labels for the entire project. Do not inherit successful V1 results automatically for a changed executable, and do not reset V1's recorded owner validation merely because a new build is pending.

Current repository contents include the Windows-compiled cinematic helper. The updated full-game executable has not yet been compiled or published by this release-preparation pass. No global failed-game status is implied.
