# Essential++ dla SDR++

**Essential++ V40** to niezależna wtyczka dla **SDR++ 1.3.2, Windows x64**. Rozbudowuje program o panel odbiornika w stylu LCD, precyzyjne strojenie, sterowanie widmem i wodospadem, obsługę urządzeń i serwerów, mierniki, zakładki, dziesięć szybkich presetów, skróty klawiaturowe i myszowe oraz cztery języki interfejsu.

**Dokumentacja:** [English](README.md) · [Deutsch](README.de.md) · [Español](README.es.md) · [Polski](README.pl.md)

> **Zgodność:** Dostarczona DLL jest przeznaczona dla **SDR++ 1.3.2 Windows x64 Nightly z 18 listopada 2026 r.** Wtyczka korzysta z wewnętrznych interfejsów SDR++ i Dear ImGui oraz integracji z Frequency Manager zależnej od konkretnej kompilacji. **Nie gwarantujemy zgodności z innymi wydaniami SDR++.** Nie jest to oficjalny moduł SDR++.

## Pełna lista funkcji

### Panel odbiornika w stylu LCD

- Duży, segmentowy **wyświetlacz ustawionej częstotliwości** z wybraną kolorystyką; pozostają dostępne oryginalne możliwości wskazywania i edytowania cyfr SDR++. Rozbudowany interfejs udostępnia także informacje o częstotliwości środkowej i odbiorniku.
- Przyciski **Wstecz** i **Dalej** obsługujące historię odbiornika: częstotliwość, modulację oraz szerokość filtra.
- **Wybór modulacji:** NFM, WFM, AM, LSB, USB, DSB, CW i RAW. Modulację można również przełączać kółkiem myszy nad jej polem.
- **Odczyt i edycja szerokości filtra:** kliknięcie pozwala wpisać dokładną wartość w Hz; kółko zmienia ją co 100 Hz. Błędne wartości są odrzucane.
- **Odczyt i edycja kroku strojenia:** kliknięcie umożliwia wpisanie pełnych Hz; kółko zmienia krok co 100 Hz.
- **Wybór trybu strojenia** pod oryginalną ikoną paska: **Free Tuning**, **Center** i **Free Fixed**. Sama zmiana trybu nie przestraja odbiornika.
- Oryginalne przyciski **Play/Stop** i **Mute** z podpowiedziami. `Space` włącza/wyłącza odbiór, a `M` wycisza lub przywraca dźwięk wybranego strumienia.
- **Regulacja głośności**, podpowiedź z wartością procentową oraz **suwak ręcznej regulacji gain** z bieżącą wartością.
- Sterowanie **Tuner AGC/AGC** i **RTL AGC**, jeśli wybrane źródło SDR je obsługuje. Niedostępne opcje są blokowane; ręczny gain jest niedostępny przy odpowiednim aktywnym AGC.
- **Dwa niezależne zegary** HH:MM:SS; każdy ma ręcznie wybieraną strefę **UTC−12 do UTC+14**. Kliknięcie zegara otwiera listę stref. Bez automatycznej korekty czasu letniego.
- **Wskazówkowe mierniki RF i AUDIO** wraz z wartościami dBFS, gdy są dostępne. RF mierzy sygnał w punkcie czerwonego markera, AUDIO — poziom dźwięku wybranego odbiornika. Zachowany jest także fabryczny miernik SNR.
- **Podpowiedzi** z funkcjami przycisków i aktualnymi wartościami; układ paska dostosowuje się do dostępnej szerokości.
- Pięć motywów LCD: **Green, Blue, Red, Orange, Magenta**. Kolory alarmowe połączenia sieciowego zachowują swoje znaczenie.

### Źródła SDR, urządzenia i połączenia sieciowe

