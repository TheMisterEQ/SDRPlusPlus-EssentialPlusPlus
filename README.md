# Essential++ for SDR++

**Essential++ V40** is a community-developed extension for **SDR++ 1.3.2 on Windows x64**. It adds a comprehensive LCD-style control interface, fine tuning, customizable spectrum and waterfall controls, receiver/source management, signal monitoring, bookmarks, ten quick presets, keyboard and mouse shortcuts, and four interface languages.

**Documentation:** [English](README.md) · [Deutsch](README.de.md) · [Español](README.es.md) · [Polski](README.pl.md)

> **Compatibility:** The supplied DLL is made for the **SDR++ 1.3.2 Windows x64 nightly from 18 November 2026**. It relies on specific internal SDR++ and Dear ImGui interfaces, as well as an exact-build Frequency Manager integration. Other SDR++ versions are **not guaranteed to work**. This is an independent community project, not an official SDR++ module.

## Complete feature list

### LCD-style receiver toolbar

- Large, segmented, colour-themed **tuned-frequency display**; original SDR++ frequency selection and digit editing remain usable. Center-frequency and receiver information are available through the enhanced interface.
- **Back** and **Forward** buttons for receiver history, preserving frequency, demodulation mode, and filter width.
- **Modulation selector** with NFM, WFM, AM, LSB, USB, DSB, CW, and RAW. The modulation field can also be changed with the mouse wheel.
- **Filter-bandwidth readout/editor**: click to enter the exact bandwidth in Hz; wheel changes it in 100 Hz increments. Invalid values are rejected.
- **Tuning-step readout/editor**: click to enter a step in whole Hz; wheel increases/decreases it by 100 Hz.
- **Tuning-mode selector** using the original toolbar tuning icon: **Free Tuning**, **Center**, and **Free Fixed**. Selecting a mode does not itself retune the receiver.
- Native **Play/Stop** and **Mute** buttons with explanatory tooltips. `Space` toggles playback and `M` toggles the selected audio stream's mute state.
- **Volume slider** with current percentage shown on hover; **manual receiver gain** slider with current gain on hover.
- **Tuner AGC/AGC** and **RTL AGC** controls where supported by the selected SDR source; unavailable controls remain disabled. Manual gain is disabled when applicable AGC is active.
- **Two independent clocks**, each showing HH:MM:SS and a selectable fixed offset from **UTC−12 to UTC+14**. Click either clock to select its offset; no automatic daylight-saving adjustment.
- **RF and AUDIO needle meters**, with dBFS readings where available; the RF meter follows the red tuned marker, while the audio meter monitors the selected receiver's audio output. The original SDR++ SNR meter remains available.
- **Contextual tooltips** for controls and live values; responsive layout adapts to available toolbar width.
- Five LCD colour schemes: **Green, Blue, Red, Orange, Magenta**. Network connection-state warnings retain their distinct status colours.

### Source, devices and network receivers

- Display selected local SDR source/device information in the toolbar.
- **Device selector** underneath the sample-rate selector; change compatible receivers even while streaming, with automatic stop/switch/resume when required.
- **Sample-rate (MSPS) selector** in the toolbar, for sources exposing compatible sample-rate options; stream lifecycle and visible spectrum zoom are handled across changes.
- For supported remote receivers exposing **native Connect and Disconnect controls**: a **server host/IP and TCP port** editor, connection/status feedback and transfer-rate indicators (**DL/UL**).
- **Connect opens only the network connection; it does not start IQ streaming.** **Play/Stop** controls reception separately; **Disconnect** closes the connection rather than merely stopping the stream. If a source does not expose separate native connection controls, Essential++ does not fake them through Play/Stop.
- Changing the endpoint can reconnect the client; an existing stream is stopped for a required reconnect, but playback does not start automatically.
- Source gain, AGC, sample-rate and device controls operate through the source's supported native interfaces; available options depend on SDR hardware, driver and module.

### Spectrum, waterfall and markers

