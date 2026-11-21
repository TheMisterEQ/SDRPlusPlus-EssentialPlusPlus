// Essential++ V40 internationalization. Technical IDs are deliberately stable.
// Locales: 0 English, 1 Deutsch, 2 Espanol, 3 Polski.
static int g_language=0;
static bool l10n_key_eq(const char*s,const char*key){
  while(*key&&*s&&*key==*s){s++;key++;}
  return !*key&&(!*s||(s[0]=='#'&&s[1]=='#'));
}
static u32 l10n_hash(const char *s){u32 h=2166136261u;for(;*s && !(s[0]=='#'&&s[1]=='#');++s){h^=(u8)*s;h*=16777619u;}return h;}
static const char* l10n_exact(const char *s){
  if(!s||g_language<0||g_language>3)return s;
  switch(l10n_hash(s)){
    case 0x002a74ddu: if(l10n_key_eq(s,"Nie mozna wyswietlic edytora Frequency Manager.")){static const char* t[4]={"Cannot display the Frequency Manager editor.","Frequency-Manager-Editor kann nicht angezeigt werden.","No se puede mostrar el editor de Frequency Manager.","Nie mozna wyswietlic edytora Frequency Manager."};return t[g_language];}break;
    case 0x0095e4a0u: if(l10n_key_eq(s,"Polacz aktualny klient sieciowy")){static const char* t[4]={"Connect network client","Netzwerkclient verbinden","Conectar cliente de red","Polacz klienta sieciowego"};return t[g_language];}break;
    case 0x01804741u: if(l10n_key_eq(s,"Play: uruchom odbior")){static const char* t[4]={"Play: start receiving","Play: Empfang starten","Play: iniciar recepción","Play: uruchom odbior"};return t[g_language];}break;
    case 0x028a3609u: if(l10n_key_eq(s,"Zamknij")){static const char* t[4]={"Close","Schließen","Cerrar","Zamknij"};return t[g_language];}break;
    case 0x03e6dbe7u: if(l10n_key_eq(s,"Colour map")){static const char* t[4]={"Colour map","Farbpalette","Paleta de colores","Paleta kolorow"};return t[g_language];}break;
    case 0x04836717u: if(l10n_key_eq(s,"Tak")){static const char* t[4]={"Yes","Ja","Sí","Tak"};return t[g_language];}break;
    case 0x094ff9a6u: if(l10n_key_eq(s,"Nie mozna otworzyc edytora zakladek.")){static const char* t[4]={"Cannot open bookmark editor.","Lesezeichen-Editor konnte nicht geöffnet werden.","No se pudo abrir el editor de marcadores.","Nie mozna otworzyc edytora zakladek."};return t[g_language];}break;
    case 0x09d3f50fu: if(l10n_key_eq(s,"Modulacja")){static const char* t[4]={"Modulation","Modulation","Modulación","Modulacja"};return t[g_language];}break;
    case 0x0a6b8020u: if(l10n_key_eq(s,"Speed")){static const char* t[4]={"Speed","Geschwindigkeit","Velocidad","Predkosc"};return t[g_language];}break;
    case 0x0c38639au: if(l10n_key_eq(s,"Automatyczny gain aktywnego Source")){static const char* t[4]={"Automatic gain for active source","Automatische Verstärkung der aktiven Quelle","Ganancia automatica de la fuente","Automatyczne wzmocnienie aktywnego zrodla"};return t[g_language];}break;
    case 0x147b1a30u: if(l10n_key_eq(s,"Krok: ")){static const char* t[4]={"Step: ","Schritt: ","Paso: ","Krok: "};return t[g_language];}break;
    case 0x1482c757u: if(l10n_key_eq(s,"Wstecz: czestotliwosc, modulacja i filtr (Ctrl+Z)")){static const char* t[4]={"Back: frequency, mode, filter (Ctrl+Z)","Zurueck: Frequenz, Modus, Filter (Ctrl+Z)","Atras: frecuencia, modo y filtro (Ctrl+Z)","Wstecz: czestotliwosc, modulacja i filtr (Ctrl+Z)"};return t[g_language];}break;
    case 0x160d296au: if(l10n_key_eq(s,"Stop: zatrzymaj odbior")){static const char* t[4]={"Stop: stop receiving","Stop: Empfang stoppen","Stop: detener recepción","Stop: zatrzymaj odbior"};return t[g_language];}break;
    case 0x1aae5d27u: if(l10n_key_eq(s,"Usun szybki wybor nr ")){static const char* t[4]={"Remove quick preset no. ","Schnellwahl Nr. ","Eliminar acceso rápido n.º ","Usun szybki wybor nr "};return t[g_language];}break;
    case 0x1cf3b04cu: if(l10n_key_eq(s,"Wybrany klient nie udostepnia polaczenia.")){static const char* t[4]={"Selected client cannot connect.","Ausgewaehlter Client kann nicht verbinden.","El cliente seleccionado no puede conectar.","Wybrany klient nie udostepnia polaczenia."};return t[g_language];}break;
    case 0x1e5ad77du: if(l10n_key_eq(s,"Volume: ")){static const char* t[4]={"Volume: ","Lautstärke: ","Volumen: ","Glosnosc: "};return t[g_language];}break;
    case 0x1fe2bae1u: if(l10n_key_eq(s," - zajety")){static const char* t[4]={" - occupied"," - belegt"," - ocupado"," - zajety"};return t[g_language];}break;
    case 0x2456b993u: if(l10n_key_eq(s,"Contrast")){static const char* t[4]={"Contrast","Kontrast","Contraste","Kontrast"};return t[g_language];}break;
    case 0x2547bbbcu: if(l10n_key_eq(s,"Brak wybranej zakladki.")){static const char* t[4]={"No bookmark selected.","Kein Lesezeichen ausgewählt.","Ningun marcador seleccionado.","Brak wybranej zakladki."};return t[g_language];}break;
    case 0x294ce0c1u: if(l10n_key_eq(s,"ESSENTIAL  /  PODGLAD")){static const char* t[4]={"ESSENTIAL / PREVIEW","ESSENTIAL / VORSCHAU","ESSENTIAL / VISTA","ESSENTIAL / PODGLAD"};return t[g_language];}break;
    case 0x2c02710eu: if(l10n_key_eq(s,"Ukryj")){static const char* t[4]={"Hide","Ausblenden","Ocultar","Ukryj"};return t[g_language];}break;
    case 0x2db97c52u: if(l10n_key_eq(s,"Czestotliwosc [Hz]")){static const char* t[4]={"Frequency [Hz]","Frequenz [Hz]","Frecuencia [Hz]","Czestotliwosc [Hz]"};return t[g_language];}break;
    case 0x329a6ba5u: if(l10n_key_eq(s," - wolny")){static const char* t[4]={" - free"," - frei"," - libre"," - wolny"};return t[g_language];}break;
    case 0x36e4d562u: if(l10n_key_eq(s,"Nie mozna otworzyc edytora zakladki.")){static const char* t[4]={"Cannot open bookmark editor.","Lesezeichen-Editor nicht verfügbar.","No se pudo abrir el editor de marcadores.","Nie mozna otworzyc edytora zakladki."};return t[g_language];}break;
    case 0x38bc319fu: if(l10n_key_eq(s,"Wybierz liste zakladki w Frequency Manager.")){static const char* t[4]={"Select the bookmark list in Frequency Manager.","Lesezeichenliste im Frequency Manager wählen.","Selecciona la lista en Frequency Manager.","Wybierz liste zakladki w Frequency Manager."};return t[g_language];}break;
    case 0x3eadfd25u: if(l10n_key_eq(s,"Kliknij, aby zmienic adres IP i port. Strzalki: transfer DL/UL.")){static const char* t[4]={"Click to change IP and port. Arrows: DL/UL traffic.","Klicken, um IP und Port zu aendern. Pfeile: DL/UL.","Pulsa para cambiar IP y puerto. Flechas: DL/UL.","Kliknij, aby zmienic IP i port. Strzalki: transfer DL/UL."};return t[g_language];}break;
    case 0x3f8effa7u: if(l10n_key_eq(s,"Dodaj do szybkiego wyboru")){static const char* t[4]={"Add to quick access","Zu Schnellwahl hinzufügen","Añadir a acceso rápido","Dodaj do szybkiego wyboru"};return t[g_language];}break;
    case 0x42de5052u: if(l10n_key_eq(s," (AGC aktywne)")){static const char* t[4]={" (AGC enabled)"," (AGC aktiv)"," (AGC activo)"," (AGC aktywne)"};return t[g_language];}break;
    case 0x431dc3a8u: if(l10n_key_eq(s,"Free Tuning")){static const char* t[4]={"Free tuning","Freies Abstimmen","Sintonía libre","Swobodne strojenie"};return t[g_language];}break;
    case 0x43746383u: if(l10n_key_eq(s,"Szybki dostep (kliknij numer, aby usunac):")){static const char* t[4]={"Quick access (click number to delete):","Schnellzugriff (Nummer zum Löschen anklicken):","Acceso rápido (clic en número para borrar):","Szybki dostep (kliknij numer, aby usunac):"};return t[g_language];}break;
    case 0x44a37323u: if(l10n_key_eq(s,"Mod")){static const char* t[4]={"Mode","Mod.","Modo","Mod."};return t[g_language];}break;
    case 0x456d4a5du: if(l10n_key_eq(s," (zajety)")){static const char* t[4]={" (occupied)"," (belegt)"," (ocupado)"," (zajety)"};return t[g_language];}break;
    case 0x46e69399u: if(l10n_key_eq(s,"Nr")){static const char* t[4]={"No.","Nr.","N.º","Nr"};return t[g_language];}break;
    case 0x4acf453cu: if(l10n_key_eq(s,"Anuluj")){static const char* t[4]={"Cancel","Abbrechen","Cancelar","Anuluj"};return t[g_language];}break;
    case 0x4b9fbea4u: if(l10n_key_eq(s,"Center")){static const char* t[4]={"Center","Zentrieren","Centrar","Centruj"};return t[g_language];}break;
    case 0x512d6c74u: if(l10n_key_eq(s,"Usun wybrana zakladke?")){static const char* t[4]={"Delete selected bookmark?","Ausgewaehltes Lesezeichen löschen?","Eliminar el marcador seleccionado?","Usunac wybrana zakladke?"};return t[g_language];}break;
    case 0x51427b1fu: if(l10n_key_eq(s,"Polaczenie zostalo przerwane.")){static const char* t[4]={"Connection interrupted.","Verbindung unterbrochen.","Conexión interrumpida.","Polaczenie zostalo przerwane."};return t[g_language];}break;
    case 0x570c5043u: if(l10n_key_eq(s,"Moc sygnalu: ")){static const char* t[4]={"Signal power: ","Signalpegel: ","Potencia de señal: ","Moc sygnalu: "};return t[g_language];}break;
    case 0x5805f9e5u: if(l10n_key_eq(s,"Nie mozna odczytac kategorii Frequency Manager.")){static const char* t[4]={"Cannot read Frequency Manager categories.","Kategorien konnten nicht gelesen werden.","No se pudieron leer las categorías.","Nie mozna odczytac kategorii Frequency Manager."};return t[g_language];}break;
    case 0x5ab7faccu: if(l10n_key_eq(s,"Lewy panel")){static const char* t[4]={"Left panel","Linkes Panel","Panel izquierdo","Lewy panel"};return t[g_language];}break;
    case 0x5e6e14aau: if(l10n_key_eq(s,"Zapisz")){static const char* t[4]={"Save","Speichern","Guardar","Zapisz"};return t[g_language];}break;
    case 0x5ffad28au: if(l10n_key_eq(s,"KROK")){static const char* t[4]={"STEP","SCHRITT","PASO","KROK"};return t[g_language];}break;
    case 0x60e80507u: if(l10n_key_eq(s,"Usun z szybkiego wyboru")){static const char* t[4]={"Remove from quick access","Aus Schnellwahl entfernen","Quitar de acceso rápido","Usun z szybkiego wyboru"};return t[g_language];}break;
    case 0x628f4c57u: if(l10n_key_eq(s,"Skrot jest zajety. Nadpisac?")){static const char* t[4]={"Preset occupied. Overwrite?","Schnellwahl belegt. Überschreiben?","Acceso ocupado. Sobrescribir?","Skrot jest zajety. Nadpisac?"};return t[g_language];}break;
    case 0x64565107u: if(l10n_key_eq(s,"Otworz liste tej zakladki w Frequency Manager.")){static const char* t[4]={"Open this bookmark list in Frequency Manager.","Diese Lesezeichenliste im Frequency Manager oeffnen.","Abre la lista del marcador en Frequency Manager.","Otworz liste tej zakladki w Frequency Manager."};return t[g_language];}break;
    case 0x65363922u: if(l10n_key_eq(s,"Rozlacz aktywny klient sieciowy")){static const char* t[4]={"Disconnect network client","Netzwerkclient trennen","Desconectar cliente de red","Rozlacz aktywnego klienta sieciowego"};return t[g_language];}break;
    case 0x6618ff39u: if(l10n_key_eq(s,"Usun zakladke")){static const char* t[4]={"Delete bookmark","Lesezeichen löschen","Eliminar marcador","Usun zakladke"};return t[g_language];}break;
    case 0x670bcbc5u: if(l10n_key_eq(s,"Czy nadpisac istniejaca zakladke?")){static const char* t[4]={"Overwrite existing bookmark?","Vorhandenes Lesezeichen überschreiben?","Sobrescribir el marcador existente?","Czy nadpisac istniejaca zakladke?"};return t[g_language];}break;
    case 0x68b793ddu: if(l10n_key_eq(s,"Szerokosc pasma [kHz]")){static const char* t[4]={"Bandwidth [kHz]","Bandbreite [kHz]","Ancho de banda [kHz]","Szerokosc pasma [kHz]"};return t[g_language];}break;
    case 0x6b69142fu: if(l10n_key_eq(s,"Pokaz po najechaniu")){static const char* t[4]={"Show on hover","Bei Beruehrung anzeigen","Mostrar al pasar el cursor","Pokaz po najechaniu"};return t[g_language];}break;
    case 0x725ad0c2u: if(l10n_key_eq(s,"Aktywne Source nie udostepnia RTL AGC")){static const char* t[4]={"Active source has no RTL AGC control","Aktive Quelle ohne RTL AGC","La fuente no permite RTL AGC","Aktywne zrodlo nie udostepnia RTL AGC"};return t[g_language];}break;
    case 0x792e9eb7u: if(l10n_key_eq(s,"Nie")){static const char* t[4]={"No","Nein","No","Nie"};return t[g_language];}break;
    case 0x7b594f49u: if(l10n_key_eq(s,"Aktywne Source nie udostepnia kontrolki AGC")){static const char* t[4]={"Active source has no AGC control","Aktive Quelle ohne AGC","La fuente no ofrece control AGC","Aktywne zrodlo nie udostepnia AGC"};return t[g_language];}break;
    case 0x7b9c1994u: if(l10n_key_eq(s,"Nieprawidlowa czestotliwosc lub szerokosc filtra.")){static const char* t[4]={"Invalid frequency or filter bandwidth.","Ungültige Frequenz oder Bandbreite.","Frecuencia o ancho de banda no válido.","Nieprawidlowa czestotliwosc lub szerokosc filtra."};return t[g_language];}break;
    case 0x86e77e7du: if(l10n_key_eq(s,"Podaj dodatnia szerokosc w Hz.")){static const char* t[4]={"Enter a positive bandwidth in Hz.","Positive Bandbreite in Hz eingeben.","Introduce un ancho de banda positivo en Hz.","Podaj dodatnia szerokosc w Hz."};return t[g_language];}break;
    case 0x8923d81fu: if(l10n_key_eq(s,"Usunac szybki dostep nr ")){static const char* t[4]={"Delete quick preset no. ","Schnellwahl Nr. ","Eliminar acceso rápido n.º ","Usunac szybki dostep nr "};return t[g_language];}break;
    case 0x8add9cd1u: if(l10n_key_eq(s,"Nieprawidlowy adres IP lub host[:port].")){static const char* t[4]={"Invalid IP or host[:port].","Ungültige IP oder host[:port].","IP o host[:puerto] incorrecto.","Nieprawidlowy adres IP lub host[:port]."};return t[g_language];}break;
    case 0x8b4dfd9bu: if(l10n_key_eq(s,"Ta czestotliwosc jest juz zapisana.")){static const char* t[4]={"This frequency is already bookmarked.","Diese Frequenz ist bereits gespeichert.","Esta frecuencia ya esta guardada.","Ta czestotliwosc jest juz zapisana."};return t[g_language];}break;
    case 0x910e5566u: if(l10n_key_eq(s," (reczne sterowanie)")){static const char* t[4]={" (manual control)"," (manuell)"," (control manual)"," (sterowanie reczne)"};return t[g_language];}break;
    case 0x9138b5afu: if(l10n_key_eq(s,"Dodaj do zakladek")){static const char* t[4]={"Add bookmark","Lesezeichen hinzufügen","Añadir marcador","Dodaj do zakladek"};return t[g_language];}break;
    case 0x9a73db9bu: if(l10n_key_eq(s,"Language")){static const char* t[4]={"Language","Sprache","Idioma","Jezyk"};return t[g_language];}break;
    case 0x9b80731eu: if(l10n_key_eq(s,"Gain")){static const char* t[4]={"Gain","Verstärkung","Ganancia","Wzmocnienie"};return t[g_language];}break;
    case 0xa568e156u: if(l10n_key_eq(s,"RF: moc przy czestotliwosci czerwonego markera, dBFS")){static const char* t[4]={"RF: power at the red marker, dBFS","HF: Signalpegel am roten Marker, dBFS","RF: potencia en el marcador rojo, dBFS","RF: moc przy czestotliwosci czerwonego markera, dBFS"};return t[g_language];}break;
    case 0xa6987a7cu: if(l10n_key_eq(s,"ZONE 1")){static const char* t[4]={"ZONE 1","ZONE 1","ZONA 1","STREFA 1"};return t[g_language];}break;
    case 0xa80232d6u: if(l10n_key_eq(s,"FILTR")){static const char* t[4]={"FILTER","FILTER","FILTRO","FILTR"};return t[g_language];}break;
    case 0xa995f8a3u: if(l10n_key_eq(s," (wolny)")){static const char* t[4]={" (free)"," (frei)"," (libre)"," (wolny)"};return t[g_language];}break;
    case 0xa9987f35u: if(l10n_key_eq(s,"ZONE 2")){static const char* t[4]={"ZONE 2","ZONE 2","ZONA 2","STREFA 2"};return t[g_language];}break;
    case 0xa9fbe64au: if(l10n_key_eq(s,"AUDIO: szczytowy poziom wybranego odbiornika po regulacji glosnosci, dBFS")){static const char* t[4]={"AUDIO: peak level after volume control, dBFS","AUDIO: Spitzenpegel nach Lautstärkeregelung, dBFS","AUDIO: nivel pico tras ajustar volumen, dBFS","AUDIO: szczytowy poziom po regulacji glosnosci, dBFS"};return t[g_language];}break;
    case 0xaada07c0u: if(l10n_key_eq(s,"Otworz liste zakladki w Frequency Manager.")){static const char* t[4]={"Open the bookmark list in Frequency Manager.","Lesezeichenliste im Frequency Manager oeffnen.","Abre la lista en Frequency Manager.","Otworz liste zakladki w Frequency Manager."};return t[g_language];}break;
    case 0xab309cf5u: if(l10n_key_eq(s,"Podaj poprawny adres i port (1-65535).")){static const char* t[4]={"Enter a valid address and port (1–65535).","Gültige Adresse und Port (1–65535) eingeben.","Introduce dirección y puerto validos (1–65535).","Podaj poprawny adres i port (1-65535)."};return t[g_language];}break;
    case 0xaf71fa45u: if(l10n_key_eq(s,"Mute: wycisz / wlacz dzwiek")){static const char* t[4]={"Mute: silence / restore audio","Mute: Ton aus / ein","Silencio: activar / desactivar audio","Mute: wycisz / wlacz dzwiek"};return t[g_language];}break;
    case 0xb1bc45a0u: if(l10n_key_eq(s,"Adres IP / host")){static const char* t[4]={"IP address / host","IP-Adresse / Host","Dirección IP / host","Adres IP / host"};return t[g_language];}break;
    case 0xb334a25du: if(l10n_key_eq(s,"Modulacja: ")){static const char* t[4]={"Mode: ","Modus: ","Modo: ","Modulacja: "};return t[g_language];}break;
    case 0xb3d9bb2du: if(l10n_key_eq(s,"Edytuj zakladke")){static const char* t[4]={"Edit bookmark","Lesezeichen bearbeiten","Editar marcador","Edytuj zakladke"};return t[g_language];}break;
    case 0xb65e9e9eu: if(l10n_key_eq(s,"Pokaz")){static const char* t[4]={"Show","Anzeigen","Mostrar","Pokaz"};return t[g_language];}break;
    case 0xb6daac0eu: if(l10n_key_eq(s,"MOC RF")){static const char* t[4]={"RF POWER","HF PEGEL","POTENCIA RF","MOC RF"};return t[g_language];}break;
    case 0xb812cd2bu: if(l10n_key_eq(s,"Wybrane Source nie udostepnia listy odbiornikow SDR")){static const char* t[4]={"Selected source has no SDR device list","Ausgewaehlte Quelle hat keine SDR-Geraeteliste","La fuente no ofrece lista de dispositivos SDR","Wybrane zrodlo nie udostepnia listy odbiornikow SDR"};return t[g_language];}break;
    case 0xb9fbd104u: if(l10n_key_eq(s,"Prawy panel")){static const char* t[4]={"Right panel","Rechtes Panel","Panel derecho","Prawy panel"};return t[g_language];}break;
    case 0xbc064b77u: if(l10n_key_eq(s,"Nie mozna przywrocic kategorii.")){static const char* t[4]={"Could not restore the category.","Kategorie konnte nicht wiederhergestellt werden.","No se pudo restaurar la categoría.","Nie mozna przywrocic kategorii."};return t[g_language];}break;
    case 0xbd474897u: if(l10n_key_eq(s,"Zniknie czestotliwosc, modulacja i filtr tego numeru.")){static const char* t[4]={"Frequency, mode and filter for this preset will be cleared.","Frequenz, Modulation und Filter werden geloescht.","Se borrarán frecuencia, modo y filtro de este acceso.","Zniknie czestotliwosc, modulacja i filtr tego numeru."};return t[g_language];}break;
    case 0xbf66b3ecu: if(l10n_key_eq(s,"Filtr: ")){static const char* t[4]={"Filter: ","Filter: ","Filtro: ","Filtr: "};return t[g_language];}break;
    case 0xc7a94c25u: if(l10n_key_eq(s,"Free Fixed")){static const char* t[4]={"Free fixed","Frei fixiert","Libre fijo","Swobodny zablokowany"};return t[g_language];}break;
    case 0xc8da6ea5u: if(l10n_key_eq(s,"Podaj dodatni krok w pelnych Hz.")){static const char* t[4]={"Enter a positive integer step in Hz.","Positiven ganzzahligen Schritt in Hz eingeben.","Indica un paso entero positivo en Hz.","Podaj dodatni krok w pelnych Hz."};return t[g_language];}break;
    case 0xc9ce3ddau: if(l10n_key_eq(s,"Numer: ")){static const char* t[4]={"Number: ","Nummer: ","Número: ","Numer: "};return t[g_language];}break;
    case 0xcbe11a58u: if(l10n_key_eq(s,"Kategoria")){static const char* t[4]={"Category","Kategorie","Categoría","Kategoria"};return t[g_language];}break;
    case 0xd1f50af7u: if(l10n_key_eq(s,"Usun szybki dostep")){static const char* t[4]={"Delete quick preset","Schnellwahl löschen","Eliminar acceso rápido","Usun szybki dostep"};return t[g_language];}break;
    case 0xd68d6635u: if(l10n_key_eq(s,"Krok strojenia [Hz]")){static const char* t[4]={"Tuning step [Hz]","Abstimmschritt [Hz]","Paso de sintonia [Hz]","Krok strojenia [Hz]"};return t[g_language];}break;
    case 0xda84790bu: if(l10n_key_eq(s,"Disconnect")){static const char* t[4]={"Disconnect","Trennen","Desconectar","Rozlacz"};return t[g_language];}break;
    case 0xdb89a899u: if(l10n_key_eq(s,"Connect")){static const char* t[4]={"Connect","Verbinden","Conectar","Polacz"};return t[g_language];}break;
    case 0xde3aaf8fu: if(l10n_key_eq(s,"MODULACJA")){static const char* t[4]={"MODE","MODUS","MODO","MODULACJA"};return t[g_language];}break;
    case 0xe215e4c9u: if(l10n_key_eq(s,"Port TCP")){static const char* t[4]={"TCP port","TCP-Port","Puerto TCP","Port TCP"};return t[g_language];}break;
    case 0xe5f3cb96u: if(l10n_key_eq(s,"Filtr")){static const char* t[4]={"Filter","Filter","Filtro","Filtr"};return t[g_language];}break;
    case 0xe92b94c2u: if(l10n_key_eq(s,"Naprzod: czestotliwosc, modulacja i filtr")){static const char* t[4]={"Forward: frequency, mode, filter","Vor: Frequenz, Modus, Filter","Adelante: frecuencia, modo y filtro","Naprzod: czestotliwosc, modulacja i filtr"};return t[g_language];}break;
    case 0xeaf9cca2u: if(l10n_key_eq(s,"Czestotliwosc: ")){static const char* t[4]={"Frequency: ","Frequenz: ","Frecuencia: ","Czestotliwosc: "};return t[g_language];}break;
    case 0xeb6fa0f8u: if(l10n_key_eq(s,"Brak aktywnego klienta sieciowego.")){static const char* t[4]={"No active network client.","Kein aktiver Netzwerkclient.","No hay cliente de red activo.","Brak aktywnego klienta sieciowego."};return t[g_language];}break;
    case 0xecc38c18u: if(l10n_key_eq(s,"Nie polaczono. Sprawdz adres, port i serwer.")){static const char* t[4]={"Not connected. Check address, port and server.","Nicht verbunden. Adresse, Port und Server prüfen.","Sin conexión. Comprueba dirección, puerto y servidor.","Nie polaczono. Sprawdz adres, port i serwer."};return t[g_language];}break;
    case 0xf16385e8u: if(l10n_key_eq(s,"RTL AGC aktywnego Source")){static const char* t[4]={"RTL AGC for active source","RTL AGC der aktiven Quelle","RTL AGC de la fuente activa","RTL AGC aktywnego zrodla"};return t[g_language];}break;
    case 0xf436fc16u: if(l10n_key_eq(s,"MSPS: czestotliwosc probkowania aktywnego Source")){static const char* t[4]={"MSPS: active source sample rate","MSPS: Abtastrate der aktiven Quelle","MSPS: frecuencia de muestreo de la fuente","MSPS: czestotliwosc probkowania aktywnego zrodla"};return t[g_language];}break;
    case 0xf6d6d81cu: if(l10n_key_eq(s,"Filtr [Hz]")){static const char* t[4]={"Filter [Hz]","Filter [Hz]","Filtro [Hz]","Filtr [Hz]"};return t[g_language];}break;
    case 0xab17e5c2u: if(l10n_key_eq(s,"W tej kategorii istnieje juz taka zakladka.")){static const char* t[4]={"This category already contains a bookmark with that name.","In dieser Kategorie gibt es bereits ein Lesezeichen mit diesem Namen.","Esta categoría ya contiene un marcador con ese nombre.","W tej kategorii istnieje juz taka zakladka."};return t[g_language];}break;
    case 0xc9fcbba3u: if(l10n_key_eq(s,"Zmiana kategorii jest niedostepna dla tego builda.")){static const char* t[4]={"Category change is not supported by this build.","Kategoriewechsel wird von diesem Build nicht unterstützt.","El cambio de categoría no esta disponible en esta version.","Zmiana kategorii jest niedostepna dla tej kompilacji."};return t[g_language];}break;
    case 0x59d938c8u: if(l10n_key_eq(s,"Nie mozna odczytac list z Frequency Manager.")){static const char* t[4]={"Cannot read Frequency Manager lists.","Frequency-Manager-Listen koennen nicht gelesen werden.","No se pueden leer las listas de Frequency Manager.","Nie mozna odczytac list z Frequency Manager."};return t[g_language];}break;
    case 0x66a54a6eu: if(l10n_key_eq(s,"Frequency Manager nie jest aktywny lub jego build jest niezgodny.")){static const char* t[4]={"Frequency Manager is inactive or incompatible.","Frequency Manager ist inaktiv oder inkompatibel.","Frequency Manager esta inactivo o no es compatible.","Frequency Manager jest nieaktywny lub niezgodny."};return t[g_language];}break;
    case 0xb8af8ea8u: if(l10n_key_eq(s,"Brak zgodnego interfejsu Frequency Manager.")){static const char* t[4]={"No compatible Frequency Manager interface.","Keine kompatible Frequency-Manager-Schnittstelle.","No hay interfaz compatible de Frequency Manager.","Brak zgodnego interfejsu Frequency Manager."};return t[g_language];}break;
    case 0x98cf2388u: if(l10n_key_eq(s,"To zrodlo nie oferuje osobnego Connect/Disconnect. Uzyj Play/Stop.")){static const char* t[4]={"This source does not offer separate Connect/Disconnect controls. Use Play/Stop.","Diese Quelle hat keine getrennten Connect/Disconnect-Befehle. Play/Stop verwenden.","Esta fuente no ofrece Connect/Disconnect independientes. Usa Play/Stop.","To zrodlo nie oferuje osobnego Connect/Disconnect. Uzyj Play/Stop."};return t[g_language];}break;
    case 0x19c0feb5u: if(l10n_key_eq(s,"Nie mozna rozlaczyc klienta. Sprawdz natywny Source.")){static const char* t[4]={"Could not disconnect the client. Check native Source controls.","Client konnte nicht getrennt werden. Native Source-Steuerung pruefen.","No se pudo desconectar el cliente. Revisa los controles Source originales.","Nie mozna rozlaczyc klienta. Sprawdz natywny Source."};return t[g_language];}break;
    default:break;
  }return s;
}
static const char* l10n(const char* s){
  if(!s)return "";
  const char*split=nullptr;for(const char*p=s;*p;p++)if(p[0]=='#'&&p[1]=='#'){split=p;break;}
  if(!split)return l10n_exact(s);
  const char*translated=l10n_exact(s);if(translated==s)return s;
  static char scratch[16][512];static int next=0;char*out=scratch[next++&15];
  c_copy(out,512,translated);c_append(out,512,split);return out;
}
static void l10n_TextUnformatted(const char* s,const char*end){if(end){ui_TextUnformatted(s,end);return;}ui_TextUnformatted(l10n(s),nullptr);}
static bool l10n_Button(const char*label,const ImVec2&sz){return ui_Button(l10n(label),sz);}
static bool l10n_Selectable(const char*label,bool selected,int flags,const ImVec2&sz){return ui_Selectable(l10n(label),selected,flags,sz);}
static bool l10n_Checkbox(const char*label,bool*checked){return ui_Checkbox(l10n(label),checked);}
static void l10n_TableSetupColumn(const char*label,int flags,float weight,u32 id){ui_TableSetupColumn(l10n(label),flags,weight,id);}
// Macros only apply within the Essential++ compilation unit, never to core exports.
#define ui_TextUnformatted l10n_TextUnformatted
#define ui_Button l10n_Button
#define ui_Selectable l10n_Selectable
#define ui_Checkbox l10n_Checkbox
#define ui_TableSetupColumn l10n_TableSetupColumn
