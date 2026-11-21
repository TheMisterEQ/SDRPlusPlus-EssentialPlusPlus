# Essential++ V40 for SDR++

A compact frequency-oriented interface extension for SDR++ 1.3.2 (Windows x64).
It started as an LCD frequency display and mouse-wheel zoom extension and now
includes VFO controls, bookmarks, presets 0-9, history, customization and a
four-language interface.

> **Compatibility:** The supplied DLL is built for the specific SDR++ 1.3.2
> Windows x64 **nightly from 18 November 2026** used for development. It uses
> internal module and Dear ImGui interfaces, including a version-specific
> Frequency Manager bridge. It is **not** a generic drop-in binary for other
> SDR++ releases. Test the exact build before using it for regular reception.

## What's new in V40

1. **Safe dropdown dismissal.** When a top-bar dropdown is open, the first
   left click on the spectrum or waterfall dismisses it without retuning. A
   subsequent click tunes as normal.
2. **Compact windows.** Quick access is about 280 px wide; the bookmark add
   and edit window is about 285 px wide (at UI scale 1). PPM context menu
   remains independently sized.
3. **Middle-click frequency copy.** Middle-click the waterfall or spectrum to
   copy `<frequency>kHz <mode>`, for example `4625kHz USB`. On the **grey
   selected VFO passband** it copies the *selected tuned frequency*, not the
   point under the cursor. Elsewhere it copies the position under the cursor,
   snapped to the selected VFO step. Sidebars and floating windows are excluded.
   USB/LSB asymmetric filter boundaries are respected. Neither copying nor
   closing a dropdown retunes the receiver.
4. **Language selection.** Essential settings now has **Language** directly
   below **Colour map**: English (default), Deutsch, Español, Polski. The choice
   is saved to `essential_settings.ini` as `Language=0..3`, applied without
   restarting, and existing V37/V38/V39 settings continue to load.
5. **Polish UI without diacritics.** The Polish translation uses plain ASCII
   (e.g. `Czestotliwosc`, `Polacz`, `Zakladki`) to display correctly in an
   unmodified SDR++. German and Spanish localized characters are preserved.

Factory SDR++ menus and native Frequency Manager controls are **not translated**.
Existing Essential++ functions from V39 are preserved, including the bookmark
right-click logic on spectrum and waterfall and NFM/0 Hz defaults for empty
quick access slots.

## Install (Windows x64)

1. **Close SDR++.** Back up your existing `modules/essential_plus_plus.dll`
   and the `essential_settings.ini`, `essential_frequency_shortcuts.ini` and
   `frequency_manager_config.json` files in your SDR++ configuration folder.
2. Copy `essential_plus_plus.dll` from this archive to SDR++ `modules/`,
   replacing the existing module.
3. Start SDR++ and enable Essential++ as usual. Select the language in the
   Essential settings panel, directly below Colour map.
4. Verify the dropdown click, middle-click clipboard text, and both dialogs.

The new language is stored in `essential_settings.ini`; presets continue to
use the separate `essential_frequency_shortcuts.ini` file.

## Language / font compatibility

This release deliberately renders the **Polski** interface without Polish
accented characters (e.g. `Czestotliwosc`, `Szerokosc`, `Polacz`, `Zakladki`).
No SDR++ executable/core changes or font patches are required for these ASCII
Polish translations. The **Deutsch** and **Español** translations retain their
accented characters. English remains the default language.

Factory SDR++ controls (including native Frequency Manager labels) are not
translated by Essential++. Language selection is persisted as before.

## Source / build

Source is in `Source/`. The project intentionally uses a narrowly targeted
MSVC x64 ABI bridge and an LLVM freestanding build, not a general-purpose
SDR++ plugin SDK. Python 3 and LLVM's `clang` and `lld-link` are required.

```sh
python Source/build.py --out build
```

The generated binary is `build/essential_plus_plus.dll`. The build script
creates import libraries from the declared target exports, so no Windows SDK
or Visual Studio install is necessary. Its output is intended only for the
supported SDR++ nightly. Do not redistribute the `build/` directory.

### Planned pre-release validation on Windows

- Check screenshot/UI text in EN/DE/ES/PL (Polish should contain no diacritics).
- Open all top-bar dropdowns and click away on **both** spectrum and waterfall:
  first click closes, next tunes.
- MMB inside grey filter copies the selected Hz; outside copies cursor Hz.
- Check zoom, filter wheel, bookmark PPM detection at 225/226 kHz,
  editing/deletion, quick presets, Priyom V2.2 co-existence.
- Restart SDR++ and verify saved language, Speed, presets and layout.

**Not tested by running SDR++ on Windows in the build environment.** A clean
compilation and source-level checks are not an end-to-end GUI test.

## GitHub release preparation

Suggested repository: `SDRPlusPlus-EssentialPlusPlus`. Add screenshots and a
precise supported-nightly link once tested; publish this archive as the
V40 Windows x64 pre-release asset. Do not publish private local INI files,
server IPs, logs or radio recordings. If you use an automated CI build,
retain the same LLVM command and verify binary imports against the exact
supported SDR++ `sdrpp_core.dll`.

### License and attribution

Essential++ source is distributed under **GPL-3.0-only** in line with
[SDR++](https://github.com/AlexandreRouma/SDRPlusPlus) (GPL-3.0). See
`Source/LICENSE`. Preserve the original upstream SDR++ copyright and license
notices where applicable. Essential++ is an independent community extension,
not an official SDR++ release.