- Wyświetlanie informacji o wybranym lokalnym źródle/odbiorniku SDR na górnym pasku.
- **Lista odbiorników** pod listą próbkowania; przełączanie obsługiwanych urządzeń podczas odbioru, z automatycznym zatrzymaniem, zmianą i wznowieniem strumienia, jeśli jest to wymagane.
- **Wybór próbkowania (MSPS)** na pasku, gdy źródło SDR udostępnia zgodne wartości; obsługa cyklu uruchomienia strumienia i zachowanie widocznego przybliżenia widma przy zmianach.
- Dla odbiorników sieciowych udostępniających natywne **Connect/Disconnect**: edycja **IP/hosta i portu TCP**, informacje o stanie i wskaźniki transferu **DL/UL**.
- **Connect tylko zestawia połączenie, nie uruchamia strumienia IQ.** Odbiór uruchamiają osobne **Play/Stop**. **Disconnect** rzeczywiście zamyka połączenie, a nie tylko zatrzymuje strumień. Jeśli źródło nie ma osobnych natywnych przycisków połączenia, Essential++ nie zastępuje ich Play/Stop.
- Zmiana adresu może wymagać ponownego połączenia; wcześniejszy odbiór zostaje w razie potrzeby zatrzymany, ale nie uruchamia się automatycznie.
- Sterowanie gain, AGC, próbkowaniem i wyborem urządzenia odbywa się przez obsługiwane natywne interfejsy źródła. Dostępność zależy od sprzętu, sterowników i modułów.

### Widmo, wodospad i markery

- **Ctrl + kółko myszy** nad widmem, wodospadem lub skalą częstotliwości przybliża/oddala względem wskazanej częstotliwości. **Ctrl + ↑/↓** przybliża/oddala względem dostrojonego VFO.
- **Kółko myszy na szarym obszarze odbieranego filtra** zmienia jego szerokość co **100 Hz**, bez znikania wodospadu.
- Kliknięcie pustego obszaru widma/wodospadu przestraja do wskazanej częstotliwości; przeciągnięcie tła przesuwa widok w odpowiednich trybach. Natywne uchwyty krawędzi filtra nadal pozwalają zmieniać szerokość pasma.
- Przeciągnięcie **dowolnego miejsca szarego filtra** przesuwa odbierane VFO z uwzględnieniem kroku strojenia; samo kliknięcie bez przesunięcia nie zmienia częstotliwości.
- Trzy tryby: **Free Tuning** (swobodne strojenie/przesuwanie), **Center** (fabryczne strojenie z centrowaniem) oraz **Free Fixed** (utrzymanie częstotliwości czerwonego markera podczas przesuwania, aż konieczne będzie strojenie przy krawędzi).
- **Zielony marker podglądu częstotliwości**. Naciśnij i puść **sam Alt**, aby przełączać tryby: **ukryty → informacje tekstowe → tekst i analogowy miernik RF**. Wybór jest zapisywany.
- Dymek markera może podawać częstotliwość, poziom RF w dBFS, modulację, filtr i gain odbiornika (o ile dostępne). Podgląd znika nad elementami interaktywnymi oraz podczas przeciągania i zmiany rozmiaru.
- **Pierwszy LPM poza otwartą listą** na górnym pasku tylko zamyka listę i **nie przestraja**. Następne kliknięcie działa normalnie.
- **Środkowy przycisk myszy (MCK)** nad widmem lub wodospadem kopiuje do schowka Windows tekst np. `4625kHz USB`. **W szarym filtrze** kopiuje aktualnie ustawioną częstotliwość VFO, **poza filtrem** — częstotliwość spod kursora. Dołączana jest modulacja, bez zmiany strojenia i markerów.
- **Prawy przycisk myszy (PPM)** otwiera menu szybkich presetów i zakładek Frequency Manager na widmie i wodospadzie. Uwzględniane są szary filtr i etykiety zakładek.
- **Żółte nazwy zakładek** są klikalne i przestrajają do zapisanej częstotliwości. Rozpoznanie zakładki do edycji/usunięcia wymaga zgodności jej częstotliwości z wybranym markerem lub bezpośredniego kliknięcia etykiety; samo nakładanie się filtrów nie wystarcza.

### Zakładki i szybki wybór 0–9

