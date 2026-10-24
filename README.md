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

## In-game settings (F1 or Esc)
| Setting | Options | Applies |
|---|---|---|
| Display mode | Windowed / Fullscreen (also F11) | instantly |
| Window size | 1280x720, 1600x900, 1920x1080, 2560x1440 | next launch |
| Render resolution | 1x (original 720p), 2x, 3x | next launch |
| VSync | On / Off | instantly |
| Aspect ratio | Keep 16:9 / Stretch to window | instantly |
| Texture filtering | Game default, Off, 2x-16x anisotropic | instantly |
| Keyboard controls | On / Off | instantly |
| Edit keyboard keys | rebind every controller button | instantly |

Settings save automatically to `poker_smash.toml` next to the exe. F3 shows the performance overlay.
The game always runs as the full (unlocked) version.

### Default keyboard layout
| Controller | Key |
|---|---|
| D-pad | Arrow keys |
| A / B / X / Y | Z / X / A / S |
| LB / RB | Q / W |
| LT / RT | E / R |
| Start / Back | Enter / Backspace |

A controller works at the same time as the keyboard.

## Layout
| Path | What |
|---|---|
| `poker_smash_manifest.toml` | ReXGlue project manifest (codegen input) |
| `poker_smash_overrides.toml` | Manual function boundaries + jump table fixes |
| `generated/` | Recompiled C++ output of `rexglue codegen` (17,517 functions) |
| `src/poker_smash_app.h` | App class - game folder default, Xenos GPU plugin, full-game license, first-launch defaults, menu/F11 binds |
| `src/game_menu.*` | F1/Esc settings menu (same layout as the ReBirth recomp menus, on ReXGlue's ImGui overlay) |
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
