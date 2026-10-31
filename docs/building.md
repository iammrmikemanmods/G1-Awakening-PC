# Matching source and builds

Download `G1_Awakening_PC_1.0_Source.zip` and `G1Awakening.exe` from the same release. The public release is 1.0; the executable and source build identity remain 1.0.1. The executable SHA-256 is:

```
3c54f46c8967d927977ba1a0d99abdd91a078b59c2fd5d5fcaed8cda99316694
```

The source archive contains `source/runtime/`, `source/launcher/`, `source/opening/`, pinned vendor source, `tools/` and `provenance/`. It deliberately excludes compiler caches, user saves and standalone font data. Original game assets/media are recovered from the exact paired EXE by a hash-checking script that does not execute it.

The recorded cross-build route is Linux/WSL with Rust 1.99.1, the `x86_64-pc-windows-gnu` target, CMake 3.31.6, Python 3, GNU Make, a native C++ compiler and MinGW x64 GCC 13.2 POSIX. Follow the source archive README and toolchain records rather than substituting the legacy cinematic-helper-only build.

```bash
rustup toolchain install 1.99.1 --profile minimal
rustup target add --toolchain 1.99.1 x86_64-pc-windows-gnu
python3 tools/prepare_assets.py /path/to/G1Awakening.exe
bash tools/build_windows.sh
```

Output: `artifacts/G1Awakening.exe`. Build paths/toolchains can affect byte identity; matching source is provided without an unsupported bit-for-bit reproducibility promise.

After the complete release is published, the source-import workflow verifies the attached source archive and makes its runtime, launcher, helper and build tools browsable in the repository. The explicitly attached matching source ZIP remains the authoritative paired snapshot.
