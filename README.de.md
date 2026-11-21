# Essential++ für SDR++

**Essential++ V40** ist ein von der Community entwickeltes Plugin für **SDR++ 1.3.2 unter Windows x64**. Es ergänzt eine umfassende LCD-Bedienleiste, präzises Abstimmen, individuell einstellbare Spektrum- und Wasserfallansichten, Empfangsgeräteverwaltung, Pegelanzeigen, Lesezeichen, zehn Schnellwahlspeicher, Tastatur- und Mauskürzel sowie vier Oberflächensprachen.

**Dokumentation:** [English](README.md) · [Deutsch](README.de.md) · [Español](README.es.md) · [Polski](README.pl.md)

> **Kompatibilität:** Die mitgelieferte DLL wurde für den **SDR++ 1.3.2 Windows-x64-Nightly-Build vom 18. November 2026** erstellt. Sie verwendet interne SDR++-/Dear-ImGui-Schnittstellen und eine buildspezifische Integration des Frequency Manager. Andere Builds sind **nicht garantiert kompatibel**. Dies ist ein unabhängiges Community-Projekt, kein offizielles SDR++-Modul.

## Vollständige Funktionsübersicht

### LCD-Empfängerleiste

- Große segmentierte **Frequenzanzeige** im LCD-Stil mit wählbaren Farben; die originale Frequenzauswahl und Ziffernbearbeitung in SDR++ bleiben erhalten. Mittenfrequenz und Empfängerinformationen stehen in der erweiterten Oberfläche zur Verfügung.
- **Zurück**- und **Vorwärts**-Tasten für den Empfangsverlauf: Frequenz, Modulationsart und Filterbandbreite werden gemeinsam wiederhergestellt.
- **Modulationsauswahl:** NFM, WFM, AM, LSB, USB, DSB, CW und RAW; die Modulation kann auch per Mausrad geändert werden.
- **Filterbandbreite:** Anzeige, Eingabe des genauen Werts in Hz durch Anklicken und Änderung um **100 Hz** je Mausradschritt. Ungültige Eingaben werden abgewiesen.
- **Abstimmschritt:** Anzeige, direkte Eingabe in ganzen Hz und Änderung um **100 Hz** je Mausradschritt.
- **Abstimmmodus-Auswahl** über das originale Symbol in der Leiste: **Free Tuning**, **Center**, **Free Fixed**. Der Moduswechsel allein verstellt die Frequenz nicht.
- Originale **Play/Stop**- und **Mute**-Schaltflächen mit Tooltips; `Space` startet/stoppt den Empfang, `M` schaltet den gewählten Audiokanal stumm.
- **Lautstärkeregler** mit Prozentanzeige beim Darüberfahren; **manueller Gain-Regler** mit Anzeige des aktuellen Verstärkungswerts.
- **Tuner AGC/AGC** und **RTL AGC**, sofern die gewählte Quelle sie unterstützt; nicht verfügbare Optionen sind deaktiviert. Bei aktiver entsprechender AGC ist manueller Gain gesperrt.
- **Zwei unabhängige Uhren** in HH:MM:SS mit einzeln auswählbarem festem UTC-Versatz von **UTC−12 bis UTC+14**. Anklicken zur Auswahl; keine automatische Sommerzeitumstellung.
- Analoge **RF- und AUDIO-Zeigerinstrumente** mit dBFS-Werten, soweit verfügbar: RF bezieht sich auf die rote Empfangsmarkierung, AUDIO auf den Ausgang des gewählten Audiokanals. Die originale SDR++-SNR-Anzeige bleibt erhalten.
- **Kontextabhängige Tooltips** und Live-Werte; die Leiste passt sich an die verfügbare Fensterbreite an.
- Fünf LCD-Farbvarianten: **Green, Blue, Red, Orange, Magenta**. Warnfarben für Verbindungszustände bleiben unterscheidbar.

### Quellen, Geräte und Netzwerkempfänger

