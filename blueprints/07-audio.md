# Doc 07 — Audio

> Estado: COMPLETO
> Última actualización: 2026-05-16

---

## Ambiente Sonoro General

Pista de audio en loop permanente durante todo el recorrido:

| Parámetro | Valor |
|-----------|-------|
| Sonido | Viento ártico suave — rachas intermitentes |
| Fuente | Freesound.org (CC0) — buscar "arctic wind ambience loop" |
| Posición OpenAL | Fuente global sin atenuación por distancia |
| Volumen | ~30% del máximo — no compite con sonidos de módulos |
| Formato | OGG Vorbis (estándar en desarrollo de videojuegos para tracks largos) |

---

## Sonido de Activación (Tecla E)

Sonido de UI breve al presionar E dentro de una plataforma:

| Parámetro | Valor |
|-----------|-------|
| Sonido | Clic/tono suave de activación (< 0.5 s) |
| Fuente | Freesound.org CC0 |
| Tipo | 2D — sin posición 3D (sonido de interfaz) |
| Formato | WAV PCM (sin compresión — estándar para efectos cortos en desarrollo gráfico) |

---

## Sonidos por Módulo (3D Posicional)

Tres módulos tienen sonido ambiental propio, activo al presionar E:

| Módulo | Sonido | Activación |
|--------|--------|------------|
| M1 izq — El hielo que desaparece | Crujido de hielo al derretirse | Al presionar E |
| M3 izq — El mar que avanza | Agua fluyendo / oleaje | Al presionar E |
| M3 der — La Tierra respira | Brisa entre hojas / naturaleza | Al presionar E |

El resto de módulos sólo tienen el sonido de activación (tecla E), sin audio propio.

Todos los sonidos de módulo en formato **WAV PCM** (efectos cortos).

---

## Configuración de Audio 3D Posicional

Para los 3 módulos con sonido propio:

| Parámetro | Valor |
|-----------|-------|
| Posición de la fuente | Coordenadas del modelo principal del módulo |
| Modelo de atenuación | Lineal con clamping (equivalente a `AL_LINEAR_DISTANCE_CLAMPED`) |
| Distancia de referencia | 8 m (volumen máximo dentro del trigger) |
| Distancia máxima | 20 m (silencio total más allá) |
| Canal | Mono (requerido para posicionamiento 3D correcto) |

El oyente se actualiza cada frame con miniaudio:
```cpp
ma_engine_listener_set_position(&engine, 0, camera.x, camera.y, camera.z);
ma_engine_listener_set_direction(&engine, 0, front.x, front.y, front.z);
ma_engine_listener_set_world_up(&engine, 0, 0.0f, 1.0f, 0.0f);
```

---

## Sin Música de Fondo

No se incluye pista musical — el viento ártico y los sonidos de módulos proveen la atmósfera suficiente sin distraer del contenido educativo.

---

## Formatos de Audio

| Uso | Formato | Justificación |
|-----|---------|---------------|
| Efectos cortos (activación, módulos) | WAV PCM | Sin compresión, carga instantánea, estándar en desarrollo gráfico |
| Ambient largo (viento en loop) | OGG Vorbis | Open source, compresión eficiente, estándar en motores de videojuegos (Unity, Godot, Unreal) |

Carga: **miniaudio** decodifica WAV y OGG de forma nativa (built-in, sin dependencias externas) → streaming o buffer interno gestionado por el engine.

---

## Resumen de Assets de Audio

| Asset | Formato | Fuente |
|-------|---------|--------|
| arctic_wind_loop.ogg | OGG | Freesound.org CC0 |
| activate.wav | WAV | Freesound.org CC0 |
| ice_cracking.wav | WAV | Freesound.org CC0 |
| water_flowing.wav | WAV | Freesound.org CC0 |
| nature_breeze.wav | WAV | Freesound.org CC0 |

Todos los archivos van en `assets/audio/`.

---

## Notas de Implementación

- Inicializar miniaudio con `ma_engine_init(NULL, &engine)` al arranque (single-header, sin DLLs)
- Un único `AudioEngine` con métodos `playGlobal(path)`, `play3D(path, pos)`, `stopAll()`
- El ambient de viento se activa con `ma_sound_set_looping(&wind, MA_TRUE)` al inicio
- Los sonidos de módulo se detienen al salir del trigger (`ma_sound_stop(&sound)`)
- El ambient es reproducido como fuente global (sin spatialization); los módulos usan `ma_sound_set_position()` para posicionamiento 3D
- Sin DLLs que redistribuir — miniaudio es header-only (`src/vendor/miniaudio.h`)