- Dodawanie, edytowanie i usuwanie **zakładek Frequency Manager** z menu PPM na widmie/wodospadzie. Obsługiwane pola: **kategoria/lista, nazwa, częstotliwość, szerokość filtra i modulacja**. Zapisy są wykonywane przez zgodny Frequency Manager.
- Kompaktowe, przesuwane okna **Dodaj zakładkę** i **Edytuj zakładkę**, z kategorią przed nazwą. Potwierdzanie nadpisania i usunięcia; brak samoczynnego przeskakiwania okien.
- **Dziesięć presetów 0–9**, z których każdy zapamiętuje **częstotliwość w Hz, modulację i szerokość filtra**.
- **Przytrzymaj cyfrę 0–9 z górnego rzędu przez co najmniej 0,6 s**, aby przywołać preset. **Krótkie naciśnięcie nie uruchamia presetu**, tylko przełącza modulację (patrz tabela skrótów).
- Edycja wszystkich dziesięciu wpisów w tabeli zakładki Essential: częstotliwość, modulacja i filtr. Kliknięcie **numeru zajętego presetu** wywołuje potwierdzenie usunięcia.
- Dodawanie/usuwanie presetu również przez menu PPM; wybór numeru i potwierdzenie przed nadpisaniem zajętego miejsca.
- Usunięty preset ma wartości **0 Hz / NFM / 0 Hz**. Zapisane `0 Hz` szerokości filtra oznacza użycie fabrycznej szerokości danego demodulatora przy przywołaniu presetu.
- Presety zapisują się niezależnie od ustawień interfejsu i pozostają po ponownym uruchomieniu SDR++.

## Wszystkie skróty klawiaturowe

Skróty działają, gdy okno SDR++ ma fokus, Essential++ jest włączony, a edytowane pole tekstowe lub okno modalne nie przejmuje klawiszy. Funkcje odbiornika wymagają odpowiedniego aktywnego VFO/źródła. **Cyfry w górnym rzędzie i na numpadzie mają inne przeznaczenie.**