- **Ctrl + mouse wheel** over the spectrum, waterfall or frequency scale zooms about the pointed frequency. **Ctrl + ↑/↓** zooms about the selected receiver frequency.
- **Mouse wheel inside the selected grey receive passband** adjusts demodulator filter bandwidth by **100 Hz per wheel notch**, without making the waterfall disappear.
- Click empty spectrum/waterfall space to tune to the pointed frequency; drag the background to pan in supported tuning modes. The native filter-edge handles still resize bandwidth.
- Drag anywhere in the selected **grey filter/passband** to move the tuned VFO; an unmoved click inside it does not retune. Device snap settings are respected.
- Three tuning modes: **Free Tuning** (free pan/tune), **Center** (native centered tuning), and **Free Fixed** (keep the red marker at its selected frequency while panning, until tuning at an edge becomes necessary).
- **Green frequency-preview marker** on hover; press and release **Alt alone** to cycle **hidden → text information → text plus analog RF meter**. The display mode is saved.
- Preview tooltip may show frequency, RF power in dBFS, demodulation, filter width and receiver gain (if available). The preview is suppressed over interactive controls and during dragging/resizing.
- A **left click outside an open top-bar dropdown** first dismisses the dropdown **without retuning**. Click again to tune normally.
- **Middle mouse button (MMB)** over spectrum or waterfall copies a string such as `4625kHz USB` to the Windows clipboard: inside the selected grey filter it uses the **currently tuned** frequency; outside it uses the **frequency under the pointer**. Modulation is appended; no tuning or marker movement occurs.
- **Right mouse button (RMB)** opens a contextual menu on spectrum and waterfall for quick presets and Frequency Manager bookmarks. Context actions recognize the grey passband and bookmark labels appropriately.
- **Clickable yellow bookmark labels** tune to their saved frequency. Bookmark detection for editing/deleting requires the matching bookmark/carrier or a direct bookmark-label hit; merely overlapping another bookmark's passband is insufficient.

### Bookmarks and quick presets

- Add, edit and delete **Frequency Manager bookmarks** using the spectrum/waterfall RMB menu, including category/list, name, frequency, bandwidth and modulation. Uses the native Frequency Manager data where supported.
- Compact movable bookmark **Add** and **Edit** windows, category before name, and confirmation for overwrites/deletions; dialogs preserve their position during use.
- **Ten quick-access presets, 0–9**, each storing **frequency (Hz), modulation and filter bandwidth**.
- **Hold a number in the upper keyboard row for at least 0.6 seconds** to recall its preset. A short press **does not recall** the preset; it changes modulation (see shortcut table).
- Edit all ten slots in the Essential sidebar table: frequency, modulation and filter width. Click an **occupied slot number** to delete it after confirmation.
- Add/remove a quick preset at the selected frequency through RMB context actions. Select a slot and confirm before overwriting an occupied one.
- Deleting a slot clears **frequency to 0 Hz, modulation to NFM, bandwidth to 0 Hz**; a zero bandwidth means the demodulator's native default when recalling a preset.
- Presets are saved separately from UI preferences, and can be used again after restarting SDR++.

## All keyboard shortcuts

Shortcuts operate when SDR++ is the focused window, Essential++ is enabled, and a text input or modal dialog is not consuming the keys. A selected Radio VFO/source is needed for the relevant receiver controls. **Top-row digits and numeric-keypad digits have different functions.**