- Anzeige der gewählten lokalen SDR-Quelle und der Empfängerinformationen in der Leiste.
- **Geräteauswahl** unterhalb der Samplerate: Wechsel zwischen kompatiblen Empfängern auch während des Betriebs; bei Bedarf automatischer Stop/Wechsel/Fortsetzung.
- **Samplerate (MSPS)** direkt in der Leiste, sofern die Quelle kompatible Werte anbietet. Beim Wechsel werden der Empfangszyklus und der sichtbare Zoom berücksichtigt.
- Für Netzwerkquellen mit **eigenen nativen Connect-/Disconnect-Befehlen**: Editor für **Hostname/IP und TCP-Port**, Verbindungsstatus sowie Übertragungsanzeigen **DL/UL**.
- **Connect stellt ausschließlich die Netzwerkverbindung her und startet keinen IQ-Stream.** **Play/Stop** steuert den Empfang unabhängig davon; **Disconnect** trennt die Verbindung und stoppt nicht bloß den Stream. Ohne native getrennte Verbindungsbefehle simuliert Essential++ diese nicht über Play/Stop.
- Nach einer Änderung von Host/Port kann eine erneute Verbindung erforderlich sein. Ein bestehender Stream wird dabei nötigenfalls gestoppt, aber **nicht automatisch neu gestartet**.
- Gain, AGC, Samplerate und Gerätewahl verwenden die nativen Funktionen der jeweiligen Quelle. Die Verfügbarkeit hängt von Hardware, Treibern und SDR++-Modulen ab.

### Spektrum, Wasserfall und Markierungen

- **Ctrl + Mausrad** über Spektrum, Wasserfall oder Frequenzskala zoomt um die Zeigerposition. **Ctrl + ↑/↓** zoomt um die gewählte VFO-Frequenz.
- **Mausrad innerhalb des grauen Empfangsfilters** ändert die Demodulator-Filterbreite um **100 Hz je Schritt**, ohne den Wasserfall auszublenden.
- Klick auf einen freien Bereich im Spektrum/Wasserfall stimmt auf die angezeigte Frequenz ab. Ziehen des Hintergrunds verschiebt die Ansicht in den unterstützten Modi. Die originalen Filterkanten-Griffe ändern weiterhin die Bandbreite.
- Ziehen **innerhalb des gesamten grauen Empfangsfilters** verschiebt die VFO-Frequenz unter Berücksichtigung des eingestellten Schritts; ein einfacher Klick ohne Verschieben verändert sie nicht.
- Drei Abstimmmodi: **Free Tuning** (freies Abstimmen/Verschieben), **Center** (originales zentriertes Abstimmen), **Free Fixed** (rote Markierung bleibt beim Verschieben auf ihrer Frequenz, bis an der Ansichtsgrenze neu abgestimmt werden muss).
- **Grüne Vorschau-Markierung** beim Zeigen auf eine Frequenz. **Alt allein drücken und loslassen** schaltet zyklisch **ausgeblendet → Textinformationen → Text + analoges RF-Messinstrument**. Die Auswahl wird gespeichert.
- Der Vorschau-Tooltip kann Frequenz, RF-Pegel in dBFS, Modulationsart, Filterbreite und Gain (sofern verfügbar) zeigen. Beim Ziehen/Skalieren oder über Bedienelementen wird er ausgeblendet.
- Der **erste linke Klick außerhalb einer geöffneten Dropdownliste** in der oberen Leiste schließt nur diese Liste, **ohne abzustimmen**. Ein weiterer Klick kann normal abstimmen.
- **Mittlere Maustaste** auf Spektrum/Wasserfall kopiert z. B. `4625kHz USB` in die Windows-Zwischenablage: **im grauen Empfangsfilter** wird die eingestellte VFO-Frequenz, **außerhalb** die Frequenz unter dem Mauszeiger kopiert. Die Modulation wird angehängt; Marker und Abstimmung bleiben unverändert.
- **Rechte Maustaste** öffnet auf Spektrum und Wasserfall das Kontextmenü für Schnellspeicher und Frequency-Manager-Lesezeichen. Treffer auf Empfangsfilter und Lesezeichenbeschriftungen werden berücksichtigt.
- **Gelbe Lesezeichenbeschriftungen** sind anklickbar und stimmen auf die gespeicherte Frequenz ab. Bearbeiten/Löschen setzt eine Übereinstimmung von Trägerfrequenz und Lesezeichen bzw. einen direkten Klick auf das Label voraus; bloße Überlappung der Empfangsbereiche reicht nicht.

### Lesezeichen und Schnellspeicher

