# Release archive and recovery policy

Public release 1.0 packages unchanged executable build 1.0.1. Preserve the exact EXE SHA-256 and matching source identity; never substitute the cinematic-only helper or relabel an older executable as a new compilation.

Keep an untouched previous working EXE and back up saves. Do not overwrite or remove `%LOCALAPPDATA%\G1Awakening\Runtime\touchHLE_sandbox` during installation or troubleshooting.

Verify archive CRCs, safe relative paths, case-insensitive uniqueness and per-file SHA-256 manifests before extraction/publication. No credentials, user saves, compiler caches or standalone font files belong in a public source archive. Preserve dependency license notices.

Record verification per identified artifact. The creator's V1 playthrough and this changed binary's automated checks are distinct evidence. Publication status is separate from game readiness. Do not publish an empty release as though its executable/source downloads were available.
