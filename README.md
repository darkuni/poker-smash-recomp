# Poker Smash Recompiled (private)

Static recompilation of **Poker Smash** (Xbox 360 XBLA, Void Star Creations, 2008; Title ID 5841089C, TU 0.0.1.4) to native Windows, using the [ReXGlue SDK](https://github.com/rexglue/rexglue-sdk) v0.9.0.

**Private repo.** `generated/` is the game's own PowerPC code machine-translated to C++. No game data is committed - you supply your own dump.

## Getting a build
GitHub Actions builds on every push to `main` (or run **Build Windows** by hand from the Actions tab). Download the `poker_smash-win-amd64` artifact from the run, unzip it, and put your extracted game files in an `assets` folder next to `poker_smash.exe`:

```
poker_smash.exe
*.dll
assets\
  default.xex
  file_bundle.bin
  loading_bundle.bin
  Data\...
```

## Layout
| Path | What |
|---|---|
| `poker_smash_manifest.toml` | ReXGlue project manifest (codegen input) |
| `poker_smash_overrides.toml` | Manual function boundaries + jump table fixes |
| `generated/` | Recompiled C++ output of `rexglue codegen` (17,517 functions) |
| `src/poker_smash_app.h` | App class - defaults the game folder to `assets\` next to the exe |
| `src/usbcam_stubs.cpp` | Xbox Live Vision camera imports (SDK 0.9.0 doesn't build its own) |
| `.github/workflows/build-windows.yml` | CI: LLVM + SDK (cached) + game, uploads exe + DLLs |

## Regenerating code
Needs `default.xex` at `assets/default.xex` and the `rexglue` CLI:
```
rexglue codegen poker_smash_manifest.toml
```

## Codegen fixes so far
- `0x8211D620`, `0x821B32F8`, `0x821CC480`: C++ EH catch funclets that end in a noreturn call (`bl 0x8221FE20`); analyzer ran into the next funclet. Ends pinned.
- Jump table at `bctr 0x824C5538` (byte table @ `0x82082E98`, no bounds check): analyzer read 56 entries, real count 44.

Not affiliated with or endorsed by Void Star Creations or Microsoft.
