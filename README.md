# Essential++ for SDR++

**Essential++ V40** is a community-developed Windows x64 plugin that adds an LCD-inspired receiver control bar and advanced mouse, keyboard, bookmark, and preset workflows to SDR++.

It is an independent extension, not an official SDR++ module.

## Features

### Receiver toolbar

- Large LCD-style tuned and center-frequency displays, with configurable colour themes.
- Frequency entry with Hz, kHz, MHz and GHz units.
- Back/forward tuning history and quick frequency recall.
- Modulation and filter-bandwidth controls, tuning-step selection, volume/mute, and play/stop controls.
- Source/device, sample-rate, gain/AGC and server IP/port controls where supported by the connected SDR device and SDR++ build.
- Receiver monitoring with S-meter, RF/audio indicators, upload/download information, and two clocks with configurable UTC offsets.

### Spectrum and waterfall

- Mouse-wheel zoom around the pointed frequency, plus keyboard zoom controls.
- Fine filter-bandwidth adjustment using the mouse wheel over the selected filter area (100 Hz per wheel step).
- Tuning modes and a green preview marker, with optional marker information display.
- Clickable bookmark labels and contextual right-click actions on the spectrum and waterfall.
- Middle-click to copy frequency and modulation to the system clipboard (for example, `4625kHz USB`). When clicking inside the selected VFO's grey filter region, the tuned frequency is copied; elsewhere, the frequency under the cursor is copied.
- When dismissing an open top-bar dropdown by clicking the spectrum or waterfall, the first click closes the list without changing frequency.

### Bookmarks and quick access

- Add, edit, or remove Frequency Manager bookmarks using compact movable dialogs and a contextual right-click menu.
- Select a bookmark category/list, name, frequency, mode, and filter bandwidth in the bookmark editor.
- Contextual recognition of an existing bookmark on the spectrum and waterfall when the tuned frequency matches the bookmark.
- Ten quick-access slots (`0`–`9`) for frequency, modulation, and filter bandwidth; clear a slot with confirmation.
- Empty quick-access slots default to `NFM` mode and `0 Hz` bandwidth.
- Separate storage for presets and Essential++ application settings.

### Appearance and preferences

- LCD colour maps: Green, Blue, Red, Orange, and Magenta.
- Adjustable side-panel visibility (shown, hover-to-show, or hidden), plus zoom, waterfall speed, and contrast controls.
- Two configurable clocks and saved interface preferences.
- Four languages for **Essential++ itself**: English (default), Deutsch, Español, and Polski.
- Language changes are applied without restarting and saved in `essential_settings.ini`.
- The Polish translation intentionally uses **unaccented text** (for example, `Czestotliwosc`), allowing it to display with the stock SDR++ font atlas. German and Spanish accented characters are retained.

**Scope of translation:** only Essential++ interface strings are localized. Native SDR++ menus, Frequency Manager controls owned by SDR++, and third-party plugins are not translated.

## Compatibility

The precompiled `essential_plus_plus.dll` targets the **SDR++ 1.3.2 Windows x64 nightly build from 18 November 2026** used during development. The plugin relies on internal SDR++ and Dear ImGui interfaces and a build-specific bridge to Frequency Manager. **Compatibility with other builds is not guaranteed.**

## Installation

1. Exit SDR++ completely.
2. Back up your current `modules/essential_plus_plus.dll`, `essential_settings.ini`, `essential_frequency_shortcuts.ini`, and `frequency_manager_config.json`, if present.
3. Copy `essential_plus_plus.dll` into SDR++'s `modules` directory.
4. Start SDR++ and enable the Essential module if needed.
5. Configure the interface in the **Essential** panel (including **Language** directly below **Colour map**).

## Configuration

- `essential_settings.ini` contains preferences such as language, panels, and display settings.
- `essential_frequency_shortcuts.ini` contains the ten quick-access presets.
- Frequency Manager manages its bookmarks in its own native configuration file.

The plugin does not require a connected account or external network service to display its interface. Network receiver controls connect only when configured for your receiver/server.

## Build from source

The complete source is in [`Source/`](Source/). This is a narrowly targeted MSVC x64 ABI bridge using LLVM's freestanding build rather than a general-purpose plugin SDK. Requirements: Python 3, LLVM `clang` and `lld-link`.

```sh
python Source/build.py --out build
```

The compiled plugin is generated at `build/essential_plus_plus.dll`. Build and verify it against the **exact target SDR++ nightly** and its `sdrpp_core.dll` exports. The prebuilt binary is not intended to run against arbitrary SDR++ releases.

## Verification and limitations

The project has undergone compile-time, source-level, and import-compatibility checks. The actual GUI behavior must be tested on Windows with the supported SDR++ build, especially popup dismissal, bookmark/category editing, keyboard shortcuts, mouse gestures, and language rendering. Do not confuse successful compilation with full end-to-end validation.

## License

Essential++ source is released under **GPL-3.0-only**. See [`Source/LICENSE`](Source/LICENSE). SDR++ itself is an independent upstream project: <https://github.com/AlexandreRouma/SDRPlusPlus>.
