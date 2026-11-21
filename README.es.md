# Essential++ para SDR++

**Essential++ V40** es un complemento desarrollado por la comunidad para **SDR++ 1.3.2 en Windows x64**. Incorpora una barra de control de estilo LCD, sintonización precisa, controles configurables del espectro y la cascada (waterfall), gestión de receptores y fuentes, medidores de señal, marcadores de frecuencia, diez memorias rápidas, atajos de teclado y ratón y cuatro idiomas para su interfaz.

**Documentación:** [English](README.md) · [Deutsch](README.de.md) · [Español](README.es.md) · [Polski](README.pl.md)

> **Compatibilidad:** La DLL incluida se compiló para la **versión nocturna (nightly) de SDR++ 1.3.2 para Windows x64 del 18 de noviembre de 2026**. Utiliza interfaces internas específicas de SDR++/Dear ImGui y una integración de Frequency Manager dependiente de esa compilación. **No se garantiza la compatibilidad** con otras versiones. Es un proyecto independiente de la comunidad, no un módulo oficial de SDR++.

## Lista completa de funciones

### Barra de control LCD del receptor

- Gran **pantalla segmentada de frecuencia sintonizada** con temas de color LCD. Siguen disponibles la selección de frecuencia y la edición de dígitos originales de SDR++. La interfaz ampliada ofrece frecuencia central e información del receptor.
- Botones **Atrás** y **Adelante** para el historial de sintonización; restauran frecuencia, modo de demodulación y ancho de filtro.
- **Selector de modulación**: NFM, WFM, AM, LSB, USB, DSB, CW y RAW; también permite cambiar con la rueda del ratón.
- **Ancho de filtro**: lectura, edición directa en Hz al pulsar y variación de **100 Hz por paso** de rueda. Se rechazan los valores no válidos.
- **Paso de sintonización**: lectura, edición en Hz enteros y ajuste de **100 Hz por paso** de rueda.
- **Selector de modo de sintonización** mediante el icono original de la barra: **Free Tuning**, **Center** y **Free Fixed**. Cambiar el modo no modifica por sí solo la frecuencia.
- Botones originales **Play/Stop** y **Mute**, con ayudas contextuales; `Space` inicia/detiene la recepción y `M` activa/desactiva el silencio del canal de audio seleccionado.
- **Control de volumen** con porcentaje al pasar el ratón; **control manual de ganancia** con el valor actual al pasar el ratón.
- Controles **Tuner AGC/AGC** y **RTL AGC** si la fuente los admite; las opciones no disponibles permanecen desactivadas. La ganancia manual se desactiva cuando corresponde con AGC activo.
- **Dos relojes independientes** con HH:MM:SS y desplazamientos fijos seleccionables de **UTC−12 a UTC+14**. Pulsa cada reloj para elegir su zona; no hay ajuste automático de horario de verano.
- **Medidores analógicos RF y AUDIO** con lecturas dBFS cuando estén disponibles: RF sigue el marcador rojo sintonizado, mientras AUDIO refleja la salida del receptor elegido. Se mantiene el medidor SNR original de SDR++.
- **Ayudas contextuales** con valores en directo y diseño adaptable al ancho disponible de la barra.
- Cinco esquemas LCD: **Green, Blue, Red, Orange, Magenta**. Los colores de advertencia de conexión conservan su significado.

### Fuentes, dispositivos y receptores de red

- Información de la fuente SDR local y del dispositivo seleccionado en la barra.
- **Selector de dispositivo** bajo la frecuencia de muestreo; permite cambiar entre receptores compatibles durante la recepción con secuencia automática de detener/cambiar/reanudar cuando se necesita.
- **Selector de frecuencia de muestreo (MSPS)** en la barra para fuentes compatibles. Al cambiar, se gestiona el ciclo de recepción y se conserva el zoom visible cuando corresponde.
- Para fuentes de red con controles nativos independientes **Connect/Disconnect**: editor de **host/IP y puerto TCP**, estado de conexión e indicadores de transferencia **DL/UL**.
- **Connect solo establece la conexión y no inicia el flujo de IQ.** **Play/Stop** controla la recepción por separado; **Disconnect** cierra realmente la conexión, no se limita a detener el flujo. Si la fuente no ofrece controles nativos separados, Essential++ no los simula con Play/Stop.
- Un cambio de host/puerto puede requerir reconexión: se detiene la recepción activa si es necesario, pero **no se inicia automáticamente** después.
- Ganancia, AGC, muestreo y selección de dispositivos utilizan los controles nativos compatibles; la disponibilidad depende del hardware, los controladores y los módulos.

### Espectro, cascada y marcadores