- **Frequency-Manager-Lesezeichen** über das Kontextmenü hinzufügen, bearbeiten und löschen; mit **Kategorie/Liste, Name, Frequenz, Filterbandbreite und Modulation**. Speicherung über den kompatiblen nativen Frequency Manager.
- Kompakte verschiebbare Dialoge für **Hinzufügen/Bearbeiten**, mit Kategorie vor Name; Bestätigungen für Überschreiben und Löschen sowie stabile Fensterpositionen.
- **Zehn Schnellwahlspeicher 0–9**, je mit **Frequenz in Hz, Modulation und Filterbandbreite**.
- **Ziffer 0–9 in der oberen Tastenreihe mindestens 0,6 Sekunden gedrückt halten**, um den Speicher abzurufen. **Kurzes Drücken ruft keinen Speicher ab**, sondern wechselt die Modulation (siehe Tastaturtabelle).
- Alle zehn Speicherplätze sind in der Essential-Seitenleiste über Frequenz-, Modulations- und Filterfelder bearbeitbar. Klick auf die **Nummer eines belegten Platzes** fragt vor dem Löschen nach Bestätigung.
- Schnellspeicher können ebenfalls über das Rechtsklickmenü erstellt und gelöscht werden. Vor dem Überschreiben eines belegten Platzes erfolgt eine Rückfrage.
- Löschen setzt den Eintrag auf **0 Hz / NFM / 0 Hz** zurück. Bei einem gespeicherten Filterwert von 0 Hz verwendet der Demodulator beim Abrufen seine voreingestellte Bandbreite.
- Speicherplätze werden getrennt von den Oberflächeneinstellungen dauerhaft gesichert.

## Alle Tastaturkürzel

Kürzel gelten, wenn SDR++ im Vordergrund und Essential++ aktiv ist und kein Textfeld oder modales Dialogfenster die Tasten verarbeitet. Für empfängerbezogene Befehle ist eine geeignete ausgewählte VFO/Quelle erforderlich. **Ziffern der oberen Tastenreihe und des Nummernblocks haben unterschiedliche Aufgaben.**