| Klawisz / gest | Działanie |
|---|---|
| `←` / `→` | Strojenie −1 / +1 kHz |
| `Shift+←` / `Shift+→` | Strojenie −10 / +10 kHz |
| `Alt+←` / `Alt+→` | Strojenie −100 / +100 Hz |
| `Ctrl+←` / `Ctrl+→` | Strojenie −100 / +100 kHz |
| `↑` / `↓` | Zwiększenie / zmniejszenie szerokości filtra o 100 Hz |
| `Shift+↑` / `Shift+↓` | Zwiększenie / zmniejszenie gain o jeden krok obsługiwany przez źródło |
| `Alt+↑` / `Alt+↓` | Następna / poprzednia dostępna częstotliwość próbkowania SDR |
| `Ctrl+↑` / `Ctrl+↓` | Przybliżanie / oddalanie względem wybranego VFO |
| **Sam** `Ctrl` (naciśnięcie i puszczenie) | Jednorazowe wycentrowanie na wybranym VFO; bez zmiany trybu strojenia i zoomu |
| **Sam** `Alt` (naciśnięcie i puszczenie) | Przełączanie informacji zielonego markera: ukryty / tekst / tekst + miernik RF |
| `Ctrl+Z` | Cofnięcie o jeden stan w historii strojenia (częstotliwość, modulacja, filtr); „Naprzód” dostępne na pasku |
| `Space` (spacja) | Play / Stop |
| `M` | Wyciszenie / przywrócenie dźwięku wybranego odbiornika |
| `` ` `` / `~` (klawisz pod Esc) | Pokazanie / ukrycie lewego panelu SDR++ |
| **Krótko** `1` (górny rząd) | NFM |
| **Krótko** `2` (górny rząd) | WFM |
| **Krótko** `3` (górny rząd) | AM |
| **Krótko** `4` (górny rząd) | LSB |
| **Krótko** `5` (górny rząd) | USB |
| **Krótko** `6` (górny rząd) | DSB |
| **Krótko** `7` (górny rząd) | CW |
| **Krótko** `8` (górny rząd) | RAW |
| **Krótko** `9` (górny rząd) | Poprzednia modulacja z listy ośmiu trybów |
| **Krótko** `0` (górny rząd) | Następna modulacja z listy ośmiu trybów |
| **Przytrzymanie** `0`–`9` (górny rząd, ≥0,6 s) | Przywołanie odpowiedniego presetu: częstotliwość + modulacja + filtr; bez akcji krótkiego naciśnięcia |
| Numpad `0`–`9` | Wpisywanie cyfr częstotliwości w oknie LCD (nie wybiera presetów) |
| Numpad `.` / separator dziesiętny | Kropka dziesiętna podczas wprowadzania częstotliwości |
| `Enter` / Numpad `Enter` | Zatwierdzenie wpisanej częstotliwości i przestrajanie |
| `Backspace` podczas wpisywania | Usunięcie poprzedniej cyfry |
| `Esc` podczas wpisywania | Anulowanie wpisywania |
| Numpad `+` / `−` | Strojenie +1 / −1 kHz |
| Numpad `*` / `/` | Strojenie +100 / −100 Hz |

### Bezpośrednie wpisywanie częstotliwości

Wpisanie cyfr numpadem otwiera duże okno LCD. Na rozwijanej liście wybierz jednostkę **Hz, kHz, MHz lub GHz**; ostatnia jednostka jest zapamiętywana (domyślnie **MHz**). Można podać część dziesiętną, jeśli wynik jest całkowitą liczbą Hz, np. `4.625` przy MHz oznacza **4 625 000 Hz**. `Enter` lub `Numpad Enter` zatwierdza, `Backspace` kasuje, `Esc` anuluje. Maksymalna częstotliwość: **999 999 999 999 Hz**. Wpisywanie do innych aktywnych pól tekstowych nie powinno uruchamiać strojenia.

## Wygląd i ustawienia

- W zakładce Essential **Lewy panel** i **Prawy panel** mają niezależne ustawienia: **Pokaż**, **Pokaż po najechaniu** i **Ukryj**. Ukrycie paneli zwiększa miejsce na wodospad.
- Lewy panel zawiera fabryczną skalę poziomu sygnału; prawy — **Zoom**, **Speed** (szybkość FFT/odświeżania wodospadu) i **Contrast**. Działają na bieżąco i zachowują ustawienia, gdy jest to obsługiwane.
- **Colour map:** Green, Blue, Red, Orange, Magenta.
- **Language** bezpośrednio pod Colour map: **English** (domyślny), **Deutsch**, **Español**, **Polski**. Zmiana języka jest natychmiastowa i zapisywana.
- Tłumaczone są **wyłącznie elementy Essential++**; fabryczne menu SDR++, inne wtyczki i niezmodyfikowane elementy Frequency Manager nie są tłumaczone.
- **Polskie tłumaczenie samego interfejsu Essential++ celowo nie ma polskich znaków** (np. `Czestotliwosc`), ponieważ standardowy atlas czcionek SDR++ nie zawiera wszystkich potrzebnych glifów. **Dokumentacja na GitHubie korzysta z normalnych polskich znaków.** Niemieckie i hiszpańskie znaki w interfejsie pozostają.
- Ustawienia zachowują m.in. język, motyw LCD, tryb paneli, strefy zegarów, jednostkę wpisywania, szybkość, kontrast, tryb markera i tryb strojenia.

## Instalacja

1. Całkowicie zamknij SDR++.
2. W razie wymiany wcześniejszej instalacji zrób kopię istniejącej `modules/essential_plus_plus.dll` i plików konfiguracyjnych.
3. Skopiuj [`essential_plus_plus.dll`](essential_plus_plus.dll) do folderu SDR++ `modules`.
4. Uruchom zgodną wersję SDR++ i włącz moduł **Essential++**, jeśli jest to wymagane.
5. W zakładce **Essential** ustaw panele, kolory, język oraz presety.

## Konfiguracja i źródła

- `essential_settings.ini` — język, motyw, strefy zegarów, panele, ustawienia widoku i inne preferencje.
- `essential_frequency_shortcuts.ini` — dziesięć presetów z częstotliwością, modulacją i filtrem.
- Frequency Manager przechowuje zakładki w **swojej** natywnej konfiguracji.
- Folder [`Source/`](Source/) zawiera kod C++ oraz skrypt kompilacji. Wymagania: **Python 3, LLVM `clang` i `lld-link`** oraz zgodne pliki/interfejs SDR++ Windows x64.

```sh
python Source/build.py --out build
```

Wynik: `build/essential_plus_plus.dll`. Kod jest powiązany z konkretną kompilacją SDR++; **nie jest** uniwersalnym API wtyczek dla każdej wersji SDR++.

## Weryfikacja i ograniczenia

Wykonano kontrole źródeł, kompilacji i zgodności importów DLL. Dostępność części funkcji zależy od urządzenia, sterownika i używanego modułu SDR++. Działanie interfejsu trzeba sprawdzać w docelowej wersji SDR++ pod Windows; sama udana kompilacja nie gwarantuje zgodności z innymi wydaniami.

## Licencja

**GPL-3.0-only** — [`Source/LICENSE`](Source/LICENSE). Niezależny projekt źródłowy: [SDR++](https://github.com/AlexandreRouma/SDRPlusPlus).