- **Ctrl + rueda del ratón** sobre espectro, cascada o escala de frecuencia acerca/aleja alrededor de la posición señalada. **Ctrl + ↑/↓** acerca/aleja alrededor de la frecuencia del VFO seleccionado.
- **Rueda dentro de la zona gris del filtro de recepción** ajusta el ancho del filtro del demodulador **100 Hz por paso**, sin ocultar la cascada.
- Un clic en una zona vacía del espectro/cascada sintoniza la frecuencia señalada. Arrastrar el fondo desplaza la vista en los modos compatibles. Los tiradores originales en los bordes del filtro siguen permitiendo cambiar su ancho.
- Arrastrar **cualquier lugar de la zona gris del filtro** mueve el VFO sintonizado teniendo en cuenta el paso seleccionado; un clic sin arrastrar no cambia la frecuencia.
- Tres modos: **Free Tuning** (sintonización y desplazamiento libres), **Center** (sintonización centrada nativa) y **Free Fixed** (mantiene el marcador rojo en su frecuencia mientras se desplaza, hasta que resulta necesaria una sintonización en el borde).
- **Marcador verde de vista previa** al mover el puntero. Pulsar y soltar **solo Alt** alterna **oculto → información de texto → texto + medidor analógico RF**. Se guarda el modo elegido.
- La ayuda del marcador puede mostrar frecuencia, nivel RF en dBFS, modulación, ancho de filtro y ganancia (si están disponibles). Se oculta sobre controles interactivos y durante arrastres o cambios de tamaño.
- El **primer clic izquierdo fuera de una lista desplegable abierta** de la barra superior solo cierra la lista **sin resintonizar**; otro clic sintoniza normalmente.
- **Botón central del ratón** sobre espectro/cascada copia al portapapeles de Windows, por ejemplo, `4625kHz USB`. **Dentro del filtro gris** usa la frecuencia sintonizada del VFO; **fuera** usa la frecuencia bajo el puntero. Incluye la modulación sin modificar sintonía ni marcadores.
- **Botón derecho** sobre espectro/cascada abre el menú contextual de memorias rápidas y marcadores del Frequency Manager. Reconoce la zona gris y las etiquetas de marcadores.
- **Etiquetas amarillas de marcadores** pulsables para sintonizar su frecuencia guardada. Editar/eliminar exige coincidencia de la frecuencia del marcador con la sintonizada o un clic directo en su etiqueta; la simple superposición de filtros no basta.

### Marcadores y memorias rápidas

- **Añadir, editar o eliminar marcadores del Frequency Manager** desde el menú derecho, con **categoría/lista, nombre, frecuencia, ancho de filtro y modulación**. Guarda mediante el Frequency Manager nativo compatible.
- Ventanas compactas y movibles de **Añadir/Editar**, con categoría antes del nombre; confirmación antes de sobrescribir o eliminar, sin saltos inesperados de posición.
- **Diez memorias rápidas 0–9**, que guardan cada una **frecuencia en Hz, modulación y ancho de filtro**.
- **Mantén pulsada una cifra 0–9 de la fila superior durante al menos 0,6 s** para recuperar su memoria. Una **pulsación breve no recupera** la memoria: cambia la modulación (consulta la tabla).
- Edita las diez memorias en la tabla de Essential (frecuencia, modulación y filtro). Pulsa el **número de una memoria ocupada** para solicitar su eliminación con confirmación.
- También se pueden crear/eliminar memorias con el menú derecho; las ocupadas requieren confirmación antes de sobrescribirse.
- Al borrar una memoria queda **0 Hz / NFM / 0 Hz**; un filtro guardado de 0 Hz significa que al recuperarlo se usa el ancho predeterminado del demodulador.
- Las memorias se guardan por separado de las preferencias de la interfaz y persisten tras reiniciar SDR++.

## Todos los atajos de teclado

Los atajos funcionan cuando la ventana SDR++ tiene el foco, Essential++ está activo y ningún cuadro modal o campo de texto está capturando las teclas. Algunas acciones requieren un VFO/fuente compatible. **Las cifras de la fila superior y del teclado numérico tienen funciones distintas.**