| Taste / Geste | Funktion |
|---|---|
| `←` / `→` | −1 / +1 kHz abstimmen |
| `Shift+←` / `Shift+→` | −10 / +10 kHz abstimmen |
| `Alt+←` / `Alt+→` | −100 / +100 Hz abstimmen |
| `Ctrl+←` / `Ctrl+→` | −100 / +100 kHz abstimmen |
| `↑` / `↓` | Filterbandbreite um 100 Hz erhöhen / verringern |
| `Shift+↑` / `Shift+↓` | Gain um einen unterstützten Quellenschritt erhöhen / verringern |
| `Alt+↑` / `Alt+↓` | Nächste / vorherige unterstützte SDR-Samplerate |
| `Ctrl+↑` / `Ctrl+↓` | Um die gewählte VFO-Frequenz hinein- / herauszoomen |
| **Nur** `Ctrl` (drücken/loslassen) | Einmalig auf die gewählte VFO zentrieren, ohne den Abstimmmodus oder Zoom zu ändern |
| **Nur** `Alt` (drücken/loslassen) | Grüne Markerinfo: aus / Text / Text + RF-Messinstrument |
| `Ctrl+Z` | Im Empfangsverlauf einen Schritt zurück (Frequenz, Modulation, Filter); vorwärts über die Schaltfläche in der Leiste |
| `Space` (Leertaste) | Play / Stop |
| `M` | Gewählten Audiokanal stummschalten / Stummschaltung aufheben |
| `` ` `` / `~` (Taste unter Esc) | Linke SDR++-Seitenleiste ein- / ausblenden |
| **Kurz** obere `1` | NFM |
| **Kurz** obere `2` | WFM |
| **Kurz** obere `3` | AM |
| **Kurz** obere `4` | LSB |
| **Kurz** obere `5` | USB |
| **Kurz** obere `6` | DSB |
| **Kurz** obere `7` | CW |
| **Kurz** obere `8` | RAW |
| **Kurz** obere `9` | Vorherige Modulation in der Acht-Modi-Folge |
| **Kurz** obere `0` | Nächste Modulation in der Acht-Modi-Folge |
| **Obere `0`–`9` halten** (≥0,6 s) | Zugehörigen Speicher mit Frequenz + Modulation + Filter abrufen; keine Aktion des kurzen Tastendrucks |
| Nummernblock `0`–`9` | Frequenzziffern im LCD-Eingabefenster eingeben (kein Speicherabruf) |
| Nummernblock `.` / Dezimaltrenner | Dezimalpunkt bei der Frequenzeingabe |
| `Enter` / Nummernblock `Enter` | Eingegebene Frequenz bestätigen und abstimmen |
| `Backspace` (beim Eingeben) | Vorherige Ziffer entfernen |
| `Esc` (beim Eingeben) | Frequenzeingabe abbrechen |
| Nummernblock `+` / `−` | +1 / −1 kHz abstimmen |
| Nummernblock `*` / `/` | +100 / −100 Hz abstimmen |

### Direkte Frequenzeingabe

Mit den Ziffern des Nummernblocks öffnet sich die große LCD-Eingabe. Im Dropdown stehen **Hz, kHz, MHz und GHz** zur Auswahl; die zuletzt verwendete Einheit wird gespeichert (anfangs **MHz**). Dezimalzahlen sind zulässig, sofern das Ergebnis auf volle Hz fällt, z. B. `4.625` MHz = **4.625.000 Hz**. Mit Enter/Nummernblock-Enter bestätigen, Backspace löscht, Esc bricht ab. Höchste erlaubte Frequenz: **999.999.999.999 Hz**. Die Frequenzeingabe überschreibt keine gerade aktive Textbearbeitung an anderer Stelle.

## Darstellung und Einstellungen

- In der Essential-Seitenleiste haben **Linkes Panel** und **Rechtes Panel** unabhängig die Zustände **Show**, **Show on hover** und **Hide**; ausgeblendete Panels geben dem Wasserfall mehr Platz.
- Links: native Signalpegelskala. Rechts: **Zoom**, **Speed** (FFT-/Wasserfallaktualisierung) und **Contrast**; wo unterstützt werden Werte direkt angewendet und gespeichert.
- **Colour map:** Green, Blue, Red, Orange, Magenta.
- **Language** unmittelbar darunter: **English** (Standard), **Deutsch**, **Español**, **Polski**. Wechsel ohne Neustart, Speicherung der Auswahl.
- Übersetzt werden **nur Essential++-Texte**. Die originalen SDR++-Menüs, andere Plugins und unveränderte Bereiche des Frequency Manager bleiben unübersetzt.
- In der **polnischen Plugin-Oberfläche** fehlen absichtlich polnische Sonderzeichen (z. B. `Czestotliwosc`), da der Standard-Schriftatlas von SDR++ nicht alle Glyphen enthält. **Die GitHub-Dokumentation** benutzt normale Unicode-Zeichen. Deutsche und spanische Akzente bleiben im Plugin erhalten.
- Gespeichert werden unter anderem Sprache, LCD-Farben, Panel-Modi, Zeitzonen, Eingabeeinheit, Geschwindigkeit, Kontrast, Markerinfos und Abstimmmodus.

## Installation

1. SDR++ vollständig beenden.
2. Vor dem Ersetzen eine Sicherung von `modules/essential_plus_plus.dll` und vorhandenen Konfigurationsdateien erstellen.
3. [`essential_plus_plus.dll`](essential_plus_plus.dll) in das SDR++-Verzeichnis `modules` kopieren.
4. Den kompatiblen SDR++-Nightly-Build starten und **Essential++** gegebenenfalls aktivieren.
5. In **Essential** Panels, Farben, Sprache und Schnellspeicher einstellen.

## Konfiguration und Quellcode

- `essential_settings.ini`: Sprache, Farben, Uhren, Panel-Anzeige und andere Einstellungen.
- `essential_frequency_shortcuts.ini`: zehn Schnellspeicher mit Frequenz, Modulation und Filter.
- Der Frequency Manager speichert Lesezeichen in seiner **eigenen** Konfiguration.
- [`Source/`](Source/) enthält den kompletten C++-Quellcode und das Buildskript. Erforderlich sind **Python 3, LLVM `clang`, `lld-link`** und die passende SDR++-Windows-x64-Binärschnittstelle.

```sh
python Source/build.py --out build
```

Ergebnis: `build/essential_plus_plus.dll`. Der Quellcode ist **buildspezifisch** und keine universelle Plugin-API für beliebige SDR++-Versionen.

## Überprüfung und Einschränkungen

Quellcode, Kompilierung und DLL-Importe wurden geprüft. Manche Funktionen setzen passende Geräte, Treiber und Quellmodule voraus. Die vollständige GUI-Funktion muss im vorgesehenen SDR++-Build unter Windows getestet werden; ein erfolgreich gebautes Modul garantiert keine Kompatibilität mit anderen Releases.

## Lizenz

**GPL-3.0-only** – [`Source/LICENSE`](Source/LICENSE). Unabhängiges Upstream-Projekt: [SDR++](https://github.com/AlexandreRouma/SDRPlusPlus).
