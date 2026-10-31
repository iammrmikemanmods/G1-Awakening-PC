# Recovery and archive policy

Keep preserved source snapshots and compiled candidates separate from newer work. Never relabel an earlier executable as a freshly compiled release. Include exact source/binary hashes, build evidence, dependency notices and validation scope with every distributable archive. Verify safe relative paths, unique case-insensitive names, extraction limits, per-file SHA-256 hashes and ZIP CRCs before publishing. Never include credentials, user saves, caches or unrelated project material.

The opening-helper workflow produces a component archive only, not the full game. Runtime/source matching and actual Windows gameplay/intro validation remain required for a game release.

Current release trust: trusted=false; trustedbuild=false; trustedupdate=false. These describe the complete game, not individual unit-test results.