| Key / gesture | Action |
|---|---|
| `←` / `→` | Tune −1 / +1 kHz |
| `Shift+←` / `Shift+→` | Tune −10 / +10 kHz |
| `Alt+←` / `Alt+→` | Tune −100 / +100 Hz |
| `Ctrl+←` / `Ctrl+→` | Tune −100 / +100 kHz |
| `↑` / `↓` | Increase / decrease receive filter bandwidth by 100 Hz |
| `Shift+↑` / `Shift+↓` | Increase / decrease supported tuner gain by one source step |
| `Alt+↑` / `Alt+↓` | Select next / previous supported SDR sample rate |
| `Ctrl+↑` / `Ctrl+↓` | Zoom in / out around the selected VFO |
| `Ctrl` **alone** (press/release) | One-time center on the selected VFO, preserving subsequent tuning mode and zoom |
| `Alt` **alone** (press/release) | Cycle green-marker information: hidden / text / text + RF meter |
| `Ctrl+Z` | Back one receiver-history state (frequency, mode and filter); forward is available on the toolbar |
| `Space` | Play / Stop |
| `M` | Mute / unmute the selected audio stream |
| `` ` `` / `~` (key below Esc) | Show / hide the SDR++ left sidebar |
| **Short** top-row `1` | NFM |
| **Short** top-row `2` | WFM |
| **Short** top-row `3` | AM |
| **Short** top-row `4` | LSB |
| **Short** top-row `5` | USB |
| **Short** top-row `6` | DSB |
| **Short** top-row `7` | CW |
| **Short** top-row `8` | RAW |
| **Short** top-row `9` | Previous modulation in the eight-mode sequence |
| **Short** top-row `0` | Next modulation in the eight-mode sequence |
| **Hold** top-row `0`–`9` (≥0.6 s) | Recall the corresponding frequency/mode/filter preset; do not apply the short-press mode action |
| Numpad `0`–`9` | Enter frequency digits in the on-screen frequency-entry panel (not a preset recall) |
| Numpad `.` / decimal | Enter a decimal point in frequency entry |
| `Enter` / Numpad `Enter` | Confirm the entered frequency and tune |
| `Backspace` (during entry) | Delete the previous digit |
| `Esc` (during entry) | Cancel frequency entry |
| Numpad `+` / `−` | Tune +1 / −1 kHz |
| Numpad `*` / `/` | Tune +100 / −100 Hz |

### Direct frequency entry

Press numpad digits to open the large LCD-style entry panel. Choose **Hz, kHz, MHz or GHz** from its unit dropdown; the last selected unit is remembered (the initial default is **MHz**). Decimal fractions are accepted only when the result is an exact whole number of Hz. Example: `4.625` in MHz becomes **4,625,000 Hz**. Confirm with Enter/Numpad Enter, delete with Backspace, or cancel with Esc. Maximum accepted frequency: **999,999,999,999 Hz**. The panel does not interrupt editing other text fields.

## Appearance and settings

- Essential sidebar: **Left panel** and **Right panel**, independently set to **Show**, **Show on hover**, or **Hide**. Hide modes increase available waterfall width.
- Left panel exposes the native signal-level scale; right panel supplies **Zoom**, **Waterfall Speed** (FFT/update rate) and **Contrast**. These controls are live and saved where applicable.
- **Colour map** selection: Green, Blue, Red, Orange or Magenta.
- **Language** immediately below Colour map: **English** (default), **Deutsch**, **Español**, **Polski**. Changes are immediate and persist across sessions.
- Only **Essential++ strings** are translated: factory SDR++ menus, native third-party plugin UIs and unmodified portions of Frequency Manager remain in their own language.
- The **Polish Essential++ interface intentionally omits Polish diacritics** (e.g. `Czestotliwosc`) because the stock SDR++ font atlas does not include all necessary glyphs. **These GitHub documentation files use normal Unicode spelling.** German and Spanish accents are retained in the plugin.
- Persisted preferences include language, LCD scheme, panel modes, clocks, frequency-entry unit, speed, contrast, marker information and tuning mode.

## Installation

1. Quit SDR++ completely.
2. Back up an existing `modules/essential_plus_plus.dll` and configuration files if replacing a previous installation.
3. Copy the repository's [`essential_plus_plus.dll`](essential_plus_plus.dll) to your SDR++ `modules` directory.
4. Start the compatible SDR++ nightly and enable **Essential++** if required.
5. Open the **Essential** sidebar section to set panel behaviour, colour map, language and presets.

## Configuration and source code

- `essential_settings.ini` — language, colour map, clock zones, panel visibility, view controls and other UI preferences.
- `essential_frequency_shortcuts.ini` — ten frequency/mode/filter presets.
- Frequency Manager retains its **own** native bookmark configuration.
- [`Source/`](Source/) contains the complete C++ source and build script. Requirements: **Python 3, LLVM `clang` and `lld-link`**, with the exact matching Windows x64 SDR++ binary/interface.

```sh
python Source/build.py --out build
```

The resulting DLL is `build/essential_plus_plus.dll`. Source/build instructions describe a build-specific integration, **not** a stable cross-version SDR++ plugin API.

## Verification and limitations

The build has undergone source-level, compilation and binary-import compatibility checks. Some features depend on receiver capabilities, hardware support and SDR++ module availability. End-to-end GUI behaviour should be validated on the supported Windows build; a successfully built DLL alone does not establish compatibility with other releases.

## License

**GPL-3.0-only** — see [`Source/LICENSE`](Source/LICENSE). Independent upstream project: [SDR++](https://github.com/AlexandreRouma/SDRPlusPlus).