| Tecla / gesto | Acción |
|---|---|
| `←` / `→` | Sintonizar −1 / +1 kHz |
| `Shift+←` / `Shift+→` | Sintonizar −10 / +10 kHz |
| `Alt+←` / `Alt+→` | Sintonizar −100 / +100 Hz |
| `Ctrl+←` / `Ctrl+→` | Sintonizar −100 / +100 kHz |
| `↑` / `↓` | Aumentar / reducir el ancho del filtro 100 Hz |
| `Shift+↑` / `Shift+↓` | Aumentar / reducir la ganancia en un paso admitido por la fuente |
| `Alt+↑` / `Alt+↓` | Siguiente / anterior frecuencia de muestreo SDR disponible |
| `Ctrl+↑` / `Ctrl+↓` | Acercar / alejar el zoom alrededor del VFO seleccionado |
| `Ctrl` **solo** (pulsar y soltar) | Centrar una vez sobre el VFO sin modificar el modo ni el zoom |
| `Alt` **solo** (pulsar y soltar) | Información del marcador verde: oculto / texto / texto + medidor RF |
| `Ctrl+Z` | Retroceder un estado del historial (frecuencia, modulación, filtro); avanzar mediante el botón de la barra |
| `Space` (espacio) | Play / Stop |
| `M` | Silenciar / activar sonido del canal de audio seleccionado |
| `` ` `` / `~` (tecla bajo Esc) | Mostrar / ocultar la barra lateral izquierda de SDR++ |
| **Breve** `1` (fila superior) | NFM |
| **Breve** `2` (fila superior) | WFM |
| **Breve** `3` (fila superior) | AM |
| **Breve** `4` (fila superior) | LSB |
| **Breve** `5` (fila superior) | USB |
| **Breve** `6` (fila superior) | DSB |
| **Breve** `7` (fila superior) | CW |
| **Breve** `8` (fila superior) | RAW |
| **Breve** `9` (fila superior) | Modulación anterior de la secuencia de ocho modos |
| **Breve** `0` (fila superior) | Modulación siguiente de la secuencia de ocho modos |
| **Mantener** `0`–`9` (fila superior, ≥0,6 s) | Recuperar memoria correspondiente: frecuencia + modulación + filtro, sin ejecutar la acción breve |
| Teclado numérico `0`–`9` | Introducir cifras de frecuencia en el panel LCD (no recupera memorias) |
| Teclado numérico `.` / decimal | Punto decimal al introducir frecuencia |
| `Enter` / teclado numérico `Enter` | Confirmar la frecuencia introducida y sintonizar |
| `Backspace` (durante la entrada) | Borrar la cifra anterior |
| `Esc` (durante la entrada) | Cancelar la introducción |
| Teclado numérico `+` / `−` | Sintonizar +1 / −1 kHz |
| Teclado numérico `*` / `/` | Sintonizar +100 / −100 Hz |

### Introducción directa de frecuencia

Al pulsar cifras en el teclado numérico aparece la ventana LCD. Selecciona **Hz, kHz, MHz o GHz** en su desplegable; se recuerda la última unidad (predeterminado inicial: **MHz**). Se permiten decimales si el resultado representa un número entero de Hz: por ejemplo, `4.625` en MHz equivale a **4 625 000 Hz**. Confirma con Enter/Enter numérico, borra con Backspace o cancela con Esc. Frecuencia máxima aceptada: **999 999 999 999 Hz**. No interfiere con otros campos de texto activos.

## Apariencia y preferencias

- En la sección Essential, los paneles **izquierdo** y **derecho** se configuran independientemente: **Show**, **Show on hover** o **Hide**. Ocultarlos amplía el espacio de la cascada.
- El izquierdo incluye la escala nativa de nivel de señal; el derecho ofrece **Zoom**, **Speed** (velocidad de actualización FFT/cascada) y **Contrast**. Estos valores se aplican en directo y se guardan cuando corresponde.
- **Colour map:** Green, Blue, Red, Orange, Magenta.
- **Language**, justo debajo de Colour map: **English** (predeterminado), **Deutsch**, **Español**, **Polski**. El idioma cambia al instante y se recuerda.
- Se traducen **solo los textos de Essential++**; los menús nativos de SDR++, las interfaces de terceros y partes sin modificar de Frequency Manager no se traducen.
- La interfaz **polaca del plugin no usa diacríticos polacos** (p. ej. `Czestotliwosc`) porque el atlas de fuentes de SDR++ no incluye todos los glifos. **Estos archivos de GitHub sí usan Unicode normal.** Se conservan los acentos del alemán y español en el plugin.
- Se guardan, entre otras, idioma, colores LCD, modos de panel, zonas horarias, unidad de entrada, velocidad, contraste, información del marcador y modo de sintonización.

## Instalación

1. Cierra SDR++ por completo.
2. Antes de actualizar, realiza una copia de `modules/essential_plus_plus.dll` y de las configuraciones existentes.
3. Copia [`essential_plus_plus.dll`](essential_plus_plus.dll) al directorio `modules` de SDR++.
4. Ejecuta la versión nocturna compatible de SDR++ y habilita **Essential++** si es necesario.
5. Configura paneles, colores, idioma y memorias en la sección **Essential**.

## Configuración y código fuente

- `essential_settings.ini`: idioma, colores, relojes, paneles y otras preferencias.
- `essential_frequency_shortcuts.ini`: diez memorias de frecuencia, modulación y filtro.
- Frequency Manager guarda sus marcadores en su **propia** configuración nativa.
- [`Source/`](Source/) contiene el código C++ completo y el script de compilación. Se requieren **Python 3, LLVM `clang`, `lld-link`** y la interfaz compatible de SDR++ para Windows x64.

```sh
python Source/build.py --out build
```

DLL resultante: `build/essential_plus_plus.dll`. El código depende de una compilación específica y **no es** una API universal para todas las versiones de SDR++.

## Verificación y limitaciones

Se han revisado el código fuente, la compilación y las importaciones binarias. Algunas funciones dependen del receptor, sus controladores y módulos de SDR++. El comportamiento completo de la GUI debe verificarse en la versión de Windows prevista; compilar correctamente no garantiza compatibilidad con otras ediciones.

## Licencia

**GPL-3.0-only** — [`Source/LICENSE`](Source/LICENSE). Proyecto independiente de referencia: [SDR++](https://github.com/AlexandreRouma/SDRPlusPlus).
